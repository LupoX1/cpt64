#include "eor.h"

void eor(cpu_6510_t *cpu, uint8_t value)
{
    uint8_t result = read_accumulator(cpu) ^ value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & 0x80) != 0);
}

void f41(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_y(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 6);
    increment_program_counter(cpu, 2);
}

void f45(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 3);
    increment_program_counter(cpu, 2);
}

void f49(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_immediate(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 2);
    increment_program_counter(cpu, 2);
}

void f4D(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 3);
}

void f51(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_indirect_y(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 5);
    increment_program_counter(cpu, 2);
}

void f55(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_zeropage_x(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 2);
}

void f59(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_y(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 3);
}

void f5D(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t address = decode_address_absolute_x(cpu, ram);
    eor(cpu, ram[address]);
    increment_cycles(cpu, 4);
    increment_program_counter(cpu, 3);
}
