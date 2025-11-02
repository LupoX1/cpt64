#include "c64.h"
#include "core/emu.h"
#include "log/log.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

emu_t *emu_create()
{
    emu_t *emu = (emu_t *)malloc(sizeof(emu_t));
    if (!emu)
        return NULL;

    emu->name = APP_NAME;
    emu->version = APP_VERSION;
    emu->clock = C64_CLOCK_PAL;
    emu->window_width = WINDOW_WIDTH;
    emu->window_height = WINDOW_HEIGHT;
    emu->running = false;
    emu->stepping = false;
    emu->paused = false;
    emu->c64 = c64_create();
    if (!emu->c64)
    {
        log_error("Failed to create C64 system");
        emu_shutdown(emu);
        return NULL;
    }

    return emu;
}

void emu_shutdown(emu_t *emu)
{
    if (!emu)
        return;
    c64_destroy(emu->c64);
    free(emu);
}

void emu_update(emu_t *emu, uint64_t t, uint64_t dt)
{
    (void)emu;
    (void)t;
    (void)dt;
    //log_debug("Update %lu %lu", t, dt);
}

void emu_load_rom(emu_t *emu, const char *type, const char *path)
{
    (void)emu;
    (void)type;
    (void)path;
}

void emu_load_program(emu_t *emu, const char *path)
{
    (void)emu;
    (void)path;
}

void emu_start(emu_t *emu)
{
    emu->paused = false;
    emu->running = true;
}

void emu_pause(emu_t *emu)
{
    emu->paused = true;
}

void emu_resume(emu_t *emu)
{
    emu->paused = false;
}

void emu_stop(emu_t *emu)
{
    emu->paused = false;
    emu->running = false;
}

void emu_step(emu_t *emu)
{
    emu->stepping = true;
}

bool emu_is_running(emu_t *emu)
{
    return emu->running;
}

bool emu_is_paused(emu_t *emu)
{
    return emu->paused;
}

void emu_save_state(emu_t *emu, const char *path)
{
    (void)emu;
    (void)path;
}

void emu_load_state(emu_t *emu, const char *path)
{
    (void)emu;
    (void)path;
}

// Informazioni stato
uint16_t emu_get_pc(emu_t *emu)
{
    (void)emu;
    return 0x356D;
}

uint8_t emu_get_a(emu_t *emu)
{
    (void)emu;
    return 0x6D;
}

uint8_t emu_get_x(emu_t *emu)
{
    (void)emu;
    return 0x7D;
}

uint8_t emu_get_y(emu_t *emu)
{
    (void)emu;
    return 0x6E;
}

uint8_t emu_get_sp(emu_t *emu)
{
    (void)emu;
    return 0x7E;
}

uint8_t emu_get_status(emu_t *emu)
{
    (void)emu;
    return 0x14;
}

const char *emu_get_current_instruction(emu_t *emu)
{
    (void)emu;
    return "LDA $54651";
}

uint8_t emu_read_memory(emu_t *emu, uint16_t addr)
{
    (void)emu;
    (void)addr;
    return 0x87;
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