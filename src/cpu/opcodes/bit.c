#include "bit.h"

void bit(cpu_6510_t *cpu, uint8_t value)
{
    uint8_t result = read_accumulator(cpu) & value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & 0x80) != 0);
    set_overflow_flag(cpu, (result & 0x40) != 0);
}

void f24(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_zeropage(cpu, ram);
    bit(cpu, *address);
}

void f2C(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute(cpu, ram);
    bit(cpu, *address);
}
