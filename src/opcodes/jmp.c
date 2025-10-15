#include "jmp.h"

void jmp(cpu_6510_t *cpu, uint8_t *address)
{
   write_program_counter(cpu, (uint16_t) address);
}

void f4C(cpu_6510_t *cpu, memory_t ram)
{
   uint8_t *address = decode_address_absolute(cpu, ram);
   jmp(cpu, address);
   increment_cycles(cpu, 3);
}

void f6C(cpu_6510_t *cpu, memory_t ram)
{
   uint8_t *address = decode_address_indirect(cpu, ram);
   jmp(cpu, address);
   increment_cycles(cpu, 5);
}