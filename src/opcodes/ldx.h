#ifndef LDX_H
#define LDX_H

#include "cpu.h"

void fA2(cpu_6510_t *cpu, memory_t ram);
void fA6(cpu_6510_t *cpu, memory_t ram);
void fAE(cpu_6510_t *cpu, memory_t ram);
void fB6(cpu_6510_t *cpu, memory_t ram);
void fBE(cpu_6510_t *cpu, memory_t ram);

#endif // LDX_H