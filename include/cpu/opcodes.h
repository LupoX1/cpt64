#ifndef OPCODES_H
#define OPCODES_H

#include "cpu/cpu.h"

bool adc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool and(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool asl(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bad(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bcc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bsc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool beq(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bit(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bmi(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bne(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bpl(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool brk(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bvc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool bvs(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool clc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool cld(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool cli(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool clv(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool cmp(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool cpx(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool cpy(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool dec(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool dex(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool dey(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool eor(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool inc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool inx(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool iny(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool jmp(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool jsr(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool lda(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool ldx(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool ldy(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool lsr(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool nop(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool ora(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool pha(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool php(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool pla(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool plp(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool rol(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool ror(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool rti(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool rts(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool sbc(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool sec(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool sed(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool sei(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool sta(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool stx(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool sty(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool tax(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool tay(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool tsx(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool txa(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool txs(cpu_6510_t *cpu, memory_t ram, uint16_t address);
bool tya(cpu_6510_t *cpu, memory_t ram, uint16_t address);

#endif // OPCODES_H