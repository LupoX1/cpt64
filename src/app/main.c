#include "c64.h"
#include "core/app.h"
#include "log/log.h"
#include <stdint.h>
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void render(app_t *app)
{
    log_debug("Render");

    BeginDrawing();
    ClearBackground((Color){25, 25, 25, 255});
    
    if (GuiButton((Rectangle){70, 410, 360, 40}, "Quit Application")) {
        log_info("Quit button pressed");
        EndDrawing();
        CloseWindow();
        return;
    }
    
    DrawText(TextFormat("FPS: %d", GetFPS()), 10, WINDOW_HEIGHT - 30, 20, LIGHTGRAY);
    EndDrawing();
}

void update(app_t *app, uint64_t t, uint64_t dt)
{
    log_debug("Update %lu %lu", t, dt);
}

void main_loop(app_t *app)
{
    uint64_t nanos_per_second = 1000000000;
    uint64_t max_frame_time = nanos_per_second / 4;

    uint64_t t = 0;
    uint64_t dt = C64_CLOCK_PAL;

    uint64_t currentTime = GetTime() * nanos_per_second;
    uint64_t accumulator = 0;

    while (!WindowShouldClose())
    {
        uint64_t newTime = GetTime() * nanos_per_second;
        uint64_t frameTime = newTime - currentTime;
        if ( frameTime > max_frame_time ) frameTime = max_frame_time;
        currentTime = newTime;

        accumulator += frameTime;

        while ( accumulator >= dt )
        {
            update(app, t, dt);
            t += dt;
            accumulator -= dt;
        }

        render(app);
    }

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
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, app_get_name(app));
    SetTargetFPS(50);
    
    log_info("GUI initialized successfully with raylib + raygui");
    
    app_start(app);
    main_loop(app);
    
    log_info("Shutting down GUI application");
    app_shutdown(app);
    
    return 0;
}