/*
typedef struct
{
  memory_t ram;
  uint8_t basic[0x2000];
  uint8_t kernal[0x2000];
  uint8_t chars[0x1000];
  cpu_6510_t *cpu;
} C64_Core;
 */

#include <stdio.h>
#include <stdlib.h>
#include "core/bus.h"

struct c64_bus
{
    cpu_t *cpu;
    memory_t *mem;
};

c64_bus_t *bus_create()
{
    c64_bus_t *bus = malloc(sizeof(c64_bus_t));
    if (!bus) return NULL;
    bus->cpu = cpu_create();
    if(!bus->cpu)
    {
        bus_destroy(bus);
        return NULL;
    }

    bus->mem = memory_create();
    if(!bus->mem)
    {
        bus_destroy(bus);
        return NULL;
    }

    return bus;
}

bool bus_load_roms(c64_bus_t *bus)
{
    if (!bus)
        return false;
    return memory_load_roms(bus->mem);
}

bool bus_load_binary(c64_bus_t *bus, const char *program_file, uint16_t address)
{
    if (!bus)
        return false;
    return memory_load_binary(bus->mem, program_file, address);
}

/*

void c64_core_reset(C64_Core *core)
{
  c64_core_load_roms(core);

  uint8_t pcl = read(core->ram, 0xFFFC);
  uint8_t pch = read(core->ram, 0xFFFD);
  uint16_t pc = pch << 8 | pcl;
  write_program_counter(core->cpu, pc);
}
*/

void bus_reset(c64_bus_t *bus)
{
    if (!bus)
        return;
    printf("bus_reset\n");
}

bool bus_clock(c64_bus_t *bus)
{
    if (!bus)
        return false;
    printf("bus_clock\n");
    return false;
}

cpu_t *bus_get_cpu(c64_bus_t *bus)
{
    if (!bus)
        return false;
    return bus->cpu;
}

memory_t *bus_get_ram(c64_bus_t *bus)
{
    if (!bus)
        return false;
    return bus->mem;
}

void bus_destroy(c64_bus_t *bus)
{
    if (!bus)
        return;
    cpu_destroy(bus->cpu);
    memory_destroy(bus->mem);
    free(bus);
}