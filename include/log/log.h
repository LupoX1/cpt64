#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_t;

typedef void (*log_callback_t)(log_level_t level, const char *fmt, va_list args);

void log_init(log_callback_t cb);
void log_shutdown(void);

void log_message(log_level_t level, const char *fmt, ...);

#define log_debug(fmt, ...) log_message(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) log_message(LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) log_message(LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log_message(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)

#endif // LOG_H