#ifndef STA_H
#define STA_H

#include "cpu.h"

void f81(cpu_6510_t *cpu, memory_t ram);
void f85(cpu_6510_t *cpu, memory_t ram);
void f89(cpu_6510_t *cpu, memory_t ram);
void f8D(cpu_6510_t *cpu, memory_t ram);
void f91(cpu_6510_t *cpu, memory_t ram);
void f95(cpu_6510_t *cpu, memory_t ram);
void f99(cpu_6510_t *cpu, memory_t ram);
void f9D(cpu_6510_t *cpu, memory_t ram);

#endif // STA_H