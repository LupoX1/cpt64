#include "tsx.h"

void f8A(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t xr = read_xr(cpu);
    write_accumulator(cpu, xr);
    set_zero_flag(cpu, xr == 0);
    set_negative_flag(cpu, (xr & 0x80) != 0);
    increment_cycles(cpu, 2);
    increment_program_counter(cpu, 1);
}