#include "cpy.h"

void cpy(cpu_6510_t *cpu, uint8_t value)
{
   uint8_t yr = read_yr(cpu);
   uint8_t result = yr - value;

   set_negative_flag(cpu, (result & 0x80) != 0);
   set_zero_flag(cpu, result == 0);
   set_carry_flag(cpu, result > 0x00FF);
}

void fC0(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_immediate(cpu, ram);
  cpy(cpu, ram[address]);
}

void fC4(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_zeropage(cpu, ram);
   cpy(cpu, ram[address]);
}

void fCC(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute(cpu, ram);
   cpy(cpu, ram[address]);
}

