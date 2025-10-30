#include "core/app.h"
#include "log/log.h"
#include "log/log_raylib.h"
#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    log_init(log_callback_raylib);
    
    log_info("Starting GUI application with raylib + raygui");
    
    app_t *app = app_init("cpt64 GUI", "0.1.0");
    if (!app) {
        log_error("Failed to initialize application");
        return 1;
    }
    
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, app->name);
    SetTargetFPS(60);
    
    log_info("GUI initialized successfully with raylib + raygui");
    
    Color bg_color = (Color){25, 25, 25, 255};
    int counter = 0;
    
    float bg_r = (float)bg_color.r;
    float bg_g = (float)bg_color.g;
    float bg_b = (float)bg_color.b;
    
    while (!WindowShouldClose() && app_is_running(app)) {
        BeginDrawing();
        ClearBackground(bg_color);
        
        GuiPanel((Rectangle){50, 50, 400, 450}, "cpt64 Control Panel");
        
        GuiLabel((Rectangle){70, 80, 360, 30}, "Welcome to cpt64 GUI!");
        
        GuiLabel((Rectangle){70, 120, 360, 20}, 
                 TextFormat("Application: %s v%s", app->name, app->version));
        
        GuiLabel((Rectangle){70, 150, 360, 20}, 
                 TextFormat("Counter: %d", counter));
        
        if (GuiButton((Rectangle){70, 180, 170, 30}, "Increment")) {
            counter++;
            log_debug("Counter incremented to %d", counter);
        }
        if (GuiButton((Rectangle){250, 180, 170, 30}, "Reset")) {
            counter = 0;
            log_debug("Counter reset");
        }
        
        GuiLabel((Rectangle){70, 230, 360, 20}, "Background Color:");
        
        GuiLabel((Rectangle){70, 260, 50, 20}, "Red:");
        GuiSliderBar((Rectangle){130, 260, 290, 20}, NULL, NULL, &bg_r, 0, 255);
        
        GuiLabel((Rectangle){70, 290, 50, 20}, "Green:");
        GuiSliderBar((Rectangle){130, 290, 290, 20}, NULL, NULL, &bg_g, 0, 255);
        
        GuiLabel((Rectangle){70, 320, 50, 20}, "Blue:");
        GuiSliderBar((Rectangle){130, 320, 290, 20}, NULL, NULL, &bg_b, 0, 255);
        
        bg_color = (Color){(int)bg_r, (int)bg_g, (int)bg_b, 255};
        
        DrawRectangle(70, 350, 360, 40, bg_color);
        DrawRectangleLines(70, 350, 360, 40, WHITE);
        GuiLabel((Rectangle){70, 355, 360, 30}, 
                 TextFormat("RGB(%d, %d, %d)", (int)bg_r, (int)bg_g, (int)bg_b));
        
        if (GuiButton((Rectangle){70, 410, 360, 40}, "Quit Application")) {
            log_info("Quit button pressed");
            break;
        }
        
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, WINDOW_HEIGHT - 30, 20, LIGHTGRAY);
        
        EndDrawing();
    }
    
    log_info("Shutting down GUI application");
    CloseWindow();
    
    app_shutdown(app);
    log_shutdown();
    
    return 0;
}
