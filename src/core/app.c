#include "c64.h"
#include "core/app.h"
#include "log/log.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct app{
    c64_system_t *c64;
    const char *name;
    const char *version;
    bool running;
    bool stepping;
};

app_t* app_create() {
    app_t *app = (app_t*)malloc(sizeof(app_t));
    if (!app) return NULL;
    
    app->name = APP_NAME;
    app->version = APP_VERSION;
    app->running = false;
    app->stepping = false;
    app->c64 = c64_create("Failed to create C64 system");
    if(!app->c64){
        log_error("");
        app_shutdown(app);
        return NULL;
    }
    
    return app;
}

void app_shutdown(app_t *app) {
    if(!app) return;
    c64_destroy(app->c64);
    free(app);
}

bool app_is_running(app_t *app) {
    return app->running;
}

void app_start(app_t *app) {
    if (app) {
        app->running = true;
    }
}

void app_stop(app_t *app) {
    if (app) {
        app->running = false;
    }
}

char *app_get_name(app_t *app)
{
    if(app) return app->name;
    return NULL;
}

char *app_get_version(app_t *app)
{
    if(app) return app->version;
    return NULL;
}

int mainnotamain(int argc, char *argv[])
{

    // Crea sistema
    c64_system_t *g_c64 = c64_create();
    bool keep_running = true;
    if (!g_c64)
    {
        fprintf(stderr, "Failed to create C64 system\n");
        return 1;
    }

    // Reset
    printf("Resetting C64...\n");
    c64_reset(g_c64);

    // Carica programma se specificato
    if (argc > 1)
    {
        uint16_t addr = 0x0801; // Default BASIC start
        if (argc > 2)
        {
            addr = (uint16_t)strtol(argv[2], NULL, 16);
        }

        printf("Loading %s at $%04X\n", argv[1], addr);
        if (!c64_load_program(g_c64, argv[1], addr))
        {
            fprintf(stderr, "Failed to load program\n");
            c64_destroy(g_c64);
            return 1;
        }
    }

    // Loop principale
    printf("Running... (Press Ctrl+C to stop)\n");
    printf("Cycles: %lu\r", c64_get_cycles(g_c64));
    fflush(stdout);

    while (keep_running && c64_step(g_c64))
    {
        uint64_t cycles = c64_get_cycles(g_c64);

        // Report ogni 100k cicli
        if (cycles % 100000 == 0)
        {
            printf("Cycles: %lu\r", cycles);
            fflush(stdout);
        }
    }

    printf("Execution stopped at %lu cycles\n", c64_get_cycles(g_c64));

    // Dump finale
    printf("Dumping final state...\n");
    c64_dump_state(g_c64, "final_dump.txt");
    printf("State saved to final_dump.txt\n");

    // Cleanup
    c64_destroy(g_c64);

    return 0;
}