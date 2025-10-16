#include "beq.h"

void fF0(cpu_6510_t *cpu, memory_t ram)
{
    bool zero = get_zero_flag(cpu);
    if(zero){
        int16_t address = decode_address_relative(cpu, ram);
        uint16_t pc = read_program_counter(cpu);
        increment_cycles(cpu, 2);
        write_program_counter(cpu, address);
        return;
    }

    increment_cycles(cpu, 2);
    increment_program_counter(cpu, 2);
}
