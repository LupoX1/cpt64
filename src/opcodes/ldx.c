#include "ldx.h"

void ldx(cpu_6510_t *cpu, uint8_t value)
{
    write_xr(cpu, value);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, (value & 0x80) != 0);
}

void fA2(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    ldx(cpu, ram[address]);
}

void fA6(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    ldx(cpu, ram[address]);
}

void fAE(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    ldx(cpu, ram[address]);
}

void fB6(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_y(cpu, ram);
    ldx(cpu, ram[address]);
}

void fBE(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_y(cpu, ram);
    ldx(cpu, ram[address]);
}