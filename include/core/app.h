#ifndef APP_H
#define APP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct app app_t;

typedef void (*render_t)(app_t *app);
typedef uint64_t (*get_time_t)(void);

app_t* app_create();
void app_shutdown(app_t *app);
bool app_is_running(app_t *app);
void app_start(app_t *app);
void app_stop(app_t *app);

char *app_get_name(app_t *app);
char *app_get_version(app_t *app);
uint64_t app_get_clock(app_t *app);
uint16_t app_get_window_width(app_t *app);
uint16_t app_get_window_height(app_t *app);

void app_main_loop(app_t *app, get_time_t get_time, render_t render);

#endif
