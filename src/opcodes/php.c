#include "php.h"

void f08(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t status = read_sr(cpu);
    push(cpu, ram, status);
    increment_cycles(cpu,3);
    increment_program_counter(cpu,1);
}