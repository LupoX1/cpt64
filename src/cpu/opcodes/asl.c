#include "asl.h"

uint8_t asl(Mos6510 *cpu, uint8_t *value)
{
    set_carry_flag(cpu, (*value & 0x80) != 0);
    *value = *value << 1;
    set_zero_flag(cpu, *value == 0);
    set_negative_flag(cpu, (*value & 0x80) != 0);
}

void f06(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_zeropage(cpu, ram);
    asl(cpu, address);
}

void f0A(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_accumulator(cpu, ram);
    asl(cpu, address);
}

void f0E(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_absolute(cpu, ram);
    asl(cpu, address);
}

void f16(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_zeropage_x(cpu, ram);
    asl(cpu, address);
}

void f1E(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_absolute_x(cpu, ram);
    asl(cpu, address);
}