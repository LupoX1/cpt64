#include "bcs.h"

void fB0(cpu_6510_t *cpu, memory_t ram)
{
    int8_t *offset = decode_address_relative(cpu, ram);

    bool carry = get_carry_flag(cpu);
    if(carry){
        uint16_t pc = read_program_counter(cpu);
        write_program_counter(cpu, pc + *offset);
    }
}
