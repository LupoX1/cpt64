#include "inc.h"

uint8_t inc(cpu_6510_t *cpu, uint8_t value)
{
  uint8_t result  = value + 1;

   set_negative_flag(cpu, (result & 0x80) != 0);
   set_zero_flag(cpu, result == 0);
   return result;
}

void fE6(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_zeropage(cpu, ram);
  ram[address] = inc(cpu, ram[address]);
  increment_cycles(cpu, 5);
  increment_program_counter(cpu, 2);
}

void fEE(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_absolute(cpu, ram);
  ram[address] = inc(cpu, ram[address]);
  increment_cycles(cpu, 6);
  increment_program_counter(cpu, 3);
}

void fF6(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_zeropage_x(cpu, ram);
  ram[address] = inc(cpu, ram[address]);
  increment_cycles(cpu, 6);
  increment_program_counter(cpu, 2);
}

void fFE(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_absolute_x(cpu, ram);
  ram[address] = inc(cpu, ram[address]);
  increment_cycles(cpu, 7);
  increment_program_counter(cpu, 3);
}


