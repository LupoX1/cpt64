#include "log/log.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>

static int level_to_raylib(log_level_t level) {
    switch(level) {
        case LOG_LEVEL_DEBUG: return LOG_DEBUG;
        case LOG_LEVEL_INFO:  return LOG_INFO;
        case LOG_LEVEL_WARN:  return LOG_WARNING;
        case LOG_LEVEL_ERROR: return LOG_ERROR;
        default: return LOG_INFO;
    }
}

static void log_message(log_level_t level, const char *fmt, va_list args)
{    
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    TraceLog(level_to_raylib(level), buffer);
}

void log_debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message(LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message(LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void log_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message(LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_message(LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}