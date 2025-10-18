#include "pla.h"

void f68(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t acc = pop(cpu, ram);
    write_accumulator(cpu, acc);
}