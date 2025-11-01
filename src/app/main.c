#include "core/app.h"
#include "log/log.h"
#include <stdint.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define NANOS_PER_SECOND 1000000000;

void render(app_t *app)
{
    log_debug("Render");

    BeginDrawing();
    ClearBackground((Color){25, 25, 25, 255});
    
    if (GuiButton((Rectangle){70, 410, 360, 40}, "Quit Application")) {
        log_info("Quit button pressed");
        app_stop(app);
    }
    
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 30, LIGHTGRAY);
    EndDrawing();
}

uint64_t get_time(void)
{
    return GetTime() * NANOS_PER_SECOND;
}

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    #ifdef DEBUG
        SetTraceLogLevel(LOG_DEBUG);
    #else
        SetTraceLogLevel(LOG_INFO);
    #endif
    
    log_info("Starting GUI application with raylib + raygui");
    
    app_t *app = app_create();
    if (!app) {
        log_error("Failed to initialize application");
        return 1;
    }
    
    InitWindow(app_get_window_width(app), app_get_window_height(app), app_get_name(app));
    SetTargetFPS(50);
    
    log_info("GUI initialized successfully with raylib + raygui");
    
    app_main_loop(app, get_time, render);
    
    CloseWindow();
    
    return 0;
}