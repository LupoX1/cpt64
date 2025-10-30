#include "video/video.h"
#include "log/log.h"
#include <stdlib.h>

struct video_context {
    int width;
    int height;
    const char *title;
    int should_close;
};

video_context_t* video_init(int width, int height, const char *title) {
    log_info("Initializing video subsystem: %dx%d '%s'", width, height, title);
    
    video_context_t *ctx = (video_context_t*)malloc(sizeof(video_context_t));
    if (!ctx) {
        log_error("Failed to allocate video context");
        return NULL;
    }
    
    ctx->width = width;
    ctx->height = height;
    ctx->title = title;
    ctx->should_close = 0;
    
    return ctx;
}

void video_shutdown(video_context_t *ctx) {
    if (ctx) {
        log_info("Shutting down video subsystem");
        free(ctx);
    }
}

void video_clear(video_context_t *ctx) {
    if (!ctx) return;
}

void video_present(video_context_t *ctx) {
    if (!ctx) return;
}

int video_should_close(video_context_t *ctx) {
    return ctx ? ctx->should_close : 1;
}
