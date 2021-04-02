/**
 * @file       ws2812b.h
 * @brief      WS2812B Write API
 *
 * @author     Oleksandr Kholiavko <HalavicH@gmail.com>
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "tim.h"

#define BPSP                            8
#define BPP                             (3 * BPSP)

#define WS28_RED                        1
#define WS28_GREEN                      0
#define WS28_BLUE                       2

#define RGB_RED                         0
#define RGB_GREEN                       1
#define RGB_BLUE                        2

/* TODO: Check it I need it in header */
#define GET_BIT_POS(pixel_pos, subpixel_pos, bit)                               \
    (pixel_pos * BPP) + i + (subpixel_pos * BPSP)

/* TODO: Hack this somehow */
typedef uint32_t pixel_buf_t;

typedef struct ws28_data_st {
    TIM_HandleTypeDef *timer_ptr;
    uint32_t          timer_channel;
    uint16_t          pixel_in_strip;

    uint32_t          pixel_buf_len;
    pixel_buf_t       *pixel_buf;
} ws28_data_st_t;

/* API */
void ws28_init(ws28_data_st_t *ws28_data);
void ws28_deinit(ws28_data_st_t *ws28_data);

void ws28_set_pixel(ws28_data_st_t *ws28_data, uint8_t red, uint8_t green,
    uint8_t blue, uint16_t pixel_pos);
void ws28_write_pixels(ws28_data_st_t *ws28_data, uint8_t rgb_pixel_data[][3],
    uint16_t pixel_cnt);

#endif /* INC_WS2812B_H_ */

