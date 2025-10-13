#include "bit.h"

void bit(Mos6510 *cpu, uint8_t value)
{
    cpu->a = cpu->a & value;
    set_zero_flag(cpu, cpu->a == 0);
    set_negative_flag(cpu, (value & 0x80) != 0);
    set_overflow_flag(cpu, (value & 0x40) != 0);
}

void f24(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_zeropage(cpu, ram);
    bit(cpu, *address);
}

void f2C(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_absolute(cpu, ram);
    bit(cpu, *address);
}