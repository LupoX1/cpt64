#include <stddef.h>
#include <stdlib.h>

#include "cpu.h"
#include "opcodes/adc.h"
#include "opcodes/and.h"
#include "opcodes/asl.h"
#include "opcodes/bcc.h"
#include "opcodes/bcs.h"
#include "opcodes/beq.h"
#include "opcodes/bit.h"
#include "opcodes/bmi.h"
#include "opcodes/bne.h"
#include "opcodes/bpl.h"
#include "opcodes/brk.h"
#include "opcodes/bvc.h"
#include "opcodes/bvs.h"

struct cpu_6510_t
{
  uint64_t cycles;
  uint16_t pc;  
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint8_t sr;
  uint8_t sp;
  bool reset;
  bool nmi;
  bool irq;  
};

cpu_6510_t *create_cpu()
{
  cpu_6510_t *cpu = malloc(sizeof(cpu_6510_t));
  if(cpu) return cpu;
  return NULL;
}

void destroy_cpu(cpu_6510_t *cpu)
{
  if(cpu) free(cpu);
}

void bad(cpu_6510_t *cpu, memory_t ram)
{
}

instruction_t instruction_set[256] = {
//    0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F 
/*0*/ &f00, &bad, &bad, &bad, &bad, &bad, &f06, &bad, &bad, &bad, &f0A, &bad, &bad, &bad, &f0E, &bad,
/*1*/ &f10, &bad, &bad, &bad, &bad, &bad, &f16, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &f1E, &bad,
/*2*/ &bad, &f21, &bad, &bad, &f24, &f25, &bad, &bad, &bad, &f29, &bad, &bad, &f2C, &f2D, &bad, &bad,
/*3*/ &f30, &f31, &bad, &bad, &bad, &f35, &bad, &bad, &bad, &f39, &bad, &bad, &bad, &f3D, &bad, &bad,
/*4*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*5*/ &f50, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*6*/ &bad, &f61, &bad, &bad, &bad, &f65, &bad, &bad, &bad, &f69, &bad, &bad, &bad, &f6D, &bad, &bad,
/*7*/ &f70, &f71, &bad, &bad, &bad, &f75, &bad, &bad, &bad, &f79, &bad, &bad, &bad, &f7D, &bad, &bad,
/*8*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*9*/ &f90, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*A*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*B*/ &fB0, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*C*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*D*/ &fD0, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*E*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*F*/ &fF0, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad};

void set_flag(cpu_6510_t *cpu, uint8_t flag, bool value)
{
    if (value)
        cpu->sr = cpu->sr | flag;
    else
        cpu->sr = cpu->sr & ~flag;
}

bool get_flag(cpu_6510_t *cpu, uint8_t flag)
{
    return cpu->sr & flag;
}

void set_carry_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_C, value); }
void set_zero_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_Z, value); }
void set_interrupt_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_I, value); }
void set_decimal_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_D, value); }
void set_break_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_B, value); }
void set_overflow_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_V, value); }
void set_negative_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_N, value); }

bool get_carry_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_C); }
bool get_zero_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_Z); }
bool get_interrupt_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_I); }
bool get_decimal_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_D); }
bool get_break_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_B); }
bool get_overflow_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_V); }
bool get_negative_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_N); }

uint8_t read_accumulator(cpu_6510_t *cpu)
{
    return cpu->a;
}

void write_accumulator(cpu_6510_t *cpu, uint8_t value)
{
    cpu->a = value;
}

uint8_t read_xr(cpu_6510_t *cpu)
{
    return cpu->x;
}

void write_xr(cpu_6510_t *cpu, uint8_t value)
{
    cpu->x = value;
}

uint8_t read_yr(cpu_6510_t *cpu)
{
    return cpu->y;
}

void write_yr(cpu_6510_t *cpu, uint8_t value)
{
    cpu->y = value;
}

uint8_t read_sp(cpu_6510_t *cpu)
{
    return cpu->sp;
}

void write_sp(cpu_6510_t *cpu, uint8_t value)
{
    cpu->sp = value;
}


uint16_t read_program_counter(cpu_6510_t *cpu)
{
    return cpu->pc;
}

void write_program_counter(cpu_6510_t *cpu, uint16_t value)
{
    cpu->pc = value;
}

uint64_t read_cycles(cpu_6510_t *cpu)
{
    return cpu->cycles;
}

void increment_cycles(cpu_6510_t *cpu, uint8_t value)
{
    cpu->cycles += value;
}

uint8_t fetch_instruction(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    return ram[pc];
}

instruction_t decode_instruction(uint8_t opcode)
{
    return instruction_set[opcode];
}

uint8_t *decode_address_implied(cpu_6510_t *cpu, memory_t ram)
{
    return NULL;
}

uint8_t *decode_address_accumulator(cpu_6510_t *cpu, memory_t ram)
{
    return &cpu->a;
}

uint8_t *decode_address_immediate(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    return &ram[pc];
}

uint8_t *decode_address_absolute(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 2);
    uint8_t low_addr = ram[pc];
    uint8_t high_addr = ram[pc + 1];
    uint16_t address = high_addr << 8 | low_addr;
    return &ram[address];
}

uint8_t *decode_address_absolute_x(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 2);
    uint8_t low_addr = ram[pc];
    uint8_t high_addr = ram[pc + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->x;
    if( (address ^ new_address) & 0xFF00 != 0) cpu->cycles++;
    return &ram[new_address];
}

uint8_t *decode_address_absolute_y(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 2);
    uint8_t low_addr = ram[pc];
    uint8_t high_addr = ram[pc + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    if( (address ^ new_address) & 0xFF00 != 0) cpu->cycles++;
    return &ram[new_address];
}

uint8_t *decode_address_zeropage(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc];
    return &ram[zp_addr];
}

uint8_t *decode_address_zeropage_x(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc] + cpu->x;
    return &ram[zp_addr];
}

uint8_t *decode_address_zeropage_y(cpu_6510_t *cpu, memory_t ram)
{
    return NULL;
}

int8_t *decode_address_relative(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    return (int8_t*)&ram[pc];
}

uint8_t *decode_address_indirect(cpu_6510_t *cpu, memory_t ram)
{
    return NULL;
}

uint8_t *decode_address_indirect_x(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc];
    uint8_t low_addr = ram[zp_addr + cpu->x];
    uint8_t high_addr = ram[zp_addr + cpu->x + 1];
    uint16_t address = high_addr << 8 | low_addr;
    return &ram[address];
}

uint8_t *decode_address_indirect_y(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc];
    uint8_t low_addr = ram[zp_addr];
    uint8_t high_addr = ram[zp_addr + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    if( (address ^ new_address) & 0xFF00 != 0) cpu->cycles++;
    return &(ram[new_address]);
}

void dump_cpu(cpu_6510_t *cpu, FILE *file)
{
  char flags[9];
  flags[0] = get_negative_flag(cpu)?'x':'.';
  flags[1] = get_overflow_flag(cpu)?'x':'.';
  flags[2] = '.';
  flags[3] = get_break_flag(cpu)?'x':'.';
  flags[4] = get_decimal_flag(cpu)?'x':'.';
  flags[5] = get_interrupt_flag(cpu)?'x':'.';
  flags[6] = get_zero_flag(cpu)?'x':'.';
  flags[7] = get_carry_flag(cpu)?'x':'.';
  flags[8] = 0;
		  
  fprintf(file, "AC:   %02X XR: %02X YR: %02X NV-BDIZC\n", cpu->a, cpu->x, cpu->y);
  fprintf(file, "PC: %04X SP: %02X SR: %02X %s\n\n", cpu->pc, cpu->sp,cpu->sr, flags);
}
