#include "c64.h"
#include "core/emu.h"
#include "log/log.h"
#include <stdint.h>
#include <raylib.h>
#include <time.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

void render(emu_t *emu, Texture2D *tex);

static c64_system_t *g_c64;

void signal_handler(int sig) {
    const char *name = "UNKNOWN";
    switch (sig) {
        case SIGSEGV: name = "Segmentation Fault"; break;
        case SIGBUS:  name = "Bus Error"; break;
        case SIGILL:  name = "Illegal Instruction"; break;
        case SIGABRT: name = "Abort"; break;
        case SIGINT:  name = "Ctrl+C Interrupt"; break;
        case SIGTERM: name = "Termination Signal"; break;
        case SIGQUIT: name = "Quit"; break;
    }

    fprintf(stderr, "\n[!] Signal caught: %s (%d)\n", name, sig);
    c64_dump_state(g_c64, "c64_error_dump.log");

    // opzionale: stampa backtrace
    void *trace[64];
    int n = backtrace(trace, 64);
    backtrace_symbols_fd(trace, n, fileno(stderr));

    // termina subito
    exit(1);
}

void install_signal_handlers(c64_system_t *c64) {
    g_c64 = c64;
    signal(SIGSEGV, signal_handler);
    signal(SIGBUS,  signal_handler);
    signal(SIGILL,  signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
}

void app_main_loop(emu_t *emu)
{
    InitWindow(emu->window_width, emu->window_height, emu->name);
    ClearWindowState(FLAG_VSYNC_HINT);
    SetTargetFPS(50);

    //Font font = GetFontDefault();
    //GenTextureMipmaps(&font.texture);
    //SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);

    log_info("GUI initialized successfully with raylib + raygui");

    Texture2D tex = LoadTextureFromImage(GenImageColor(320, 200, BLACK));

    const uint64_t nanos_per_second = 1000000000;
    const uint64_t max_frame_time = nanos_per_second / 4;
    const uint64_t dt = nanos_per_second / emu->clock;
    
    uint64_t t = 0;
    uint64_t accumulator = 0;
    uint64_t currentTime = GetTime() * nanos_per_second;

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

        render(emu, &tex);
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

    install_signal_handlers(emu->c64);

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