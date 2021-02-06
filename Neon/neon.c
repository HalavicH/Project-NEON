#include <stdint.h>
#include <string.h>

#include "log.h"
#include "neon.h"
#include "tim.h"
#include "usart.h"
#include "ws28-presets.h"
#include "ws2812b.h"

ws28_data_st_t ws28_ch1 = {0};

void neon_main()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); /* Start alive LED */

    init_uart_logger(&huart3);

    ws28_ch1.timer_ptr = &htim2;
    ws28_ch1.timer_channel = TIM_CHANNEL_1;
    ws28_ch1.pixel_in_strip = 7;

    ws28_init(&ws28_ch1);

    uint8_t colors[3] = {0xFF, 0x0, 0xFF};

    ws28_chess(&ws28_ch1, colors);
}

