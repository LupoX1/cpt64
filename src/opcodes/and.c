#include "and.h"

void and(cpu_6510_t *cpu, uint8_t value)
{
    uint8_t result = read_accumulator(cpu) & value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & 0x80) != 0);
}

void f21(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_indirect_x(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 6);
}

void f25(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_zeropage(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 3);
}

void f29(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_immediate(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 2);
}

void f2D(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 4);
}

void f31(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_indirect_y(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 5);
}

void f35(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_zeropage_x(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 4);
}

void f39(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute_y(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 4);
}

void f3D(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute_x(cpu, ram);
    and(cpu, *address);
    increment_cycles(cpu, 4);
}
