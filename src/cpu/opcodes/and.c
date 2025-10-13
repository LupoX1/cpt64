#include "and.h"

void and(Mos6510 *cpu, uint8_t value)
{
    cpu->a = cpu->a & value;
    set_zero_flag(cpu, cpu->a == 0);
    set_negative_flag(cpu, (cpu->a & 0x80) != 0);
}

void f21(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_indirect_x(cpu, ram);
    and(cpu, *address);
}

void f25(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_zeropage(cpu, ram);
    and(cpu, *address);
}

void f29(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_immediate(cpu, ram);
    and(cpu, *address);
}

void f2D(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_absolute(cpu, ram);
    and(cpu, *address);
}

void f31(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_indirect_y(cpu, ram);
    and(cpu, *address);
}

void f35(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_zeropage_x(cpu, ram);
    and(cpu, *address);
}

void f39(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_absolute_y(cpu, ram);
    and(cpu, *address);
}

void f3D(Mos6510 *cpu, Ram ram)
{
    uint8_t *address = decode_address_absolute_x(cpu, ram);
    and(cpu, *address);
}