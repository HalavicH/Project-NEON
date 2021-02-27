#ifndef NEON_H
#define NEON_H

/* config */
#define ACTIVE_PIXEL_CNT                7
#define WS28_FRAME_SIZE                 ACTIVE_PIXEL_CNT * 24

/* End of config */

#define BLINK(port, pin)                                                        \
    HAL_GPIO_WritePin(port, pin, 1);                                            \
    HAL_GPIO_WritePin(port, pin, 0);

void neon_main();

#endif /* NEON_H */

