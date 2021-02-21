#ifndef NEON_H
#define NEON_H

#define BLINK(port, pin)                                                        \
    HAL_GPIO_WritePin(port, pin, 1);                                            \
    HAL_GPIO_WritePin(port, pin, 0);

void neon_main();

#endif /* NEON_H */

