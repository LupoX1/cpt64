#ifndef VIDEO_H
#define VIDEO_H

typedef struct video_context video_context_t;

video_context_t* video_init(int width, int height, const char *title);
void video_shutdown(video_context_t *ctx);
void video_clear(video_context_t *ctx);
void video_present(video_context_t *ctx);
int video_should_close(video_context_t *ctx);

#endif
