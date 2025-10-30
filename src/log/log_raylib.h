#ifndef LOG_RAYLIB_H
#define LOG_RAYLIB_H

#include <stdarg.h>

void log_callback_raylib(log_level_t level, const char *fmt, va_list args);

#endif // LOG_RAYLIB_H