#ifndef ROR_H
#define ROR_H

#include "cpu.h"

void f66(cpu_6510_t *cpu, memory_t ram);
void f6A(cpu_6510_t *cpu, memory_t ram);
void f6E(cpu_6510_t *cpu, memory_t ram);
void f76(cpu_6510_t *cpu, memory_t ram);
void f7E(cpu_6510_t *cpu, memory_t ram);

#endif // ROR_H