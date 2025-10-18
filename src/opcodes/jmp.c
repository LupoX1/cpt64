#include "jmp.h"

void f4C(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute(cpu, ram);
   write_program_counter(cpu, address);
}

void f6C(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_indirect(cpu, ram);
   write_program_counter(cpu, address);
}