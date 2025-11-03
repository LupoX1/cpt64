#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "core/system.h"
#include "core/bus.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "video/vic.h"

struct c64_system
{
    c64_bus_t *bus;
    c64_framebuffer_t *framebuffer;
};

c64_system_t* c64_create()
{
    c64_system_t *sys = malloc(sizeof(c64_system_t));
    if(!sys) return NULL;
    
    sys->bus = bus_create();
    if(!sys->bus)
    {
        free(sys);
        return NULL;
    }

    return sys;
}

bool c64_load_program(c64_system_t *sys, const char *program_file, uint16_t address)
{
    if(!sys) return false;
    return bus_load_binary(sys->bus, program_file, address);
}

void c64_reset(c64_system_t *sys)
{
    if(!sys) return;
    if(!bus_load_roms(sys->bus)) return;
    bus_reset(sys->bus);
}

bool c64_step(c64_system_t *sys)
{
    if(!sys) return false;
    return bus_clock(sys->bus);
}

uint64_t c64_get_cycles(c64_system_t *sys)
{
    if(!sys) return 0;
    cpu_state_t state;
    cpu_t *cpu = bus_get_cpu(sys->bus);
    cpu_get_state(cpu, &state);
    return state.cycles;
}

void c64_dump_state(c64_system_t *sys, const char* filename)
{
    if(!sys || !filename) return;

    FILE *file = fopen(filename, "w+");
    if(!file) return;

    cpu_t *cpu = bus_get_cpu(sys->bus);
    cpu_dump(cpu, file);
    fprintf(file, "\n");
    memory_dump(sys->bus, file);
    
    fclose(file);
}

void c64_debug(c64_system_t* sys)
{
    bus_log(sys->bus);
}

c64_bus_t* c64_get_bus(c64_system_t* sys)
{
    if(!sys) return NULL;
    return sys->bus;
}

void c64_destroy(c64_system_t *sys)
{
    if(!sys) return;
    bus_destroy(sys->bus);
    free(sys);
}

uint32_t *c64_get_framebuffer(c64_system_t* sys)
{
    if(!sys) return NULL;
    return bus_get_framebuffer(sys->bus);
}