#include "adc.h"

void adc(cpu_6510_t *cpu, uint8_t value)
{
    bool carry = get_carry_flag(cpu);
    uint8_t acc = read_accumulator(cpu);
    uint16_t result = acc + value + carry ? 1 : 0;
    write_accumulator(cpu, result & 0x00FF);
    set_overflow_flag(cpu, (~(acc ^ value) & (acc ^ result) & 0x80) != 0);
    set_zero_flag(cpu, result == 0);
    set_carry_flag(cpu, result > 0x00FF);
    set_negative_flag(cpu, (result & 0x80) != 0);
}

void f61(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_indirect_x(cpu, ram);
    adc(cpu, *address);
}

void f65(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_zeropage(cpu, ram);
    adc(cpu, *address);
}

void f69(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_immediate(cpu, ram);
    adc(cpu, *address);
}

void f6D(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute(cpu, ram);
    adc(cpu, *address);
}

void f71(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_indirect_y(cpu, ram);
    adc(cpu, *address);
}

void f75(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_zeropage_x(cpu, ram);
    adc(cpu, *address);

}

void f79(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute_y(cpu, ram);
    adc(cpu, *address);
}

void f7D(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t *address = decode_address_absolute_x(cpu, ram);
    adc(cpu, *address);
}
