#include "stx.h"

void f86(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ram[address] = read_xr(cpu);
}

void f8E(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ram[address] = read_xr(cpu);
}

void f96(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_y(cpu, ram);
    ram[address] = read_xr(cpu);
}