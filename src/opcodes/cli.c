#include "cli.h"

void f58(cpu_6510_t *cpu, memory_t ram)
{
    set_carry_flag(cpu, false);
    
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);

    increment_cycles(cpu, 2);
}
