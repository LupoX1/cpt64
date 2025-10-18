#include "ldy.h"

void ldy(cpu_6510_t *cpu, uint8_t value)
{
    write_yr(cpu, value);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, (value & 0x80) != 0);
}

void fA0(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    ldy(cpu, ram[address]);
}

void fA4(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ldy(cpu, ram[address]);
}

void fAC(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ldy(cpu, ram[address]);
}

void fB4(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    ldy(cpu, ram[address]);
}

void fBC(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    ldy(cpu, ram[address]);
}