#include "tay.h"

void fA8(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t acc = read_accumulator(cpu);
    write_yr(cpu, acc);
    set_zero_flag(cpu, acc == 0);
    set_negative_flag(cpu, (acc & 0x80) != 0);
}