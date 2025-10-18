#include "sty.h"

void f84(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ram[address] = read_yr(cpu);
}

void f8C(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ram[address] = read_yr(cpu);
}

void f94(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    ram[address] = read_yr(cpu);
}