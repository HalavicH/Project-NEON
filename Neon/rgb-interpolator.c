/*
 * auth blah-blah
 */

#include "rgb-interpolator.h"

#define COPY_PIXEL(dest, src)           (dest)[0] = src[0];                     \
                                        (dest)[1] = src[1];                     \
                                        (dest)[2] = src[2]

#define SET_PIXEL(dest, r, g, b)        (dest)[0] = r;                          \
                                        (dest)[1] = g;                          \
                                        (dest)[2] = b

#define INTERP_PIXEL(intp_px, px_bef, px_aft, sub_pos)                          \
    intp_px[0] = sub_pos * (px_aft[0] - px_bef[0]) + px_bef[0];                 \
    intp_px[1] = sub_pos * (px_aft[1] - px_bef[1]) + px_bef[1];                 \
    intp_px[2] = sub_pos * (px_aft[2] - px_bef[2]) + px_bef[2]

/* returns value from an array, with checking for index accuracy */
static inline
void get_parent_px(pixel_t *input_px, int input_px_cnt, int index,
    pixel_t *parent_px)
{
    if (0 == input_px_cnt) {
        SET_PIXEL(*parent_px, 0, 0, 0);
    } else if (index < 0) {
        COPY_PIXEL(*parent_px, input_px[0]);
    } else if (index >= input_px_cnt) {
        COPY_PIXEL(*parent_px, input_px[input_px_cnt - 1]);
    } else {
        COPY_PIXEL(*parent_px, input_px[index]);
    }
}

void interpolate_rgb(interp_data_st_t *interp_data)
{
    int out_px_pos;
    float sub_pos;
    float sub_pos_fractional;
    int left_px_pos;
    int right_px_pos;
    float multiplier;
    pixel_t left_px;
    pixel_t right_px;

    multiplier = ((float)interp_data->out_px_cnt - 1) /
                 ((float)interp_data->input_px_cnt - 1);

    for (out_px_pos = 0; out_px_pos < interp_data->out_px_cnt; ++out_px_pos) {
        sub_pos = out_px_pos / multiplier;
        left_px_pos = (int)sub_pos; /* lower px position in current iter */
        right_px_pos = (left_px_pos + 1);
        sub_pos_fractional = sub_pos - left_px_pos;

        get_parent_px(interp_data->input_px, interp_data->input_px_cnt,
                      left_px_pos, &left_px);
        get_parent_px(interp_data->input_px, interp_data->input_px_cnt,
                      right_px_pos, &right_px);

        /* Calculating interpolated value itself */
        if (sub_pos_fractional <= 0) {
            COPY_PIXEL(interp_data->out_px[out_px_pos], left_px);
        } else if (sub_pos_fractional >= 1) {
            COPY_PIXEL(interp_data->out_px[out_px_pos], right_px);
        } else {
            INTERP_PIXEL(interp_data->out_px[out_px_pos], left_px, right_px,
                         sub_pos_fractional);
        }
    }
}

