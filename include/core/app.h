#ifndef APP_H
#define APP_H

#include <stdbool.h>

typedef struct app app_t;

app_t* app_create();
void app_shutdown(app_t *app);
bool app_is_running(app_t *app);
void app_start(app_t *app);
void app_stop(app_t *app);

char *app_get_name(app_t *app);
char *app_get_version(app_t *app);

#endif
