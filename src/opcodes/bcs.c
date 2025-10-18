#include "bcs.h"

void fB0(cpu_6510_t *cpu, memory_t ram)
{
    bool carry = get_carry_flag(cpu);
    if(carry){
        int16_t address = decode_address_relative(cpu, ram);
        uint16_t pc = read_program_counter(cpu);
        increment_cycles(cpu, 2);
        write_program_counter(cpu, address);
        return;
    }
}
