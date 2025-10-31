#include "video/video.h"
#include "log/log.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    log_init(LOG_LEVEL_DEBUG);
    
    printf("Testing video module...\n");
    
    video_context_t *ctx = video_init(800, 600, "Test");
    assert(ctx != NULL);
    assert(video_should_close(ctx) == 0);
    
    video_shutdown(ctx);
    
    printf("Video tests passed!\n");
    log_shutdown();
    return 0;
}
