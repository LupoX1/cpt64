#include "dec.h"

uint8_t dec(cpu_6510_t *cpu, uint8_t value)
{
   uint8_t result = value - 1;

   set_negative_flag(cpu, (result & 0x80) != 0);
   set_zero_flag(cpu, result == 0);

   return result;
}

void fC6(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_zeropage(cpu, ram);
  ram[address] = dec(cpu, ram[address]);
}

void fCE(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_absolute(cpu, ram);
  ram[address] = dec(cpu, ram[address]);
}

void fD6(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_zeropage_x(cpu, ram);
  ram[address] = dec(cpu, ram[address]);
}

void fDE(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_absolute_x(cpu, ram);
  ram[address] = dec(cpu, ram[address]);
}


