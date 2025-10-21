#ifndef ADDRESSING_H
#define ADDRESSING_H

#include <stdint.h>

uint16_t decode_address_accumulator(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_implied(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_immediate(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_absolute(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_absolute_x(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_absolute_y(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_zeropage(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_zeropage_x(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_zeropage_y(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_relative(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_indirect(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_indirect_x(struct cpu *cpu, struct c64_bus *bus);
uint16_t decode_address_indirect_y(struct cpu *cpu, struct c64_bus *bus);

typedef uint16_t (*addr_mode_t)(struct cpu *cpu, struct c64_bus *bus);

#endif // ADDRESSING_H