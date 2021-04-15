/*
 * ws2812b.c
 *
 *  Created on: Nov 15, 2020
 *      Author: halavich
 */

#include <stdbool.h>
#include <stdlib.h>

#include "log.h"
#include "neon.h"
#include "state-machine.h"
#include "ws2812b.h"

/*--------------------------------------------------------------------------- */
#define DELAY_LEN   50
#define HIGH        72 /* eth 58/ KS 72 */
#define LOW         20 /* eth 28/ KS 20 */

#define CHECK_BIT(reg, bit)  ((reg & (1 << bit)) != 0)

#define WS_BIT_POS(pixel_pos, subpixel_pos, bit)                                \
    (GET_BIT_POS(pixel_pos, subpixel_pos, bit) + DELAY_LEN)

/*--------------------------------------------------------------------------- */
/* Reader-related data */
#define IS_NEW_PIXEL(n)                 (!((n) % BPP))
#define IS_NEW_SUBPIXEL(n)              (!((n) % BPSP))
#define PIXEL_INDEX(n)                  ((n) / BPP)

/* Parser */
#define BUF_TO_BIT(buf, bit)            (buf[7 - bit] << bit)
#define PIXEL_OFFSET(buf, pixel)        (buf + (pixel * BPP))
#define SUBP_OFFSET(buf, subpixel)      (buf + (subpixel * BPSP))

/*--------------------------------------------------------------------------- */

static ws28_data_st_t *active_ws28_channel = NULL;

/* Active reader data (for callback) */
static TIM_TypeDef *active_eof_tim = NULL;
static GPIO_TypeDef *active_gpio_port = NULL;
static EXTI_TypeDef *active_exti = NULL;
static uint32_t active_gpio_pin;
static bool *active_frame_buf = NULL;
static uint32_t active_frame_buf_len;

uint32_t input_bits_cnt;

static
void set_reader_callback_data(ws28_reader_data_st_t *ws28_reader_data)
{
    active_eof_tim = ws28_reader_data->cmsis_eof_timer_ptr;
    active_gpio_port = ws28_reader_data->cmsis_input_gpio_port;
    active_exti = ws28_reader_data->cmsis_exti_irq_ptr;
    active_gpio_pin = ws28_reader_data->gpio_pin_mask;
    active_frame_buf = ws28_reader_data->frame_buf;
    active_frame_buf_len = ws28_reader_data->frame_buf_len;
}

void ws28_reader_init(ws28_reader_data_st_t *ws28_reader_data)
{
    if (NULL == ws28_reader_data) {
        log_err("Reader data ptr is null\n");

        Error_Handler();
    } else if (NULL == ws28_reader_data->cmsis_eof_timer_ptr ||
               NULL == ws28_reader_data->cmsis_exti_irq_ptr ||
               NULL == ws28_reader_data->cmsis_input_gpio_port ||
               NULL == ws28_reader_data->frame_buf) {
        log_err("Reader data content ptr is null\n");

        Error_Handler();
    }

    set_reader_callback_data(ws28_reader_data);

    /* Init EOF Timer. Enable global interrupt */
    active_eof_tim->DIER |= TIM_DIER_UIE;
}

void ws28_init(ws28_data_st_t *ws28_data)
{
    uint16_t i;

    if (NULL == ws28_data) {
        log_err("ws28_data is NULL\n");

        return;
    }

    ws28_data->finished_writing = true;
    ws28_data->pixel_buf_len = (DELAY_LEN + ws28_data->pixel_in_strip * BPP);

    ws28_data->pixel_buf = calloc(ws28_data->pixel_buf_len,
                                  sizeof(pixel_buf_t));
    if (NULL == ws28_data->pixel_buf) {
        log_err("Can't alloc memory for pixel_buf\n");

        return;
    }

    for (i = DELAY_LEN; i < ws28_data->pixel_buf_len; i++) {
        ws28_data->pixel_buf[i] = LOW;
    }
}

void ws28_deinit(ws28_data_st_t *ws28_data)
{
    free(ws28_data->pixel_buf);
}

void print_buff(bool *frame_buf, int bit_cnt)
{
    for (int i = 0; i < bit_cnt; i++) {
        if (IS_NEW_SUBPIXEL(i)) {
            if (IS_NEW_PIXEL(i)) {
                system_log(4, "\nPixel [%d]: ", PIXEL_INDEX(i));
            } else {
                system_log(4, " ");
            }
        }

        system_log(4, "%d", frame_buf[i]);
    }

    system_log(4, "\n\n");
}

extern bool *frame_buf;

/* Check with inline */
inline void ws28_irq_reader_callback(void)
{
#if 0 /* Isn't fast enough */
    active_exti->PR = active_gpio_pin; /* Reset pending bit */
    active_eof_tim->CR1 |= !input_bits_cnt; /* Start counter */
    active_eof_tim->CNT = 2; /* Reset counter */

    active_frame_buf[input_bits_cnt++] =
        !!(active_gpio_port->IDR & active_gpio_pin); /* mov.w ldr ldr str */
#endif

    EXTI->PR = GPIO_PIN_1;        /* Reset pending bit 0x4001 0400  + 0x34 */
    TIM3->CR1 |= !input_bits_cnt; /* Start counter */
    TIM3->CNT = 2;                /* Reset counter */

    frame_buf[input_bits_cnt++] = !!(GPIOA->IDR & WS28_CH1_Input_Pin); /* mov.w ldr ldr str */
}

void ws28_eof_timer_callback()
{
    int rc = 0;

    /* Stop EOF timer */
    active_eof_tim->CR1 &= ~TIM_CR1_CEN; /* Stop counter */
    active_eof_tim->CNT = 0;             /* Reset counter */

    if (0 == input_bits_cnt) {
        /* Investigate! Situation of misstrigger. */
        HAL_GPIO_WritePin(Cycle_LED_GPIO_Port, Cycle_LED_Pin, 1);

        return;
    }

    printf("Got: %d bits\n", input_bits_cnt);

    if (active_frame_buf_len == input_bits_cnt) {
        log_info("Full frame!\n");
        print_buff(active_frame_buf, input_bits_cnt);

        rc = set_state(PROCESS_FRAME);
        if (STATE_SET_SUCCESS == rc) {
            /* Disable WS28 reader */
            active_exti->IMR &= ~EXTI_IMR_MR1; /* Mask IRQ */
        }
    }

    input_bits_cnt = 0;

    /* For time measurement */
    BLINK(EOF_GPIO_Port, EOF_Pin);
}

static inline
void parse_pixel(bool *pixel_buf, pixel_t pixel_p)
{
    for (int i = 7; i >= 0; i--) {
        pixel_p[RGB_RED] |= BUF_TO_BIT(SUBP_OFFSET(pixel_buf, WS28_RED), i);
        pixel_p[RGB_GREEN] |= BUF_TO_BIT(SUBP_OFFSET(pixel_buf, WS28_GREEN), i);
        pixel_p[RGB_BLUE] |= BUF_TO_BIT(SUBP_OFFSET(pixel_buf, WS28_BLUE), i);
    }
}

static
void parse_frame_buf(bool *frame_buf, pixel_t *pixel_buf, uint32_t pixel_cnt)
{
    uint16_t pixel_it = 0;

    if (NULL == frame_buf || NULL == pixel_buf) {
        log_err("Invalid args\n");

        return;
    }

    memset(pixel_buf, 0, (pixel_cnt * sizeof(pixel_t)));

    for (pixel_it = 0; pixel_it < pixel_cnt; pixel_it++) {
        parse_pixel(PIXEL_OFFSET(frame_buf, pixel_it), pixel_buf[pixel_it]);
    }
}

void ws28_read_frame(ws28_reader_data_st_t *ws28_reader_data,
    pixel_t *input_pixel_buf, uint32_t input_pixel_cnt)
{
    set_state(READ_FRAME);

    set_reader_callback_data(ws28_reader_data);

    /* Start read */
    active_exti->IMR |= EXTI_IMR_MR1; /* Enable IRQ */

    /* Wait till read finishes */
    while (STATE_NOT_MATCH == check_state(PROCESS_FRAME)) {
    }

    log_info("Hey dude, I'm done with reading!\n");

    /* TODO: check args */

    parse_frame_buf(ws28_reader_data->frame_buf, input_pixel_buf,
                    input_pixel_cnt);

    for (int i = 0; i < input_pixel_cnt; i++) {
        log_dbg("Parsed RGB[%d]:[%02X %02X %02X]\n", i,
                input_pixel_buf[i][RGB_RED],
                input_pixel_buf[i][RGB_GREEN],
                input_pixel_buf[i][RGB_BLUE]);
    }
}

void ws28_set_pixel(ws28_data_st_t *ws28_data, uint8_t red, uint8_t green,
    uint8_t blue, uint16_t pixel_pos)
{
    volatile uint16_t i;

    for(i = 0; i < 8; i++) {
        if (CHECK_BIT(green, (7 - i)) == 1) {
            ws28_data->pixel_buf[WS_BIT_POS(pixel_pos, WS28_GREEN, i)] = HIGH;
        } else {
            ws28_data->pixel_buf[WS_BIT_POS(pixel_pos, WS28_GREEN, i)] = LOW;
        }

        if (CHECK_BIT(red, (7 - i)) == 1) {
            ws28_data->pixel_buf[WS_BIT_POS(pixel_pos, WS28_RED, i)] = HIGH;
        } else {
            ws28_data->pixel_buf[WS_BIT_POS(pixel_pos, WS28_RED, i)] = LOW;
        }

        if (CHECK_BIT(blue, (7 - i)) == 1) {
            ws28_data->pixel_buf[WS_BIT_POS(pixel_pos, WS28_BLUE, i)] = HIGH;
        } else {
            ws28_data->pixel_buf[WS_BIT_POS(pixel_pos, WS28_BLUE, i)] = LOW;
        }
    }
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    log_warn("This callback should not be called!\n");
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == active_ws28_channel->timer_ptr) {
        active_ws28_channel->finished_writing = true;
        HAL_TIM_PWM_Stop_DMA(active_ws28_channel->timer_ptr,
                             active_ws28_channel->timer_channel);
    }
}

void ws28_write_pixels(ws28_data_st_t *ws28_data, uint8_t rgb_pixel_data[][3],
    uint16_t pixel_cnt)
{
    int i = 0;

    /* Wait till dma finishes */
    while (true != ws28_data->finished_writing) {
        log_info("Not finished yet\n");
    }

    ws28_data->finished_writing = false;

    if (rgb_pixel_data == NULL ||
        NULL == ws28_data ||
        pixel_cnt > ws28_data->pixel_in_strip) {
        log_err("Invalid args\n");

        return;
    }

    for (i = 0; i < pixel_cnt; i++) {
        ws28_set_pixel(ws28_data,
                       rgb_pixel_data[i][RGB_RED],
                       rgb_pixel_data[i][RGB_GREEN],
                       rgb_pixel_data[i][RGB_BLUE],
                       i);

        log_dbg("Pixel: [%03d] WS28 GRB: [%02X %02X %02X]\n", i,
                rgb_pixel_data[i][RGB_GREEN],
                rgb_pixel_data[i][RGB_RED],
                rgb_pixel_data[i][RGB_BLUE]);
    }

    active_ws28_channel = ws28_data;

    HAL_TIM_PWM_Start_DMA(ws28_data->timer_ptr,
                          ws28_data->timer_channel,
                          (uint32_t *)ws28_data->pixel_buf,
                          ws28_data->pixel_buf_len);

    /* Disabling Half interrupt. Must be right after DMA starting func */
    DMA1_Channel5->CCR &= ~DMA_CCR_HTIE; /* HT interrupt disabled */
}

