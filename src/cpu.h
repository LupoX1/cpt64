#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "memory.h"

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

typedef struct cpu_6510_t cpu_6510_t;

typedef void (*instruction_t)(cpu_6510_t *cpu, memory_t ram);

cpu_6510_t *create_cpu();
void destroy_cpu(cpu_6510_t *cpu);

uint8_t read_accumulator(cpu_6510_t *cpu);
void write_accumulator(cpu_6510_t *cpu, uint8_t value);
uint8_t read_xr(cpu_6510_t *cpu);
void write_xr(cpu_6510_t *cpu, uint8_t value);
uint8_t read_yr(cpu_6510_t *cpu);
void write_yr(cpu_6510_t *cpu, uint8_t value);

void set_carry_flag(cpu_6510_t *cpu, bool value);
void set_zero_flag(cpu_6510_t *cpu, bool value);
void set_interrupt_flag(cpu_6510_t *cpu, bool value);
void set_decimal_flag(cpu_6510_t *cpu, bool value);
void set_break_flag(cpu_6510_t *cpu, bool value);
void set_overflow_flag(cpu_6510_t *cpu, bool value);
void set_negative_flag(cpu_6510_t *cpu, bool value);

bool get_carry_flag(cpu_6510_t *cpu);
bool get_zero_flag(cpu_6510_t *cpu);
bool get_interrupt_flag(cpu_6510_t *cpu);
bool get_decimal_flag(cpu_6510_t *cpu);
bool get_break_flag(cpu_6510_t *cpu);
bool get_overflow_flag(cpu_6510_t *cpu);
bool get_negative_flag(cpu_6510_t *cpu);

uint16_t read_program_counter(cpu_6510_t *cpu);
void write_program_counter(cpu_6510_t *cpu, uint16_t value);

uint8_t fetch_instruction(cpu_6510_t *cpu, memory_t ram);
instruction_t decode_instruction(uint8_t opcode);

uint8_t *decode_address_implied(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_accumulator(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_immediate(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_absolute(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_absolute_x(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_absolute_y(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_zeropage(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_zeropage_x(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_zeropage_y(cpu_6510_t *cpu, memory_t ram);
int8_t  *decode_address_relative(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_indirect(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_indirect_x(cpu_6510_t *cpu, memory_t ram);
uint8_t *decode_address_indirect_y(cpu_6510_t *cpu, memory_t ram);

void dump(cpu_6510_t *cpu, FILE *file);

#endif // CPU_H
