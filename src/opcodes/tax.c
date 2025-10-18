#include "tax.h"

void fAA(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t acc = read_accumulator(cpu);
    write_xr(cpu, acc);
    set_zero_flag(cpu, acc == 0);
    set_negative_flag(cpu, (acc & 0x80) != 0);
}