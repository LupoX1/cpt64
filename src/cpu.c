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
#include "opcodes/clc.h"
#include "opcodes/cld.h"
#include "opcodes/cli.h"
#include "opcodes/clv.h"
#include "opcodes/cmp.h"
#include "opcodes/cpx.h"
#include "opcodes/cpy.h"
#include "opcodes/dec.h"
#include "opcodes/dex.h"
#include "opcodes/dey.h"
#include "opcodes/eor.h"
#include "opcodes/inc.h"
#include "opcodes/inx.h"
#include "opcodes/iny.h"
#include "opcodes/jmp.h"
#include "opcodes/jsr.h"
#include "opcodes/lda.h"
#include "opcodes/ldx.h"
#include "opcodes/ldy.h"
#include "opcodes/lsr.h"
#include "opcodes/nop.h"
#include "opcodes/ora.h"
#include "opcodes/pha.h"
#include "opcodes/php.h"
#include "opcodes/pla.h"
#include "opcodes/plp.h"
#include "opcodes/rol.h"
#include "opcodes/ror.h"

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
    if (cpu)
        return cpu;
    return NULL;
}

void destroy_cpu(cpu_6510_t *cpu)
{
    if (cpu)
        free(cpu);
}

void bad(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    uint8_t opcode = read(ram, pc);
    printf("BAD %02X @ %04X\n", opcode, pc);
    increment_program_counter(cpu, 1);
}

instruction_t instruction_set[256] = {
    //    0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F
    /*0*/ &f00, &f01, &bad, &bad, &bad, &f05, &f06, &bad, &f08, &f09, &f0A, &bad, &bad, &f0D, &f0E, &bad,
    /*1*/ &f10, &f11, &bad, &bad, &bad, &f15, &f16, &bad, &f18, &f19, &bad, &bad, &bad, &f1D, &f1E, &bad,
    /*2*/ &f20, &f21, &bad, &bad, &f24, &f25, &f26, &bad, &f28, &f29, &f2A, &bad, &f2C, &f2D, &f2E, &bad,
    /*3*/ &f30, &f31, &bad, &bad, &bad, &f35, &f36, &bad, &bad, &f39, &bad, &bad, &bad, &f3D, &f3E, &bad,
    /*4*/ &bad, &f41, &bad, &bad, &bad, &f45, &f46, &bad, &f48, &f49, &f4A, &bad, &f4C, &f4D, &f4E, &bad,
    /*5*/ &f50, &f51, &bad, &bad, &bad, &f55, &f56, &bad, &f58, &f59, &bad, &bad, &bad, &f5D, &f5E, &bad,
    /*6*/ &bad, &f61, &bad, &bad, &bad, &f65, &f66, &bad, &f68, &f69, &f6A, &bad, &f6C, &f6D, &f6E, &bad,
    /*7*/ &f70, &f71, &bad, &bad, &bad, &f75, &f76, &bad, &bad, &f79, &bad, &bad, &bad, &f7D, &f7E, &bad,
    /*8*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &f88, &bad, &bad, &bad, &bad, &bad,
    /*9*/ &f90, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
    /*A*/ &fA0, &fA1, &fA2, &bad, &fA4, &fA5, &fA6, &bad, &bad, &fA9, &bad, &bad, &fAC, &fAD, &fAE, &bad,
    /*B*/ &fB0, &fB1, &bad, &bad, &fB4, &fB5, &fB6, &bad, &fB8, &fB9, &bad, &bad, &fBC, &fBD, &fBE, &bad,
    /*C*/ &fC0, &fC1, &bad, &bad, &fC4, &fC5, &fC6, &bad, &fC8, &fC9, &fCA, &bad, &fCC, &fCD, &fCE, &bad,
    /*D*/ &fD0, &fD1, &bad, &bad, &bad, &fD5, &fD6, &bad, &fD8, &fD9, &bad, &bad, &bad, &fDD, &fDE, &bad,
    /*E*/ &fE0, &bad, &bad, &bad, &fE4, &bad, &fE6, &bad, &fE8, &bad, &fEA, &bad, &fEC, &bad, &fEE, &bad,
    /*F*/ &fF0, &bad, &bad, &bad, &bad, &bad, &fF6, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &fFE, &bad};

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

uint8_t read_sr(cpu_6510_t *cpu)
{
    return cpu->sr;
}

void write_sr(cpu_6510_t *cpu, uint8_t value)
{
    cpu->sr = value;
}

uint16_t read_program_counter(cpu_6510_t *cpu)
{
    return cpu->pc;
}

void write_program_counter(cpu_6510_t *cpu, uint16_t value)
{
    cpu->pc = value;
}

void increment_program_counter(cpu_6510_t *cpu, uint16_t value)
{
    cpu->pc = cpu->pc + value;
}

void push(cpu_6510_t *cpu, memory_t ram, uint8_t value)
{
    ram[cpu->sp] = value;
    cpu->sp = cpu->sp - 1;
}

uint8_t pop(cpu_6510_t *cpu, memory_t ram)
{
    cpu->sp = cpu->sp + 1;
    return ram[cpu->sp];
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
    return ram[cpu->pc];
}

instruction_t decode_instruction(uint8_t opcode)
{
    return instruction_set[opcode];
}

uint16_t decode_address_immediate(cpu_6510_t *cpu, memory_t ram)
{
    return cpu->pc + 1;
}

uint16_t decode_address_absolute(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    return high_addr << 8 | low_addr;
}

uint16_t decode_address_absolute_x(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->x;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

uint16_t decode_address_absolute_y(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

uint16_t decode_address_zeropage(cpu_6510_t *cpu, memory_t ram)
{
    return (uint16_t)ram[cpu->pc + 1];
}

uint16_t decode_address_zeropage_x(cpu_6510_t *cpu, memory_t ram)
{
    return (uint16_t)ram[cpu->pc + 1] + cpu->x;
}

uint16_t decode_address_zeropage_y(cpu_6510_t *cpu, memory_t ram)
{
    return (uint16_t)ram[cpu->pc + 1] + cpu->y;
}

uint16_t decode_address_relative(cpu_6510_t *cpu, memory_t ram)
{
    int8_t offset = (int8_t)ram[cpu->pc + 1];
    uint16_t address = cpu->pc + 2;
    uint16_t new_address = address + offset;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

uint16_t decode_address_indirect(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    uint16_t address = high_addr << 8 | low_addr;
    return ram[address + 1] << 8 | ram[address];
}

uint16_t decode_address_indirect_x(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t zp_addr = (uint16_t)ram[cpu->pc + 1];
    uint8_t low_addr = ram[zp_addr + cpu->x];
    uint8_t high_addr = ram[zp_addr + cpu->x + 1];
    return high_addr << 8 | low_addr;
}

uint16_t decode_address_indirect_y(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t zp_addr = (uint16_t)ram[cpu->pc + 1];
    uint8_t low_addr = ram[zp_addr];
    uint8_t high_addr = ram[zp_addr + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

void dump_cpu(cpu_6510_t *cpu, FILE *file)
{
    char flags[9];
    flags[0] = get_negative_flag(cpu) ? 'x' : '.';
    flags[1] = get_overflow_flag(cpu) ? 'x' : '.';
    flags[2] = '.';
    flags[3] = get_break_flag(cpu) ? 'x' : '.';
    flags[4] = get_decimal_flag(cpu) ? 'x' : '.';
    flags[5] = get_interrupt_flag(cpu) ? 'x' : '.';
    flags[6] = get_zero_flag(cpu) ? 'x' : '.';
    flags[7] = get_carry_flag(cpu) ? 'x' : '.';
    flags[8] = 0;

    fprintf(file, "AC:   %02X XR: %02X YR: %02X NV-BDIZC Cycle\n", cpu->a, cpu->x, cpu->y);
    fprintf(file, "PC: %04X SP: %02X SR: %02X %s %lu\n\n", cpu->pc, cpu->sp, cpu->sr, flags, cpu->cycles);
}