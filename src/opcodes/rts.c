#include "rts.h"

void f60(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t pc = pop(cpu, ram);
    increment_cycles(cpu, 6);
    write_program_counter(cpu, pc + 1);
}