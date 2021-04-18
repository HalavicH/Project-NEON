#ifndef RGBINTERPOLATOR_H
#define RGBINTERPOLATOR_H

#include <stdint.h>

typedef uint8_t pixel_t[3];

typedef struct interp_data {
    float attenuation_index;

    pixel_t *input_px;
    uint32_t input_px_cnt;

    pixel_t *out_px;
    uint32_t out_px_cnt;
} interp_data_st_t;

void interpolate_rgb(interp_data_st_t *interp_data);

#endif /* RGBINTERPOLATOR_H */

