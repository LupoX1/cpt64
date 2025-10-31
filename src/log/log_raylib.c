#include "log_internal.h"
#include <raylib.h>
#include <stdlib.h>

static int level_to_raylib(log_level_t level) {
    switch(level) {
        case LOG_LEVEL_DEBUG: return LOG_DEBUG;
        case LOG_LEVEL_INFO:  return LOG_INFO;
        case LOG_LEVEL_WARN:  return LOG_WARNING;
        case LOG_LEVEL_ERROR: return LOG_ERROR;
        default: return LOG_INFO;
    }
}

static void raylib_output(log_level_t level, const char *message) {
    TraceLog(level_to_raylib(level), message);
}

static void raylib_destroy() {
}

static const log_ops_t raylib_ops = {
    .output = raylib_output,
    .destroy = raylib_destroy
};

void log_init_raylib(log_level_t level) {
    log_context_t *ctx = malloc(sizeof(log_context_t));
    if (!ctx) return;
    
    SetTraceLogLevel(level);
    ctx->min_level = level;
    ctx->ops = &raylib_ops;

    g_context = ctx;
}