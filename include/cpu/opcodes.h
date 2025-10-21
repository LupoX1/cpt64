#ifndef OPCODES_H
#define OPCODES_H

#include <stdbool.h>

typedef bool (*opcode_t)(struct cpu*, struct c64_bus*, uint16_t);

bool adc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool and(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool asl(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bad(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bcc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bsc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool beq(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bit(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bmi(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bne(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bpl(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool brk(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bvc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool bvs(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool clc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool cld(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool cli(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool clv(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool cmp(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool cpx(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool cpy(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool dec(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool dex(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool dey(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool eor(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool inc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool inx(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool iny(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool jmp(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool jsr(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool lda(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool ldx(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool ldy(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool lsr(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool nop(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool ora(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool pha(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool php(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool pla(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool plp(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool rol(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool ror(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool rti(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool rts(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool sbc(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool sec(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool sed(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool sei(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool sta(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool stx(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool sty(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool tax(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool tay(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool tsx(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool txa(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool txs(struct cpu *cpu, struct c64_bus *bus, uint16_t address);
bool tya(struct cpu *cpu, struct c64_bus *bus, uint16_t address);

#endif // OPCODES_H