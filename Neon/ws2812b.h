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

#endif /* INC_WS2812B_H_ */

