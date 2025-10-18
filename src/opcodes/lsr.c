#include "lsr.h"

uint8_t lsr(cpu_6510_t *cpu, uint8_t value)
{
    uint8_t result = value >> 1;
    set_carry_flag(cpu, (value & 0x0001) != 0);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, false);
}

void f46(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    uint8_t result = lsr(cpu, ram[address]);
    ram[address] = result;
}

void f4A(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t value = read_accumulator(cpu);
    uint8_t result = lsr(cpu, value);
    write_accumulator(cpu, result);
}

void f4E(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    uint8_t result = lsr(cpu, ram[address]);
    ram[address] = result;
}

void f56(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    uint8_t result = lsr(cpu, ram[address]);
    ram[address] = result;
}

void f5E(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    uint8_t result = lsr(cpu, ram[address]);
    ram[address] = result;
}