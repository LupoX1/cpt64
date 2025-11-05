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

    log_info("Execution stopped at %lu cycles\n", c64_get_cycles(emu->c64));

    // Dump finale
    log_info("Dumping final state...\n");
    c64_dump_state(emu->c64, "final_dump.txt");
    log_info("State saved to final_dump.txt\n");

    c64_destroy(emu->c64);
    free(emu);
}

void emu_update(emu_t *emu, uint64_t t, uint64_t dt)
{
    (void)t;
    (void)dt;

    if (!emu_is_running(emu))
        return;

    if (!emu_is_paused(emu))
    {
        if (!c64_step(emu->c64))
            log_error("Error cpu step");
        return;
    }

    if (emu->stepping)
    {
        emu->stepping = false;
        if (!c64_step(emu->c64))
            log_error("Error cpu step");
        c64_debug(emu->c64);
        return;
    }
}

void emu_load_rom(emu_t *emu, const char *type, const char *path)
{
    (void)emu;
    (void)type;
    (void)path;
}

bool emu_load_program(emu_t *emu, const char *path, uint16_t addr)
{
    (void)emu;
    (void)path;
    (void)addr;

    return true;
}

void emu_start(emu_t *emu)
{
    log_info("Running... (Press Ctrl+C to stop)\n");
    log_info("Cycles: %lu\r", c64_get_cycles(emu->c64));

    emu->paused = false;
    emu->running = true;
}

void emu_pause(emu_t *emu)
{
    emu->paused = true;
    c64_debug(emu->c64);
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

void emu_reset(emu_t *emu)
{
    c64_reset(emu->c64);
}

// Informazioni stato
uint16_t emu_get_pc(emu_t *emu)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);
    return cpu_get_pc(cpu);
}

uint8_t emu_get_a(emu_t *emu)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);
    return cpu_get_a(cpu);
}

uint8_t emu_get_x(emu_t *emu)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);
    return cpu_get_x(cpu);
}

uint8_t emu_get_y(emu_t *emu)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);
    return cpu_get_y(cpu);
}

uint8_t emu_get_sp(emu_t *emu)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);
    return cpu_get_sp(cpu);
}

uint8_t emu_get_status(emu_t *emu)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);
    return cpu_get_sr(cpu);
}

void emu_get_interrupt(emu_t *emu, char *irq)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);

    if(cpu_reset_pending(cpu)) 
    { irq[0] = 'R'; irq[1] = 'S'; irq[2] = 'T'; }
    else  { irq[0] = '-'; irq[1] = '-'; irq[2] = '-'; }
    irq[3] = ' ';
    if(cpu_nmi_pending(cpu)) { irq[4] = 'N'; irq[5] = 'M'; irq[6] = 'I'; }
    else { irq[4] = '-'; irq[5] = '-'; irq[6] = '-'; }
    irq[7] = ' ';
    if(cpu_irq_pending(cpu)) { irq[8] = 'I'; irq[9] = 'R'; irq[10] = 'Q'; }
    else { irq[8] = '-'; irq[9] = '-'; irq[10] = '-'; }
    irq[11] = 0;
}

void emu_get_flags(emu_t *emu, char *flags)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    cpu_t *cpu = bus_get_cpu(bus);

    flags[0] = cpu_get_flag(cpu, FLAG_N) ? 'N' : 'n';
    flags[1] = cpu_get_flag(cpu, FLAG_V) ? 'V' : 'v';
    flags[2] = '-';
    flags[3] = cpu_get_flag(cpu, FLAG_B) ? 'B' : 'b';
    flags[4] = cpu_get_flag(cpu, FLAG_D) ? 'D' : 'd';
    flags[5] = cpu_get_flag(cpu, FLAG_I) ? 'I' : 'i';
    flags[6] = cpu_get_flag(cpu, FLAG_Z) ? 'Z' : 'z';
    flags[7] = cpu_get_flag(cpu, FLAG_C) ? 'C' : 'c';
    flags[8] = 0;
}

uint64_t emu_get_cycle(emu_t *emu)
{
    return c64_get_cycles(emu->c64);
}

const char *emu_get_current_instruction(emu_t *emu)
{
    (void)emu;
    return "LDA $54651";
}

uint8_t emu_read_memory(emu_t *emu, uint16_t addr)
{
    c64_bus_t *bus = c64_get_bus(emu->c64);
    return bus_read(bus, addr);
}

uint32_t *emu_get_framebuffer(emu_t *emu)
{
    return c64_get_framebuffer(emu->c64);
}

uint8_t emu_read_vic(emu_t *emu, uint8_t reg)
{
    return c64_read_vic(emu->c64, reg);
}