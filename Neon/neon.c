#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tim.h"
#include "usart.h"

#include "neon.h"

#include "log.h"
#include "state-machine.h"
#include "ws28-presets.h"
#include "ws2812b.h"

ws28_data_st_t ws28_ch1 = {0};
ws28_reader_data_st_t ws28_reader = {0};

void neon_main()
{
    uint32_t input_pixel_cnt = INPUT_PIXEL_CNT;
    uint32_t output_pixel_cnt = OUTPUT_PIXEL_CNT;
    pixel_t *input_pixel_buf = NULL;
    pixel_t *output_pixel_buf = NULL;
    bool *frame_buf = NULL;

    init_uart_logger(&huart3);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); /* Start alive LED */

    ws28_ch1.timer_ptr = &htim2;
    ws28_ch1.timer_channel = TIM_CHANNEL_1;
    ws28_ch1.pixel_in_strip = output_pixel_cnt;

    ws28_init(&ws28_ch1);

    frame_buf = calloc(WS28_FRAME_SIZE, sizeof(bool));
    if (NULL == frame_buf) {
        log_err("Out of memory\n");

        return;
    }

    ws28_reader.cmsis_eof_timer_ptr = TIM3;
    ws28_reader.cmsis_exti_irq_ptr = EXTI;
    ws28_reader.cmsis_input_gpio_port = GPIOA;
    ws28_reader.frame_buf = frame_buf;
    ws28_reader.frame_buf_len = WS28_FRAME_SIZE;
    ws28_reader.gpio_pin_mask = GPIO_PIN_0;

    ws28_reader_init(&ws28_reader);

    input_pixel_buf = calloc(input_pixel_cnt, sizeof(pixel_t));
    if (NULL == input_pixel_buf) {
        log_err("Out of memory\n");

        return;
    }

    output_pixel_buf = calloc(output_pixel_cnt, sizeof(pixel_t));
    if (NULL == output_pixel_buf) {
        log_err("Out of memory\n");

        return;
    }

    /* Main loop */
    while (1) {
        ws28_read_frame(&ws28_reader, input_pixel_buf, input_pixel_cnt);
    }
}

