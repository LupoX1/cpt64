#include "inc.h"

void inc(cpu_6510_t *cpu, uint8_t *value)
{
   *value = *value + 1;

   set_negative_flag(cpu, (*value & 0x80) != 0);
   set_zero_flag(cpu, *value == 0);
}

void fE6(cpu_6510_t *cpu, memory_t ram)
{
  uint8_t *address = decode_address_zeropage(cpu, ram);
  dec(cpu, address);
  increment_cycles(cpu, 5);
}

void fEE(cpu_6510_t *cpu, memory_t ram)
{
  uint8_t *address = decode_address_absolute(cpu, ram);
  dec(cpu, address);
  increment_cycles(cpu, 6);
}

void fF6(cpu_6510_t *cpu, memory_t ram)
{
  uint8_t *address = decode_address_zeropage_x(cpu, ram);
  dec(cpu, address);
  increment_cycles(cpu, 6);
}

void fFE(cpu_6510_t *cpu, memory_t ram)
{
  uint8_t *address = decode_address_absolute_x(cpu, ram);
  dec(cpu, address);
  increment_cycles(cpu, 7);
}


