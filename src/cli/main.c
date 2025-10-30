#include "core/app.h"
#include "log/log.h"
#include "video/video.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    log_init(NULL);
    
    log_info("Starting CLI application");
    
    app_t *app = app_init("cpt64 CLI", "0.1.0");
    if (!app) {
        log_error("Failed to initialize application");
        return 1;
    }
    
    log_info("Application: %s v%s", app->name, app->version);
    
    int counter = 0;
    while (app_is_running(app) && counter < 5) {
        log_debug("Loop iteration %d", counter);
        printf("CLI running... iteration %d\n", counter);
        sleep(1);
        counter++;
    }
    
    log_info("Shutting down CLI application");
    app_shutdown(app);
    log_shutdown();
    
    return 0;
}
