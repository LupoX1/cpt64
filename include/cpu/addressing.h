#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>

uint16_t decode_address_immediate(struct cpu *cpu, struct c64_bus *bus);  // #$nn
uint16_t decode_address_absolute(struct cpu *cpu, struct c64_bus *bus);   // $nnnn
uint16_t decode_address_absolute_x(struct cpu *cpu, struct c64_bus *bus); // $nnnn,X
uint16_t decode_address_absolute_y(struct cpu *cpu, struct c64_bus *bus); // $nnnn,Y
uint16_t decode_address_zeropage(struct cpu *cpu, struct c64_bus *bus);   // $nn
uint16_t decode_address_zeropage_x(struct cpu *cpu, struct c64_bus *bus); // $nn,X
uint16_t decode_address_zeropage_y(struct cpu *cpu, struct c64_bus *bus); // $nn,Y
uint16_t decode_address_relative(struct cpu *cpu, struct c64_bus *bus);   // label
uint16_t decode_address_indirect(struct cpu *cpu, struct c64_bus *bus);   // ($nnnn)
uint16_t decode_address_indirect_x(struct cpu *cpu, struct c64_bus *bus); // ($nn,X)
uint16_t decode_address_indirect_y(struct cpu *cpu, struct c64_bus *bus); // ($nn),Y

typedef uint16_t (*addr_mode_t)(struct cpu *cpu, struct c64_bus *bus);

typedef enum
{
    N_D,
    ACC,
    IMP,
    IMM,
    ABS,
    ABX,
    ABY,
    ZPG,
    ZPX,
    ZPY,
    REL,
    IND,
    INX,
    INY
} addr_mode_idx_t;

#endif // ADDRESSING_H