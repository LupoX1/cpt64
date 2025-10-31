#include "core/app.h"
#include "log/log.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    log_init(LOG_LEVEL_DEBUG);
    
    printf("Testing core module...\n");
    
    app_t *app = app_init("Test", "1.0");
    assert(app != NULL);
    assert(app_is_running(app) == 1);
    
    app_stop(app);
    assert(app_is_running(app) == 0);
    
    app_shutdown(app);
    
    printf("Core tests passed!\n");
    log_shutdown();
    return 0;
}
