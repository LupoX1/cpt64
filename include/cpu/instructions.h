#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define N_D NULL
#define ACC decode_address_accumulator
#define IMP decode_address_implied
#define IMM &decode_address_immediate
#define ABS decode_address_absolute
#define ABX decode_address_absolute_x
#define ABY decode_address_absolute_y
#define ZPG decode_address_zeropage
#define ZPX decode_address_zeropage_x
#define ZPY decode_address_zeropage_y
#define REL decode_address_relative
#define IND decode_address_indirect
#define INX decode_address_indirect_x
#define INY decode_address_indirect_y

addr_mode_t get_addressing_mode(uint8_t opcode);
opcode_t get_opcode_handler(uint8_t opcode);
uint8_t get_instruction_size(uint8_t opcode);
uint8_t get_cycles_count(uint8_t opcode);

#endif // INSTRUCTIONS_H