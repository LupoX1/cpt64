#ifndef ASL_H
#define ASL_H

#include "mos6510.h"

void f06(Mos6510 *cpu, Ram ram);
void f0A(Mos6510 *cpu, Ram ram);
void f0E(Mos6510 *cpu, Ram ram);
void f16(Mos6510 *cpu, Ram ram);
void f1E(Mos6510 *cpu, Ram ram);

#endif // ASL_H
