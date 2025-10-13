#ifndef MOS_6510_H
#define MOS_6510_H

#include "c64_types.h"

void set_carry_flag(Mos6510 *cpu, bool value);
void set_zero_flag(Mos6510 *cpu, bool value);
void set_interrupt_flag(Mos6510 *cpu, bool value);
void set_decimal_flag(Mos6510 *cpu, bool value);
void set_break_flag(Mos6510 *cpu, bool value);
void set_overflow_flag(Mos6510 *cpu, bool value);
void set_negative_flag(Mos6510 *cpu, bool value);

bool get_carry_flag(Mos6510 *cpu);
bool get_zero_flag(Mos6510 *cpu);
bool get_interrupt_flag(Mos6510 *cpu);
bool get_decimal_flag(Mos6510 *cpu);
bool get_break_flag(Mos6510 *cpu);
bool get_overflow_flag(Mos6510 *cpu);
bool get_negative_flag(Mos6510 *cpu);

uint16_t read_program_counter(Mos6510 *cpu);
void write_program_counter(Mos6510 *cpu, uint16_t value);

uint8_t fetch_instruction(Mos6510 *cpu, Ram ram);
instruction_t decode_instruction(uint8_t opcode);

uint8_t *decode_address_implied(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_accumulator(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_immediate(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_absolute(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_absolute_x(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_absolute_y(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_zeropage(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_zeropage_x(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_zeropage_y(Mos6510 *cpu, Ram ram);
int8_t  *decode_address_relative(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_indirect(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_indirect_x(Mos6510 *cpu, Ram ram);
uint8_t *decode_address_indirect_y(Mos6510 *cpu, Ram ram);

#endif // MOS_6510_H