#include <stdio.h>
#include <stdlib.h>
#include "core/bus.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "video/vic.h"

struct c64_bus
{
    cpu_t *cpu;
    memory_t *mem;
    vic_t *vic;
};

c64_bus_t* bus_create()
{
    struct c64_bus *bus = malloc(sizeof(struct c64_bus));
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

void bus_destroy(struct c64_bus *bus)
{
    if (!bus)
        return;
    cpu_destroy(bus->cpu);
    memory_destroy(bus->mem);
    free(bus);
}

bool bus_load_roms(struct c64_bus *bus)
{
    if (!bus)
        return false;
    return memory_load_roms(bus->mem);
}

bool bus_load_binary(struct c64_bus *bus, const char *program_file, uint16_t address)
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

void bus_reset(struct c64_bus *bus)
{
    if (!bus)
        return;
    cpu_reset(bus->cpu, bus);
}

bool bus_clock(struct c64_bus *bus)
{
    if (!bus)
        return false;
    return cpu_step(bus->cpu, bus);
}

cpu_t* bus_get_cpu(struct c64_bus *bus)
{
    if (!bus)
        return false;
    return bus->cpu;
}

memory_t* bus_get_ram(struct c64_bus *bus)
{
    if (!bus)
        return false;
    return bus->mem;
}

void bus_write(c64_bus_t *bus, uint16_t addr, uint8_t value)
{
    write_direct(bus->mem, addr, value);
}

uint8_t bus_read(c64_bus_t *bus, uint16_t addr)
{
    return read_direct(bus->mem, addr);
}