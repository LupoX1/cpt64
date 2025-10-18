#include "txs.h"

void f9A(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t xr = read_xr(cpu);
    write_sp(cpu, xr);
}