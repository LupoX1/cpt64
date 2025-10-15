#ifndef CMP_H
#define CMP_H

#include "cpu.h"

void fC1(cpu_6510_t *cpu, memory_t ram);
void fC5(cpu_6510_t *cpu, memory_t ram);
void fC9(cpu_6510_t *cpu, memory_t ram);
void fCD(cpu_6510_t *cpu, memory_t ram);
void fD1(cpu_6510_t *cpu, memory_t ram);
void fD5(cpu_6510_t *cpu, memory_t ram);
void fD9(cpu_6510_t *cpu, memory_t ram);
void fDD(cpu_6510_t *cpu, memory_t ram);

#endif // CMP_H
