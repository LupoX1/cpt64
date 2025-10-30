#ifndef APP_H
#define APP_H

typedef struct {
    const char *name;
    const char *version;
    int running;
} app_t;

app_t* app_init(const char *name, const char *version);
void app_shutdown(app_t *app);
int app_is_running(app_t *app);
void app_stop(app_t *app);

#endif
