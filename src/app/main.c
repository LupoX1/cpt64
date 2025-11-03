#include "core/emu.h"
#include "log/log.h"
#include <stdint.h>
#include <raylib.h>
#include <time.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Struttura per lo stato UI (da dichiarare fuori dal render)
typedef struct
{
    int toolbar_height;
    int sidebar_width;
    int c64_screen_width;
    int c64_screen_height;
    int status_height;
    bool show_memory_viewer;
    bool show_register_viewer;
    bool show_disassembly;
    int memory_scroll;
    int memory_address;
    char rom_path[256];
    char program_path[256];
    char state_path[256];
    Texture2D *tex;
} ui_state_t;

void render_toolbar(ui_state_t *ui, emu_t *emu)
{
    bool is_running = emu_is_running(emu);
    bool is_paused = emu_is_paused(emu);

    // Bottoni controllo emulazione
    int btn_x = ui->sidebar_width + 5;
    int btn_y = 0;
    int btn_w = 80;
    int btn_h = 30;
    int btn_gap = 0;

    GuiSetState(!is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(ICON_PLAYER_PLAY, "Start")))
        emu_start(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(is_paused ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE, is_paused ? "Resume" : "Pause")))
        is_paused ? emu_resume(emu) : emu_pause(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(ICON_PLAYER_STOP, "Stop")))
        emu_stop(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(is_running & is_paused ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(ICON_PLAYER_NEXT, "Step")))
        emu_step(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(STATE_NORMAL);
}

void render_sidebar(ui_state_t *ui, emu_t *emu)
{
    // ========================================
    // SIDEBAR SINISTRA - Caricamento files
    // ========================================
    int sidebar_x = 0;
    int sidebar_y = 0;
    GuiPanel((Rectangle){sidebar_x, sidebar_y, ui->sidebar_width, GetScreenHeight()}, "Files");

    int sy = sidebar_y + 40;
    int sx = 10;
    int sw = ui->sidebar_width - 20;

    // ROM Files
    GuiGroupBox((Rectangle){sx, sy, sw, 105}, "ROM Files");
    sy += 10;

    GuiLabel((Rectangle){sx + 10, sy, 100, 20}, "BASIC ROM:");
    if (GuiButton((Rectangle){sx + 110, sy, sw - 120, 25}, "Load BASIC"))
    {
        // TODO: File dialog
        emu_load_rom(emu, "basic", "roms/basic.rom");
    }
    sy += 30;

    GuiLabel((Rectangle){sx + 10, sy, 100, 20}, "KERNAL ROM:");
    if (GuiButton((Rectangle){sx + 110, sy, sw - 120, 25}, "Load KERNAL"))
    {
        emu_load_rom(emu, "kernal", "roms/kernal.rom");
    }
    sy += 30;

    GuiLabel((Rectangle){sx + 10, sy, 100, 20}, "CHARSET ROM:");
    if (GuiButton((Rectangle){sx + 110, sy, sw - 120, 25}, "Load CHARSET"))
    {
        emu_load_rom(emu, "charset", "roms/charset.rom");
    }
    sy = 165;

    // Program
    GuiGroupBox((Rectangle){sx, sy, sw, 50}, "Program");
    sy += 10;

    if (GuiButton((Rectangle){sx + 10, sy, sw - 20, 30}, "#5#Load Program (.PRG)"))
    {
        // TODO: File dialog
        emu_load_program(emu, "program.prg", 0x0801);
    }
    sy = 235;

    // State
    GuiGroupBox((Rectangle){sx, sy, sw, 60}, "Save State");
    sy += 10;

    if (GuiButton((Rectangle){sx + 10, sy, (sw - 30) / 2, 30}, "#2#Save State"))
    {
        // TODO: File dialog
        emu_save_state(emu, "state.sav");
    }
    if (GuiButton((Rectangle){sx + 15 + (sw - 30) / 2, sy, (sw - 30) / 2, 30}, "#5#Load State"))
    {
        emu_load_state(emu, "state.sav");
    }
    sy += 30;

    GuiLabel((Rectangle){sx + 10, sy, sw - 20, 20}, "Quick save: F5 | Quick load: F9");
}

void render_status(ui_state_t *ui, emu_t *emu)
{
    int screen_x = ui->sidebar_width + 20;
    int screen_y = ui->toolbar_height + 20;

    // ========================================
    // PANNELLO STATUS - Sotto lo schermo
    // ========================================
    if (!emu_is_paused(emu))
        return;

    int status_y = screen_y + ui->c64_screen_height + 50;
    GuiPanel((Rectangle){screen_x - 5, status_y, ui->c64_screen_width + 10, ui->status_height}, "Debugger");

    int info_y = status_y + 30;
    int col1_x = screen_x + 10;
    int col2_x = screen_x + 250;
    int col3_x = screen_x + 450;

    // Registri CPU
    GuiLabel((Rectangle){col1_x, info_y, 200, 20}, TextFormat("PC: $%04X", emu_get_pc(emu)));
    GuiLabel((Rectangle){col2_x, info_y, 200, 20}, TextFormat("A: $%02X", emu_get_a(emu)));
    GuiLabel((Rectangle){col3_x, info_y, 200, 20}, TextFormat("X: $%02X", emu_get_x(emu)));
    info_y += 25;

    GuiLabel((Rectangle){col1_x, info_y, 200, 20}, TextFormat("Y: $%02X", emu_get_y(emu)));
    GuiLabel((Rectangle){col2_x, info_y, 200, 20}, TextFormat("SP: $%02X", emu_get_sp(emu)));
    GuiLabel((Rectangle){col3_x, info_y, 200, 20}, TextFormat("SR: $%02X", emu_get_status(emu)));
    info_y += 25;

    // Flags
    uint8_t status = emu_get_status(emu);
    char flags[9];
    flags[0] = (status & 0x80) ? 'N' : 'n';
    flags[1] = (status & 0x40) ? 'V' : 'v';
    flags[2] = '-';
    flags[3] = (status & 0x10) ? 'B' : 'b';
    flags[4] = (status & 0x08) ? 'D' : 'd';
    flags[5] = (status & 0x04) ? 'I' : 'i';
    flags[6] = (status & 0x02) ? 'Z' : 'z';
    flags[7] = (status & 0x01) ? 'C' : 'c';
    flags[8] = '\0';
    GuiLabel((Rectangle){col1_x, info_y, 300, 20}, TextFormat("Flags: %s", flags));
    info_y += 30;

    // Istruzione corrente
    GuiLabel((Rectangle){col1_x, info_y, 600, 20}, TextFormat("Current: %s", emu_get_current_instruction(emu)));
}

void render_side_panels(ui_state_t *ui, emu_t *emu)
{
    int screen_x = ui->sidebar_width + 20;
    int screen_y = ui->toolbar_height + 20;

    // ========================================
    // PANNELLI LATERALI OPZIONALI
    // ========================================
    int side_x = screen_x + ui->c64_screen_width + 30;
    int side_w = GetScreenWidth() - side_x - 10;

    if (side_w > 200)
    {
        int panel_y = ui->toolbar_height + 20;

        // Memory Viewer
        if (ui->show_memory_viewer)
        {
            GuiPanel((Rectangle){side_x, panel_y, side_w, 250}, "Memory Viewer");

            int mem_y = panel_y + 30;
            GuiLabel((Rectangle){side_x + 10, mem_y, 100, 20}, "Address:");
            GuiTextBox((Rectangle){side_x + 80, mem_y, 100, 25}, TextFormat("%04X", ui->memory_address), 5, false);

            mem_y += 35;

            // Mostra 16 righe di memoria (16 byte per riga)
            for (int row = 0; row < 8; row++)
            {
                uint16_t addr = ui->memory_address + (row * 16);
                char line[80];
                sprintf(line, "%04X: ", addr);

                for (int col = 0; col < 16; col++)
                {
                    sprintf(line + strlen(line), "%02X ", emu_read_memory(emu, addr + col));
                }

                GuiLabel((Rectangle){side_x + 10, mem_y, side_w - 20, 20}, line);
                mem_y += 22;
            }

            panel_y += 260;
        }

        // Register Viewer (dettagliato)
        if (ui->show_register_viewer && emu_is_paused(emu))
        {
            GuiPanel((Rectangle){side_x, panel_y, side_w, 200}, "CPU Registers");
            // Già mostrato nel pannello status principale
            panel_y += 210;
        }

        // Disassembly
        if (ui->show_disassembly && emu_is_paused(emu))
        {
            GuiPanel((Rectangle){side_x, panel_y, side_w, 300}, "Disassembly");

            int dis_y = panel_y + 30;
            uint16_t pc = emu_get_pc(emu);

            // Mostra 10 istruzioni intorno al PC
            for (int i = -3; i < 7; i++)
            {
                // TODO: Implementa disassembler per ottenere istruzioni
                // const char *instr = emu_disassemble(pc + i * 3);

                Color color = (i == 0) ? RED : LIGHTGRAY;
                DrawText(TextFormat("%04X: NOP", pc + i * 3), side_x + 10, dis_y, 16, color);
                dis_y += 22;
            }
        }
    }
}

void render_footer(ui_state_t *ui, emu_t *emu)
{
    // ========================================
    // FOOTER - Info generali
    // ========================================
    const char *status_text = emu_is_running(emu) ? (emu_is_paused(emu) ? "PAUSED" : "RUNNING") : "STOPPED";
    DrawText(TextFormat("Status: %s | FPS: %d", status_text, GetFPS()), 10, GetScreenHeight() - 20, 16, LIGHTGRAY);
}

void app_main_loop(emu_t *emu)
{
    InitWindow(emu->window_width, emu->window_height, emu->name);
    SetTargetFPS(50);
    log_info("GUI initialized successfully with raylib + raygui");

    const uint64_t nanos_per_second = 1000000000;
    const uint64_t max_frame_time = nanos_per_second / 4;

    uint64_t t = 0;
    uint64_t dt = emu->clock;

    uint64_t currentTime = GetTime() * nanos_per_second;
    uint64_t accumulator = 0;

    Texture2D tex = LoadTextureFromImage(GenImageColor(320, 200, BLACK));

    ui_state_t ui_state =
        {
            .show_memory_viewer = false,
            .show_register_viewer = false,
            .show_disassembly = false,
            .memory_scroll = 0,
            .memory_address = 0x744B,
            .rom_path = "",
            .program_path = "",
            .state_path = "",
            .toolbar_height = 50,
            .sidebar_width = 240,
            .c64_screen_width = 640,
            .c64_screen_height = 400,
            .status_height = 40};

    while (!WindowShouldClose())
    {
        uint64_t newTime = GetTime() * nanos_per_second;
        uint64_t frameTime = newTime - currentTime;
        if (frameTime > max_frame_time)
            frameTime = max_frame_time;
        currentTime = newTime;

        accumulator += frameTime;

        while (accumulator >= dt)
        {
            emu_update(emu, t, dt);
            t += dt;
            accumulator -= dt;
        }

        BeginDrawing();
//        render_sidebar(&ui_state, emu);
        render_toolbar(&ui_state, emu);
/*
        // ========================================
        // SCHERMO C64 - Centro
        // ========================================
        int screen_x = ui_state.sidebar_width + 20;
        int screen_y = ui_state.toolbar_height + 20;

        GuiPanel((Rectangle){screen_x - 5, screen_y - 5, ui_state.c64_screen_width + 10, ui_state.c64_screen_height + 35}, "C64 Display");

        // Bordo schermo
        DrawRectangle(screen_x - 2, screen_y + 25 - 2, ui_state.c64_screen_width + 4, ui_state.c64_screen_height + 4, BLUE);
        DrawRectangle(screen_x, screen_y + 25, ui_state.c64_screen_width, ui_state.c64_screen_height, BLACK);

        // Texture dello schermo C64 emu_get_framebuffer
        // uint32_t *framebuffer = emu_get_framebuffer(emu);
        // UpdateTexture(tex, random32);
        DrawTexturePro(tex,
                       (Rectangle){0, 0, 320, 200},
                       (Rectangle){screen_x, screen_y + 25, ui_state.c64_screen_width, ui_state.c64_screen_height},
                       (Vector2){0, 0}, 0.0f, WHITE);

        // render_side_panels(ui, emu);
        render_footer(&ui_state, emu);*/
        EndDrawing();
    }

    UnloadTexture(tex);
    CloseWindow();
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

#ifdef DEBUG
    SetTraceLogLevel(LOG_DEBUG);
#else
    SetTraceLogLevel(LOG_INFO);
#endif

    log_info("Starting GUI application with raylib + raygui");

    emu_t *emu = emu_create();
    if (!emu)
    {
        log_error("Failed to initialize application");
        return 1;
    }

    emu_reset(emu);

    // Carica programma se specificato
    if (argc > 1)
    {
        uint16_t addr = 0x0801; // Default BASIC start
        if (argc > 2)
        {
            addr = (uint16_t)strtol(argv[2], NULL, 16);
        }

        log_info("Loading %s at $%04X\n", argv[1], addr);
        if (!emu_load_program(emu, argv[1], addr))
        {
            log_error("Failed to load program\n");
            emu_shutdown(emu);
            return 1;
        }
    }

    app_main_loop(emu);

    // Cleanup
    log_info("Shutting down GUI application");
    emu_shutdown(emu);

    return 0;
}