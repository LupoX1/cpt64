#include "log/log.h"
#include <stdio.h>
#include <stdlib.h>

static log_callback_t global_logger = NULL;

static const char* level_strings[] = {
    "DEBUG", "INFO", "WARN", "ERROR"
};

static const char* level_colors[] = {
    "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"
};

static void default_logger(log_level_t level, const char *fmt, va_list args) {
    fprintf(stdout, "%s[%s] ", level_colors[level], level_strings[level]);
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

void log_init(log_callback_t cb) {
    global_logger = cb ? cb : default_logger;
}

void log_shutdown(void) {
    global_logger = NULL;
}

void log_message(log_level_t level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (global_logger) global_logger(level, fmt, args);
    va_end(args);
}