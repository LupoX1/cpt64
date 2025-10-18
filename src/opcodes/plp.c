#include "plp.h"

void f28(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t status = pop(cpu, ram);
    write_sr(cpu, status);
}