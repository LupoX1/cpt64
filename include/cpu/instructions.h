#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

addr_mode_idx_t get_addressing_mode_idx(uint8_t);
opcode_t get_opcode_handler(uint8_t);
uint8_t get_instruction_size(uint8_t);
uint8_t get_cycles_count(uint8_t);
char* get_instruction_name(uint8_t);

#endif // INSTRUCTIONS_H