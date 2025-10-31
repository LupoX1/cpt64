#include "log_internal.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// Contesto globale
log_context_t *g_context = NULL;

void log_shutdown(void) {
    if (!g_context) return;
    
    free(g_context);
    g_context = NULL;
}

static void log_message(log_level_t level, const char *fmt, va_list args) {
    if (!g_context) return;
    if (level < g_context->min_level) return;
    
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    
    g_context->ops->output(level, buffer);
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