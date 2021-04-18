#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "tim.h"
#include "usart.h"

#include "neon.h"

#include "log.h"
#include "rgb-interpolator.h"
#include "state-machine.h"
#include "ws28-presets.h"
#include "ws2812b.h"

ws28_data_st_t ws28_ch1 = {0};
ws28_reader_data_st_t ws28_reader = {0};
bool *frame_buf = NULL;

void neon_main()
{
    uint32_t input_pixel_cnt = INPUT_PIXEL_CNT;
    uint32_t output_pixel_cnt = OUTPUT_PIXEL_CNT;
    pixel_t *input_pixel_buf = NULL;
    pixel_t *output_pixel_buf = NULL;
    interp_data_st_t interp_data = {0};

    init_uart_logger(&huart3);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); /* Start alive LED */

    /* Preset channel */
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

    log_dbg("Allocated input_pixel_buf of %d elements\n", input_pixel_cnt);

    output_pixel_buf = calloc(output_pixel_cnt, sizeof(pixel_t));
    if (NULL == output_pixel_buf) {
        log_err("Out of memory\n");

        return;
    }

    log_dbg("Allocated output_pixel_buf of %d elements\n", output_pixel_cnt);

    interp_data.attenuation_index = ATTENUATION_INDEX;

    interp_data.input_px = input_pixel_buf;
    interp_data.input_px_cnt = input_pixel_cnt;
    interp_data.out_px = output_pixel_buf;
    interp_data.out_px_cnt = output_pixel_cnt;

    /* Main loop */
    while (1) {
        log_dbg("Going to read the frame\n");

        ws28_read_frame(&ws28_reader, input_pixel_buf, input_pixel_cnt);

        /* TODO: Interpolate here */
        log_dbg("Interpolating %d pixels to %d\n", input_pixel_cnt,
                output_pixel_cnt);

        interpolate_rgb(&interp_data);

        for (int i = 0; i < output_pixel_cnt; i++) {
            log_dbg("Interpolated RGB[%d]:[%02X %02X %02X]\n", i,
                    output_pixel_buf[i][RGB_RED],
                    output_pixel_buf[i][RGB_GREEN],
                    output_pixel_buf[i][RGB_BLUE]);
        }

        set_state(WRITE_FRAME);

        /* Write here */
        log_info("Write interpolated pixels\n");
        ws28_write_pixels(&ws28_ch1, output_pixel_buf, output_pixel_cnt);
    }
}

