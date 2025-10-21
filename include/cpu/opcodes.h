#ifndef OPCODES_H
#define OPCODES_H

#include "cpu/cpu.h"
#include "memory/memory.h"

bool adc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool and(cpu_t *cpu, memory_t *mem, uint16_t address);
bool asl(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bad(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bcc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bsc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool beq(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bit(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bmi(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bne(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bpl(cpu_t *cpu, memory_t *mem, uint16_t address);
bool brk(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bvc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool bvs(cpu_t *cpu, memory_t *mem, uint16_t address);
bool clc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool cld(cpu_t *cpu, memory_t *mem, uint16_t address);
bool cli(cpu_t *cpu, memory_t *mem, uint16_t address);
bool clv(cpu_t *cpu, memory_t *mem, uint16_t address);
bool cmp(cpu_t *cpu, memory_t *mem, uint16_t address);
bool cpx(cpu_t *cpu, memory_t *mem, uint16_t address);
bool cpy(cpu_t *cpu, memory_t *mem, uint16_t address);
bool dec(cpu_t *cpu, memory_t *mem, uint16_t address);
bool dex(cpu_t *cpu, memory_t *mem, uint16_t address);
bool dey(cpu_t *cpu, memory_t *mem, uint16_t address);
bool eor(cpu_t *cpu, memory_t *mem, uint16_t address);
bool inc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool inx(cpu_t *cpu, memory_t *mem, uint16_t address);
bool iny(cpu_t *cpu, memory_t *mem, uint16_t address);
bool jmp(cpu_t *cpu, memory_t *mem, uint16_t address);
bool jsr(cpu_t *cpu, memory_t *mem, uint16_t address);
bool lda(cpu_t *cpu, memory_t *mem, uint16_t address);
bool ldx(cpu_t *cpu, memory_t *mem, uint16_t address);
bool ldy(cpu_t *cpu, memory_t *mem, uint16_t address);
bool lsr(cpu_t *cpu, memory_t *mem, uint16_t address);
bool nop(cpu_t *cpu, memory_t *mem, uint16_t address);
bool ora(cpu_t *cpu, memory_t *mem, uint16_t address);
bool pha(cpu_t *cpu, memory_t *mem, uint16_t address);
bool php(cpu_t *cpu, memory_t *mem, uint16_t address);
bool pla(cpu_t *cpu, memory_t *mem, uint16_t address);
bool plp(cpu_t *cpu, memory_t *mem, uint16_t address);
bool rol(cpu_t *cpu, memory_t *mem, uint16_t address);
bool ror(cpu_t *cpu, memory_t *mem, uint16_t address);
bool rti(cpu_t *cpu, memory_t *mem, uint16_t address);
bool rts(cpu_t *cpu, memory_t *mem, uint16_t address);
bool sbc(cpu_t *cpu, memory_t *mem, uint16_t address);
bool sec(cpu_t *cpu, memory_t *mem, uint16_t address);
bool sed(cpu_t *cpu, memory_t *mem, uint16_t address);
bool sei(cpu_t *cpu, memory_t *mem, uint16_t address);
bool sta(cpu_t *cpu, memory_t *mem, uint16_t address);
bool stx(cpu_t *cpu, memory_t *mem, uint16_t address);
bool sty(cpu_t *cpu, memory_t *mem, uint16_t address);
bool tax(cpu_t *cpu, memory_t *mem, uint16_t address);
bool tay(cpu_t *cpu, memory_t *mem, uint16_t address);
bool tsx(cpu_t *cpu, memory_t *mem, uint16_t address);
bool txa(cpu_t *cpu, memory_t *mem, uint16_t address);
bool txs(cpu_t *cpu, memory_t *mem, uint16_t address);
bool tya(cpu_t *cpu, memory_t *mem, uint16_t address);

#endif // OPCODES_H