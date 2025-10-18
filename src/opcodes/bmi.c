#include "bmi.h"

void f30(cpu_6510_t *cpu, memory_t ram)
{
    bool negative = get_negative_flag(cpu);
    if(negative){
        int16_t address = decode_address_relative(cpu, ram);
        uint16_t pc = read_program_counter(cpu);
        increment_cycles(cpu, 2);
        write_program_counter(cpu, address);
        return;
    }
}
