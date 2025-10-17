#include "sbc.h"

void sbc(cpu_6510_t *cpu, uint8_t value)
{
    bool decimal_mode = get_decimal_flag(cpu);
    bool carry = get_carry_flag(cpu);
    uint8_t acc = read_accumulator(cpu);
    if (decimal_mode)
    {
        uint8_t a = (acc & 0xF0) * 10 + (acc & 0x0F);
        uint8_t b = ((value & 0xF0) >> 4) * 10 + (value & 0x0F);
        uint8_t c = acc - value - carry ? 1 : 0;
        uint8_t result = c < 0 ? c + 100 : c;

        write_accumulator(cpu, result);
        set_zero_flag(cpu, result == 0);
        set_carry_flag(cpu, c < 0);
        set_negative_flag(cpu, 0);
    }
    else
    {
        uint16_t result = acc - value - carry ? 1 : 0;
        write_accumulator(cpu, result & 0x00FF);
        set_overflow_flag(cpu, (~(acc ^ value) & (acc ^ result) & 0x80) != 0);
        set_zero_flag(cpu, result == 0);
        set_carry_flag(cpu, result > 0x00FF);
        set_negative_flag(cpu, (result & 0x80) != 0);
    }
}

void fE1(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_x(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 6);
    increment_program_counter(cpu, 2);
}

void fE5(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 3);
    increment_program_counter(cpu, 2);
}

void fE9(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 2);
    increment_program_counter(cpu, 2);
}

void fED(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 3);
}

void fF1(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_y(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 5);
    increment_program_counter(cpu, 2);
}

void fF5(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 2);
}

void fF9(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_y(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 3);
}

void fFD(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    sbc(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 3);
}
