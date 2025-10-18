#include "pha.h"

void f48(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t acc = read_accumulator(cpu);
    push(cpu, ram, acc);
}