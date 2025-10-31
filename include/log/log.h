#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} log_level_t;

typedef struct log_context log_context_t;

extern log_context_t *g_context;

void log_init(log_level_t level);
void log_init_raylib(log_level_t level);
void log_shutdown(void);

void log_debug(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_warn(const char *fmt, ...);
void log_error(const char *fmt, ...);

#endif // LOG_H