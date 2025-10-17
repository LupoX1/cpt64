#include "jsr.h"

void f20(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute(cpu, ram);
   uint16_t pc = read_program_counter(cpu);
   push(cpu, ram, pc + 2);
   write_program_counter(cpu, address);
   increment_cycles(cpu, 6);
}