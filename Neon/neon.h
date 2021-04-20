#ifndef NEON_H
#define NEON_H

/* config */
#define MULTIPLIER                      4.875
#define INPUT_PIXEL_CNT                 32
#define OUTPUT_PIXEL_CNT                (int)(INPUT_PIXEL_CNT * MULTIPLIER)
#define ATTENUATION_INDEX               2.5
#define GAMMA_CORRECTION_INDEX          2.2

#define WS28_FRAME_SIZE                 816

/* End of config */

#define BLINK(port, pin)                                                        \
    HAL_GPIO_WritePin(port, pin, 1);                                            \
    HAL_GPIO_WritePin(port, pin, 0);

void neon_main();

#endif /* NEON_H */

