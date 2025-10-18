#include "lda.h"

void lda(cpu_6510_t *cpu, uint8_t value)
{
    write_accumulator(cpu, value);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, (value & 0x80) != 0);
}

void fA1(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_x(cpu, ram);
    lda(cpu, ram[address]);
}

void fA5(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    lda(cpu, ram[address]);
}

void fA9(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    lda(cpu, ram[address]);
}

void fAD(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    lda(cpu, ram[address]);
}

void fB1(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_y(cpu, ram);
    lda(cpu, ram[address]);
}

void fB5(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    lda(cpu, ram[address]);
}

void fB9(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_y(cpu, ram);
    lda(cpu, ram[address]);
}

void fBD(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    lda(cpu, ram[address]);
}