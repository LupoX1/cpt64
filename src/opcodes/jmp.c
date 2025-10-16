#include "jmp.h"

void f4C(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute(cpu, ram);
   write_program_counter(cpu, address);
   increment_cycles(cpu, 3);
   increment_program_counter(cpu, 3);
}

void f6C(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_indirect(cpu, ram);
   write_program_counter(cpu, address);
   increment_cycles(cpu, 5);
   increment_program_counter(cpu, 3);
}