#include "log_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const char* level_strings[] = {
    "DEBUG", "INFO", "WARN", "ERROR"
};

static const char* level_colors[] = {
    "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m"
};

static void stdout_output(log_level_t level, const char *message) {
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    
    fprintf(stdout, "%s[%02d:%02d:%02d %s]\x1b[0m %s\n",
            level_colors[level],
            t->tm_hour, t->tm_min, t->tm_sec,
            level_strings[level],
            message);
    
    fflush(stdout);
}

static void stdout_destroy() {
}

static const log_ops_t stdout_ops = {
    .output = stdout_output,
    .destroy = stdout_destroy
};

void log_init(log_level_t level) {
    log_context_t *ctx = malloc(sizeof(log_context_t));
    if (!ctx) return;
    
    ctx->min_level = level;
    ctx->ops = &stdout_ops;
    
    g_context = ctx;
}