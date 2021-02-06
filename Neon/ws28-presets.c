#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "ws2812b.h"

#include "ws28-presets.h"

/*
 * TODO:
 * Add speed regulator
 */

#define FRAME_DELAY 30

#define SET_PIXEL(pixel, red, green, blue)                                      \
    do {                                                                        \
        pixel[0] = red;                                                         \
        pixel[1] = green;                                                       \
        pixel[2] = blue;                                                        \
    } while (0);

uint8_t ws28_comet_condition = 1;

void ws28_comet(ws28_data_st_t *ws28_data, uint8_t *RGB)
{
    uint8_t(*pixels)[3];
    uint32_t i = 0;
    int8_t dir = 1;

    if (NULL == ws28_data || NULL == RGB) {
        return;
    }

    pixels = calloc(ws28_data->pixel_in_strip * 3, sizeof(uint8_t));

    do {
        memset((void *)pixels, 0, ws28_data->pixel_in_strip * 3 *
               sizeof(uint8_t));

        SET_PIXEL(pixels[i - dir * 2], RGB[0] / 5, RGB[1] / 5, RGB[2] / 5);
        SET_PIXEL(pixels[i - dir], RGB[0] / 5, RGB[1] / 5, RGB[2] / 5);
        SET_PIXEL(pixels[i], RGB[0], RGB[1], RGB[2]);

        ws28_write_pixels(ws28_data, pixels, ws28_data->pixel_in_strip);

        (dir > 0) ? i++ : i--;

        if (i >= ws28_data->pixel_in_strip - 1) {
            dir = -1;
        }

        if (i <= 0) {
            dir = 1;
        }

        HAL_Delay(FRAME_DELAY);
    } while (ws28_comet_condition);

    free(pixels);
}

