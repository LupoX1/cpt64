#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "core/system.h"
#include "core/bus.h"
#include "cpu/cpu.h"

struct c64_system
{
    c64_bus_t *bus;
};

c64_system_t *c64_create()
{
    printf("c64_create\n");
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

bool c64_load_program(c64_system_t *sys, char *program_file, uint16_t address)
{
    if(!sys) return false;
    printf("c64_load_program\n");
    return bus_load_binary(sys->bus, program_file, address);
}

void c64_reset(c64_system_t *sys)
{
    if(!sys) return;
    printf("c64_reset\n");
    bus_load_roms(sys->bus);
    bus_reset(sys->bus);
}

bool c64_step(c64_system_t *sys)
{
    if(!sys) return false;
    printf("c64_step\n");
    return bus_clock(sys->bus);
}

uint64_t c64_get_cycles(c64_system_t *sys)
{
    if(!sys) return 0;
    printf("c64_get_cycles\n");
    cpu_state_t state;
    cpu_t *cpu = bus_get_cpu(sys->bus);
    cpu_get_state(cpu, &state);
    return state.cycles;
}

void c64_dump_state(c64_system_t *sys, const char* filename)
{
    printf("c64_dump_state to %s\n", filename);
    if(!sys || !filename) return;

    FILE *file = fopen(filename, "w+");
    if(!file) return;

    cpu_t *cpu = bus_get_cpu(sys->bus);
    cpu_dump(cpu, file);
    fprintf(file, "\n");
    memory_t *ram = bus_get_ram(sys->bus);
    memory_dump(ram, file);
    
    fclose(file);
}

void c64_destroy(c64_system_t *sys)
{
    printf("c64_destroy\n");
    if(!sys) return;
    bus_destroy(sys->bus);
    free(sys);
}

/*
void c64_run_cycles(c64_system_t *sys, uint64_t cycles)
{
    uint64_t start = c64_get_cycles(sys);
    uint64_t target = start + cycles;
    
    while (c64_get_cycles(sys) < target) {
        if (!c64_step(sys)) break;
    }
}
*/