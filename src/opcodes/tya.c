#include "tya.h"

void f98(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t yr = read_yr(cpu);
    write_accumulator(cpu, yr);
    set_zero_flag(cpu, yr == 0);
    set_negative_flag(cpu, (yr & 0x80) != 0);
}