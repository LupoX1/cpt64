#ifndef SBC_H
#define SBC_H

#include "cpu.h"

void fE1(cpu_6510_t *cpu, memory_t ram);
void fE5(cpu_6510_t *cpu, memory_t ram);
void fE9(cpu_6510_t *cpu, memory_t ram);
void fED(cpu_6510_t *cpu, memory_t ram);
void fF1(cpu_6510_t *cpu, memory_t ram);
void fF5(cpu_6510_t *cpu, memory_t ram);
void fF9(cpu_6510_t *cpu, memory_t ram);
void fFD(cpu_6510_t *cpu, memory_t ram);

#endif // SBC_H
