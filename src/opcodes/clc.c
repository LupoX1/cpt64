#include "clc.h"

void f18(cpu_6510_t *cpu, memory_t ram)
{
    set_carry_flag(cpu, false);
    increment_program_counter(cpu, 1);
    increment_cycles(cpu, 2);
}
