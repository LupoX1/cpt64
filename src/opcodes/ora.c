#include "ora.h"

void ora(cpu_6510_t *cpu, uint8_t value)
{
    uint8_t result = read_accumulator(cpu) | value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & 0x80) != 0);
}

void f01(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_x(cpu, ram);
    ora(cpu, ram[address]);
}

void f05(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ora(cpu, ram[address]);
}

void f09(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    ora(cpu, ram[address]);
}

void f0D(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ora(cpu, ram[address]);
}

void f11(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_y(cpu, ram);
    ora(cpu, ram[address]);
}

void f15(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    ora(cpu, ram[address]);
}

void f19(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_y(cpu, ram);
    ora(cpu, ram[address]);
}

void f1D(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    ora(cpu, ram[address]);
}