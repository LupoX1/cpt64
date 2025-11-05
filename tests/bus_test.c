#include <stdio.h>
#include <stdlib.h>
#include "c64.h"

struct c64_bus
{
    cpu_t *cpu;
    memory_t *mem;
    vic_t *vic;
};

c64_bus_t *bus_create()
{
    c64_bus_t *bus = malloc(sizeof(c64_bus_t));
    if (!bus)
        return NULL;
    bus->cpu = cpu_create();
    if (!bus->cpu)
    {
        bus_destroy(bus);
        return NULL;
    }

    bus->mem = memory_create();
    if (!bus->mem)
    {
        bus_destroy(bus);
        return NULL;
    }

    bus->vic = vic_create();
    if (!bus->vic)
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
    vic_destroy(bus->vic);
    free(bus);
}

bool bus_load_roms(c64_bus_t *bus)
{
    // Test mode: non caricare ROM
    (void)bus;
    return true;
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
    // Test mode: non inizializzare $0000/$0001
    cpu_reset(bus->cpu, bus);
}

bool bus_clock(c64_bus_t *bus)
{
    if (!bus)
        return false;

    // Test mode: solo CPU, no VIC
    return cpu_step(bus->cpu, bus);
}

bool bus_badline(c64_bus_t *bus)
{
    // Test mode: no badlines
    (void)bus;
    return false;
}

cpu_t *bus_get_cpu(c64_bus_t *bus)
{
    if (!bus)
        return NULL;
    return bus->cpu;
}

memory_t *bus_get_ram(c64_bus_t *bus)
{
    if (!bus)
        return NULL;
    return bus->mem;
}

// === ACCESSO DIRETTO ALLA RAM - NO BANK SWITCHING ===
void bus_write(c64_bus_t *bus, uint16_t addr, uint8_t value)
{
    if (!bus)
        return;
    write_direct(bus->mem, addr, value);
}

uint8_t bus_read(c64_bus_t *bus, uint16_t addr)
{
    if (!bus)
        return 0xFF;
    return read_direct(bus->mem, addr);
}

void bus_log(c64_bus_t *bus)
{
    if (!bus)
        return;
    cpu_log(bus->cpu, bus);
}

void bus_trigger_cpu_irq(c64_bus_t *bus)
{
    if (!bus)
        return;
    cpu_trigger_irq(bus->cpu);
}

uint32_t *bus_get_framebuffer(c64_bus_t *bus)
{
    // Test mode: no framebuffer
    (void)bus;
    return NULL;
}

uint8_t bus_read_vic_internal(c64_bus_t *bus, uint8_t reg)
{
    // Test mode: no VIC
    (void)bus;
    (void)reg;
    return 0xFF;
}