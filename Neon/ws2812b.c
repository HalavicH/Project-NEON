/*
 * ws2812b.c
 *
 *  Created on: Nov 15, 2020
 *      Author: halavich
 */

#include <stdlib.h>

#include "log.h"
#include "ws2812b.h"

/*--------------------------------------------------------------------------- */
#define DELAY_LEN   100
#define HIGH        65
#define LOW         26

#define CHECK_BIT(reg, bit)  ((reg & (1 << bit)) != 0)

#define WS_BIT_POS(pixel_pos, subpixel_pos, bit)                                \
    (GET_BIT_POS(pixel_pos, subpixel_pos, bit) + DELAY_LEN)

/*--------------------------------------------------------------------------- */

static ws28_data_st_t *active_ws28_channel = NULL;

void ws28_init(ws28_data_st_t *ws28_data)
{
    uint16_t i;

    if (NULL == ws28_data) {
        log_err("ws28_data is NULL\n");

        return;
    }

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
        HAL_TIM_PWM_Stop_DMA(active_ws28_channel->timer_ptr,
                             active_ws28_channel->timer_channel);
    }
}

void ws28_write_pixels(ws28_data_st_t *ws28_data, uint8_t rgb_pixel_data[][3],
    uint16_t pixel_cnt)
{
    int i = 0;

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
}

