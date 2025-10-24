#include <stdio.h>
#include <stdlib.h>
#include "c64.h"

struct c64_bus
{
    cpu_t *cpu;
    memory_t *mem;
    vic_t *vic;
};

c64_bus_t* bus_create()
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

void bus_destroy(c64_bus_t *bus)
{
    if (!bus)
        return;
    cpu_destroy(bus->cpu);
    memory_destroy(bus->mem);
    free(bus);
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

void bus_reset(c64_bus_t *bus)
{
    if (!bus)
        return;
    write_direct(bus->mem, 0x0000, 0x2F);
    write_direct(bus->mem, 0x0001, 0x37);
    cpu_reset(bus->cpu, bus);
}

bool bus_clock(c64_bus_t *bus)
{
    if (!bus)
        return false;
    return cpu_step(bus->cpu, bus);
}

cpu_t* bus_get_cpu(c64_bus_t *bus)
{
    if (!bus)
        return false;
    return bus->cpu;
}

memory_t* bus_get_ram(c64_bus_t *bus)
{
    if (!bus)
        return false;
    return bus->mem;
}

bool is_io(memory_t *mem, uint16_t address)
{
  return (read_direct(mem, 1) & CHAREN) && (address >= CHARGEN_ADDRESS) && ((uint32_t)address < (CHARGEN_ADDRESS + CHARGEN_SIZE));
}

bool is_basic_rom(memory_t *mem, uint16_t address)
{
  return (read_direct(mem, 1) & ( HIRAM | LORAM ) ) && (address >= BASIC_ADDRESS) && ((uint32_t)address < (BASIC_ADDRESS + BASIC_SIZE));
}

bool is_kernal_rom(memory_t *mem, uint16_t address)
{
  return (read_direct(mem, 1) & HIRAM) && (address >= KERNAL_ADDRESS) && ((uint32_t)address < (KERNAL_ADDRESS + KERNAL_SIZE));
}

bool is_char_rom(memory_t *mem, uint16_t address)
{
  return (read_direct(mem, 1) & ~CHAREN) && (address >= CHARGEN_ADDRESS) && ((uint32_t)address <= (CHARGEN_ADDRESS + CHARGEN_SIZE));
}

void bus_write(c64_bus_t *bus, uint16_t addr, uint8_t value)
{
    write_direct(bus->mem, addr, value);
}

uint8_t bus_read_io(c64_bus_t *bus, uint16_t addr)
{
    return 0xFF;
}

uint8_t bus_read(c64_bus_t *bus, uint16_t addr)
{
    if(is_basic_rom(bus->mem, addr)) return read_basic(bus->mem, addr);
    if(is_kernal_rom(bus->mem, addr)) return read_kernal(bus->mem, addr);
    if(is_char_rom(bus->mem, addr)) return read_chargen(bus->mem, addr);
    if(is_io(bus->mem, addr)) return bus_read_io(bus, addr);

    return read_direct(bus->mem, addr);
}

void bus_log(c64_bus_t* bus)
{
    cpu_log(bus->cpu, bus);
}