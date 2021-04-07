/*
 * auth blah-blah
 */

#include <stdint.h>

/* returns value from an array, with checking for index accuracy */
static inline
uint8_t get_colour_src(uint8_t *input_px_channel, int input_px_cnt, int index)
{
    if (0 == input_px_cnt) {
        return 0;
    }

    if (index < 0) {
        return input_px_channel[0];
    }

    if (index >= input_px_cnt) {
        return input_px_channel[input_px_cnt - 1];
    }

    return input_px_channel[index];
}

/* returns float casted to (int) interpolated value, according to float position
 * in source array */
uint8_t get_interped_colour(uint8_t *input_px_channel, int input_px_cnt, float
    sub_pos)
{
    int pos_before = (int)sub_pos; /* lower colour position in current iter */
    int pos_after = (pos_before + 1);
    float coefficient = sub_pos - pos_before;
    uint8_t colour_before = 0;
    uint8_t colour_after = 0;
    uint8_t interped_colour = 0;

    colour_before = get_colour_src(input_px_channel, input_px_cnt, pos_before);
    colour_after = get_colour_src(input_px_channel, input_px_cnt, pos_after);

    /* Calculating interpolated value itself */
    if (coefficient <= 0) {
        return colour_before;
    }

    if (coefficient >= 1) {
        return colour_after;
    }

    interped_colour = (colour_before + coefficient * (colour_after -
                                                      colour_before));

    return interped_colour;
}

void interpolate_rgb(uint8_t input_px[][3], int input_px_cnt,
    uint8_t out_px[][3], int out_px_cnt)
{
    float multiplier = ((float)out_px_cnt - 1) / (input_px_cnt - 1);

    uint8_t red_px[input_px_cnt];
    uint8_t green_px[input_px_cnt];
    uint8_t blue_px[input_px_cnt];

    for (int i = 0; i < input_px_cnt; ++i) {
        red_px[i] = input_px[i][0];
        green_px[i] = input_px[i][1];
        blue_px[i] = input_px[i][2];
    }

    for (int i = 0; i < out_px_cnt; ++i) {
        out_px[i][0] =
            get_interped_colour(red_px, input_px_cnt, i / multiplier);
        out_px[i][1] =
            get_interped_colour(green_px, input_px_cnt, i / multiplier);
        out_px[i][2] =
            get_interped_colour(blue_px, input_px_cnt, i / multiplier);
    }
}

