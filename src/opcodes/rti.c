#include "rti.h"

void f40(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t status = pop(cpu, ram);
    uint8_t pc = pop(cpu, ram);

    set_negative_flag(cpu, (status & FLAG_N) != 0);
    set_zero_flag(cpu, (status & FLAG_Z) != 0);
    set_carry_flag(cpu, (status & FLAG_C) != 0);
    set_interrupt_flag(cpu, (status & FLAG_I) != 0);
    set_decimal_flag(cpu, (status & FLAG_D) != 0);
    set_overflow_flag(cpu, (status & FLAG_V) != 0);

    increment_cycles(cpu, 6);
    increment_program_counter(cpu, 1);
    //write_program_counter(cpu, pc);
}