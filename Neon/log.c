/**
 * @file       log.c
 *
 * @brief      Contains UART logger implementation
 * @created    Jan 2, 2021
 * @author     Oleksandr Kholiavko <HalavicH@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

static UART_HandleTypeDef *log_dev_ptr;

void init_uart_logger(UART_HandleTypeDef *huart)
{
    log_dev_ptr = huart;
}

int retarget_put_char(int ch)
{
    HAL_UART_Transmit(log_dev_ptr, (uint8_t *)&ch, 1, 1);

    return 0;
}

int _write(int fd, char *ptr, int len)
{
    (void)fd;
    int i = 0;

    while (ptr[i] && (i < len)) {
        retarget_put_char((int)ptr[i]);
        if (ptr[i] == '\n') {
            retarget_put_char((int)'\r');
        }

        i++;
    }

    return len;
}

void system_log(verbose_level_t verbosity, const char *fmt, ...)
{
#if defined(LOG_LEVEL)
    va_list args;

    va_start(args, fmt);

    if (LOG_LEVEL >= verbosity) {
        vprintf(fmt, args);
    }
    va_end(args);
#endif
}

