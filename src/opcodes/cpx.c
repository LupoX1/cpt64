#include "cpx.h"

void cpx(cpu_6510_t *cpu, uint8_t value)
{
   uint8_t xr = read_xr(cpu);
   uint8_t result = xr - value;

   set_negative_flag(cpu, (result & 0x80) != 0);
   set_zero_flag(cpu, result == 0);
   set_carry_flag(cpu, result > 0x00FF);
}

void fE0(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_immediate(cpu, ram);
  cpx(cpu, ram[address]);
}

void fE4(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_zeropage(cpu, ram);
   cpx(cpu, ram[address]);
}

void fEC(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute(cpu, ram);
   cpx(cpu, ram[address]);
}

