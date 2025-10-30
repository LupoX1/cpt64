#include "core/app.h"
#include <stdlib.h>
#include <string.h>

app_t* app_init(const char *name, const char *version) {
    app_t *app = (app_t*)malloc(sizeof(app_t));
    if (!app) return NULL;
    
    app->name = name;
    app->version = version;
    app->running = 1;
    
    return app;
}

void app_shutdown(app_t *app) {
    if (app) {
        app->running = 0;
        free(app);
    }
}

int app_is_running(app_t *app) {
    return app ? app->running : 0;
}

void app_stop(app_t *app) {
    if (app) {
        app->running = 0;
    }
}
