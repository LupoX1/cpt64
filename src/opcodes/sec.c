#include "sec.h"

void f38(cpu_6510_t *cpu, memory_t ram)
{
    set_carry_flag(cpu, true);
    increment_program_counter(cpu, 1);
    increment_cycles(cpu, 2);
}