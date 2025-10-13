#include "mos6510.h"
#include "adc.h"
#include "and.h"
#include "asl.h"
#include "bcc.h"
#include "bcs.h"
#include "beq.h"
#include "bit.h"

void bad(Mos6510 *cpu, Ram ram)
{
}

instruction_t instruction_set[256] = {
//    0x00  0x01  0x02  0x03  0x04  0x05  0x06  0x07  0x08  0x09  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F 
/*0*/ &bad, &bad, &bad, &bad, &bad, &bad, &f06, &bad, &bad, &bad, &f0A, &bad, &bad, &bad, &f0E, &bad,
/*1*/ &bad, &bad, &bad, &bad, &bad, &bad, &f16, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &f1E, &bad,
/*2*/ &bad, &f21, &bad, &bad, &f24, &f25, &bad, &bad, &bad, &f29, &bad, &bad, &f2C, &f2D, &bad, &bad,
/*3*/ &bad, &f31, &bad, &bad, &bad, &f35, &bad, &bad, &bad, &f39, &bad, &bad, &bad, &f3D, &bad, &bad,
/*4*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*5*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*6*/ &bad, &f61, &bad, &bad, &bad, &f65, &bad, &bad, &bad, &f69, &bad, &bad, &bad, &f6D, &bad, &bad,
/*7*/ &bad, &f71, &bad, &bad, &bad, &f75, &bad, &bad, &bad, &f79, &bad, &bad, &bad, &f7D, &bad, &bad,
/*8*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*9*/ &f90, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*A*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*B*/ &fB0, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*C*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*D*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*E*/ &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad,
/*F*/ &fF0, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad, &bad};

void set_flag(Mos6510 *cpu, uint8_t flag, bool value)
{
    if (value)
        cpu->sr = cpu->sr | flag;
    else
        cpu->sr = cpu->sr & ~flag;
}

bool get_flag(Mos6510 *cpu, uint8_t flag)
{
    return cpu->sr & flag;
}

void set_carry_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_C, value); }
void set_zero_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_Z, value); }
void set_interrupt_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_I, value); }
void set_decimal_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_D, value); }
void set_break_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_B, value); }
void set_overflow_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_V, value); }
void set_negative_flag(Mos6510 *cpu, bool value) { set_flag(cpu, FLAG_N, value); }

bool get_carry_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_C); }
bool get_zero_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_Z); }
bool get_interrupt_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_I); }
bool get_decimal_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_D); }
bool get_break_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_B); }
bool get_overflow_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_V); }
bool get_negative_flag(Mos6510 *cpu) { return get_flag(cpu, FLAG_N); }

uint16_t read_program_counter(Mos6510 *cpu)
{
    uint16_t pch = cpu->pch;
    uint16_t pcl = cpu->pcl;
    return pch << 8 | pcl;
}

void write_program_counter(Mos6510 *cpu, uint16_t value)
{
    cpu->pch = value >> 8;
    cpu->pcl = value & 0x00FF;
}

uint8_t fetch_instruction(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    return ram[pc];
}

instruction_t decode_instruction(uint8_t opcode)
{
    return instruction_set[opcode];
}

uint8_t *decode_address_implied(Mos6510 *cpu, Ram ram)
{
    return NULL;
}

uint8_t *decode_address_accumulator(Mos6510 *cpu, Ram ram)
{
    return &cpu->a;
}

uint8_t *decode_address_immediate(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    return &ram[pc];
}

uint8_t *decode_address_absolute(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 2);
    uint8_t low_addr = ram[pc];
    uint8_t high_addr = ram[pc + 1];
    uint16_t address = high_addr << 8 | low_addr;
    return &ram[address];
}

uint8_t *decode_address_absolute_x(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 2);
    uint8_t low_addr = ram[pc];
    uint8_t high_addr = ram[pc + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->x;
    // if( (address ^ new_address) & 0xFF00 != 0) cycles++;
    return &ram[new_address];
}

uint8_t *decode_address_absolute_y(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 2);
    uint8_t low_addr = ram[pc];
    uint8_t high_addr = ram[pc + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    // if( (address ^ new_address) & 0xFF00 != 0) cycles++;
    return &ram[new_address];
}

uint8_t *decode_address_zeropage(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc];
    return &ram[zp_addr];
}

uint8_t *decode_address_zeropage_x(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc] + cpu->x;
    return &ram[zp_addr];
}

uint8_t *decode_address_zeropage_y(Mos6510 *cpu, Ram ram)
{
    return NULL;
}

int8_t *decode_address_relative(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    return (int8_t*)&ram[pc];
}

uint8_t *decode_address_indirect(Mos6510 *cpu, Ram ram)
{
    return NULL;
}

uint8_t *decode_address_indirect_x(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc];
    uint8_t low_addr = ram[zp_addr + cpu->x];
    uint8_t high_addr = ram[zp_addr + cpu->x + 1];
    uint16_t address = high_addr << 8 | low_addr;
    return &ram[address];
}

uint8_t *decode_address_indirect_y(Mos6510 *cpu, Ram ram)
{
    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);
    uint8_t zp_addr = ram[pc];
    uint8_t low_addr = ram[zp_addr];
    uint8_t high_addr = ram[zp_addr + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    // if( (address ^ new_address) & 0xFF00 != 0) cycles++;
    return &(ram[new_address]);
}
