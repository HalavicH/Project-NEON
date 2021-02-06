/*
 * ws2812b.c
 *
 *  Created on: Nov 15, 2020
 *      Author: halavich
 */

#include <stdlib.h>

#include "log.h"
#include "ws2812b.h"

#define DELAY_LEN   100
#define HIGH        65
#define LOW         26

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

