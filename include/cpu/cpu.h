#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "../memory/memory.h"

#define N_D NULL
#define ACC &decode_address_accumulator
#define IMP &decode_address_implied
#define IMM &decode_address_immediate
#define ABS &decode_address_absolute
#define ABX &decode_address_absolute_x
#define ABY &decode_address_absolute_y
#define ZPG &decode_address_zeropage
#define ZPX &decode_address_zeropage_x
#define ZPY &decode_address_zeropage_y
#define REL &decode_address_relative
#define IND &decode_address_indirect
#define INX &decode_address_indirect_x
#define INY &decode_address_indirect_y

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80


typedef struct {
    uint64_t cycles;
    uint16_t pc;
    uint8_t sp;
    uint8_t sr;
    uint8_t x;
    uint8_t y;
    uint8_t a;
} cpu_state_t;

typedef struct cpu cpu_t;

cpu_t *cpu_create();
void cpu_dump(cpu_t *cpu, FILE *file);
void cpu_get_state(cpu_t *cpu, cpu_state_t *state);
void cpu_set_state(cpu_t *cpu, cpu_state_t *state);
void cpu_destroy(cpu_t *cpu);

typedef bool (*execute_t)(cpu_t *cpu, memory_t *mem, uint16_t address);
typedef uint16_t (*address_mode_t)(cpu_t *cpu, memory_t *mem);

cpu_t *create_cpu();
void destroy_cpu(cpu_t *cpu);

void set_carry_flag(cpu_t *cpu, bool value);
void set_zero_flag(cpu_t *cpu, bool value);
void set_interrupt_flag(cpu_t *cpu, bool value);
void set_decimal_flag(cpu_t *cpu, bool value);
void set_break_flag(cpu_t *cpu, bool value);
void set_overflow_flag(cpu_t *cpu, bool value);
void set_negative_flag(cpu_t *cpu, bool value);

bool get_carry_flag(cpu_t *cpu);
bool get_zero_flag(cpu_t *cpu);
bool get_interrupt_flag(cpu_t *cpu);
bool get_decimal_flag(cpu_t *cpu);
bool get_break_flag(cpu_t *cpu);
bool get_overflow_flag(cpu_t *cpu);
bool get_negative_flag(cpu_t *cpu);

uint8_t read_accumulator(cpu_t *cpu);
void write_accumulator(cpu_t *cpu, uint8_t value);
uint8_t read_xr(cpu_t *cpu);
void write_xr(cpu_t *cpu, uint8_t value);
uint8_t read_yr(cpu_t *cpu);
void write_yr(cpu_t *cpu, uint8_t value);
uint8_t read_sp(cpu_t *cpu);
void write_sp(cpu_t *cpu, uint8_t value);
uint8_t read_sr(cpu_t *cpu);
void write_sr(cpu_t *cpu, uint8_t value);
uint16_t read_program_counter(cpu_t *cpu);
void write_program_counter(cpu_t *cpu, uint16_t value);
void increment_program_counter(cpu_t *cpu, uint16_t value);

void push(cpu_t *cpu, memory_t *mem, uint8_t value);
uint8_t pop(cpu_t *cpu, memory_t *mem);

uint64_t read_cycles(cpu_t *cpu);
void increment_cycles(cpu_t *cpu, uint8_t value);

uint8_t fetch_instruction(cpu_t *cpu, memory_t *mem);
execute_t decode_instruction(uint8_t opcode);

uint16_t decode_address_accumulator(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_implied(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_immediate(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_absolute(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_absolute_x(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_absolute_y(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_zeropage(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_zeropage_x(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_zeropage_y(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_relative(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_indirect(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_indirect_x(cpu_t *cpu, memory_t *mem);
uint16_t decode_address_indirect_y(cpu_t *cpu, memory_t *mem);

void dump_cpu(cpu_t *cpu, FILE *file);
void log_cpu(cpu_t *cpu, memory_t *mem);

bool cpu_step(cpu_t *cpu, memory_t *mem);

#endif // CPU_H