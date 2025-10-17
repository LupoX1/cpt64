#include "nop.h"

void fEA(cpu_6510_t *cpu, memory_t ram)
{
    increment_cycles(cpu,2);
    increment_program_counter(cpu,1);
}