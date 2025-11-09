#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>

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

addr_mode_t get_addr_mode(addr_mode_idx_t idx);

#endif // ADDRESSING_H