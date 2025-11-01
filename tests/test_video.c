#include "video/video.h"
#include "log/log.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    
    printf("Testing video module...\n");
    
    video_context_t *ctx = video_init(800, 600, "Test");
    assert(ctx != NULL);
    assert(video_should_close(ctx) == 0);
    
    video_shutdown(ctx);
    
    printf("Video tests passed!\n");
    return 0;
}
