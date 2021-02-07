#include "neon.h"
#include "tim.h"

void neon_main()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); /* Start alive LED */
}

