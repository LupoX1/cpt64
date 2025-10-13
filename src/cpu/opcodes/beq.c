#include "beq.h"

void fF0(Mos6510 *cpu, Ram ram)
{
    int8_t *offset = decode_address_relative(cpu, ram);

    bool zero = get_zero_flag(cpu);
    if(zero){
        uint16_t pc = read_program_counter(cpu);
        write_program_counter(cpu, pc + *offset);
    }
}