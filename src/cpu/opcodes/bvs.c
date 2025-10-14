#include "bvs.h"

void f70(cpu_6510_t *cpu, memory_t ram)
{
    int8_t *offset = decode_address_relative(cpu, ram);

    bool overflow = get_overflow_flag(cpu);
    if(overflow){
        uint16_t pc = read_program_counter(cpu);
        write_program_counter(cpu, pc + *offset);
    }

    increment_cycles(cpu, 2);
}
