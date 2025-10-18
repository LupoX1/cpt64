#include "sta.h"

void f81(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_x(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f85(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f89(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f8D(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f91(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_y(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f95(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f99(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_y(cpu, ram);
    ram[address] = read_accumulator(cpu);
}

void f9D(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    ram[address] = read_accumulator(cpu);
}