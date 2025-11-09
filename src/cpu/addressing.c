#include "core/bus.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "cpu/addressing.h"

uint16_t decode_address_immediate(cpu_t *cpu, c64_bus_t *bus)
{
    (void) bus;
    return cpu_get_pc(cpu) + 1;
}

uint16_t decode_address_absolute(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t low_addr = bus_read(bus, pc + 1);
    uint8_t high_addr = bus_read(bus, pc + 2);
    return high_addr << 8 | low_addr;
}

uint16_t decode_address_absolute_x(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t low_addr = bus_read(bus, pc + 1);
    uint8_t high_addr = bus_read(bus, pc + 2);
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu_get_x(cpu);
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu_inc_cycles(cpu, 1);
    return new_address;
}

uint16_t decode_address_absolute_y(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t low_addr = bus_read(bus, pc + 1);
    uint8_t high_addr = bus_read(bus, pc + 2);
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu_get_y(cpu);
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu_inc_cycles(cpu, 1);
    return new_address;
}

uint16_t decode_address_zeropage(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    return (uint8_t)bus_read(bus, pc + 1);
}

uint16_t decode_address_zeropage_x(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t x = cpu_get_x(cpu);

    return (uint8_t)(bus_read(bus, pc + 1) + x);
}

uint16_t decode_address_zeropage_y(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t y = cpu_get_y(cpu);

    return (uint8_t)(bus_read(bus, pc + 1) + y);
}

uint16_t decode_address_relative(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    int8_t offset = (int8_t)bus_read(bus, pc + 1);
    uint16_t address = pc + 2;
    uint16_t new_address = address + offset;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu_inc_cycles(cpu, 1);
    return new_address;
}

uint16_t decode_address_indirect(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t low_addr = bus_read(bus, pc + 1);
    uint8_t high_addr = bus_read(bus, pc + 2);
    uint16_t address = high_addr << 8 | low_addr;

    uint8_t target_low = bus_read(bus, address);
    uint8_t target_high = bus_read(bus, (address & 0xFF00) | ((address + 1) & 0x00FF));

    return (target_high << 8) | target_low;
}

uint16_t decode_address_indirect_x(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t x = cpu_get_x(cpu);

    uint8_t zp_base = bus_read(bus, pc + 1);
    uint8_t zp_addr = zp_base + x;
    uint8_t low_addr = bus_read(bus, zp_addr);
    uint8_t high_addr = bus_read(bus, (uint8_t)(zp_addr + 1));
    return high_addr << 8 | low_addr;
}

uint16_t decode_address_indirect_y(cpu_t *cpu, c64_bus_t *bus)
{
    uint16_t pc = cpu_get_pc(cpu);
    uint8_t y = cpu_get_y(cpu);

    uint16_t zp_addr = (uint16_t)bus_read(bus, pc + 1);
    uint8_t low_addr = bus_read(bus, zp_addr);
    uint8_t high_addr = bus_read(bus, (uint8_t)(zp_addr + 1));
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + y;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu_inc_cycles(cpu, 1);
    return new_address;
}

addr_mode_t modes[] =
    {
        NULL,
        NULL,
        NULL,
        decode_address_immediate,
        decode_address_absolute,
        decode_address_absolute_x,
        decode_address_absolute_y,
        decode_address_zeropage,
        decode_address_zeropage_x,
        decode_address_zeropage_y,
        decode_address_relative,
        decode_address_indirect,
        decode_address_indirect_x,
        decode_address_indirect_y};

addr_mode_t get_addr_mode(addr_mode_idx_t idx)
{
    return modes[idx];
}