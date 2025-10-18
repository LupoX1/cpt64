#include "cmp.h"

void cmp(cpu_6510_t *cpu, uint8_t value)
{
    uint8_t acc = read_accumulator(cpu);
    uint8_t result = acc - value;

    set_negative_flag(cpu, (result & 0x80) != 0);
    set_zero_flag(cpu, result == 0);
    set_carry_flag(cpu, result > 0x00FF);
}

void fC1(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_indirect_x(cpu, ram);
   cmp(cpu, ram[address]);
}

void fC5(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_zeropage(cpu, ram);
   cmp(cpu, ram[address]);
}

void fC9(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_immediate(cpu, ram);
  cmp(cpu, ram[address]);
}

void fCD(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute(cpu, ram);
   cmp(cpu, ram[address]);
}

void fD1(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_indirect_y(cpu, ram);
   cmp(cpu, ram[address]);
}

void fD5(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_zeropage_x(cpu, ram);
   cmp(cpu, ram[address]);
}

void fD9(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute_y(cpu, ram);
   cmp(cpu, ram[address]);
}

void fDD(cpu_6510_t *cpu, memory_t ram)
{
   uint16_t address = decode_address_absolute_x(cpu, ram);
   cmp(cpu, ram[address]);
}

