#include "asl.h"

uint8_t asl(cpu_6510_t *cpu, uint8_t value)
{
    set_carry_flag(cpu, (value & 0x80) != 0);
    uint8_t result = value << 1;
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & 0x80) != 0);
    return result;
}

void f06(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ram[address] = asl(cpu, ram[address]);
}

void f0A(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t result = (cpu, read_accumulator(cpu));
    write_accumulator(cpu, result);
}

void f0E(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ram[address] = asl(cpu, ram[address]);
}

void f16(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    ram[address] = asl(cpu, ram[address]);
}

void f1E(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    ram[address] = asl(cpu, ram[address]);
}