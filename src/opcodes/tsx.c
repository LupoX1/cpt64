#include "tsx.h"

void fBA(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t sp = read_sp(cpu);
    write_xr(cpu, sp);
    set_zero_flag(cpu, sp == 0);
    set_negative_flag(cpu, (sp & 0x80) != 0);
    increment_cycles(cpu, 2);
    increment_program_counter(cpu, 1);
}