/**
 * @file       log.h
 * @brief      Logging API
 *
 * @created    Jan 2, 2021
 * @author     Oleksandr Kholiavko <HalavicH@gmail.com>
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <stdarg.h>
#include <string.h>

#include "stm32f1xx_hal.h"

#define DEFAULT_LOG_TAG "ws-writer"

#ifndef LOG_TAG
#define LOG_TAG DEFAULT_LOG_TAG
#endif

#define FILE_NAME_NO_PATH(filepath)                                             \
    strrchr((filepath), '/') ?                                                  \
    strrchr((filepath), '/') + 1 : (filepath)

typedef enum verbose_level {
    NO_DEBUG = 0,
    LOG_ERR  = 1,
    LOG_WARN = 2,
    LOG_INFO = 3,
    LOG_DBG  = 4,
} verbose_level_t;

/* Prototypes */
void system_log(verbose_level_t verbosity, const char *fmt, ...);
void init_uart_logger(UART_HandleTypeDef *huart);

#define log(log_level, msg, fmt, ...)                                           \
    do {                                                                        \
        system_log(log_level, "|%s|[%s:%d:%s] %s" fmt, LOG_TAG,                 \
                   FILE_NAME_NO_PATH(__FILE__), __LINE__,                       \
                   __func__, msg, ## __VA_ARGS__);                                \
    } while (0);

#define log_err(fmt, ...)           log(LOG_ERR, "Err:", fmt,                   \
                                        ## __VA_ARGS__)
#define log_warn(fmt, ...)          log(LOG_WARN, "Warn:", fmt,                 \
                                        ## __VA_ARGS__)
#define log_info(fmt, ...)          log(LOG_INFO, "Info:", fmt,                 \
                                        ## __VA_ARGS__)
#define log_dbg(fmt, ...)           log(LOG_DBG, "Dbg:", fmt,                   \
                                        ## __VA_ARGS__)

#endif /* INC_LOG_H_ */

