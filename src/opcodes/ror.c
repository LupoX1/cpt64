#include "ror.h"

uint8_t ror(cpu_6510_t *cpu, uint8_t value)
{
    bool new_carry = (value & 0x01) != 0;
    bool old_carry = get_carry_flag(cpu);
    uint8_t result = value >> 1;
    set_carry_flag(cpu, new_carry);
    if(old_carry){
        result = result | 0x80;
    }
    set_zero_flag(cpu, result != 0);
    set_negative_flag(cpu, (result & 0x80) != 0);
    return result;
}

void f66(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_zeropage(cpu, ram);
  uint8_t result = ror(cpu, ram[address]);
  ram[address] = result;
}

void f6A(cpu_6510_t *cpu, memory_t ram)
{
  uint8_t acc = read_accumulator(cpu);
  uint8_t result = ror(cpu, acc);
  write_accumulator(cpu, acc);
}

void f6E(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_absolute(cpu, ram);
  uint8_t result = ror(cpu, ram[address]);
  ram[address] = result;
}

void f76(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_zeropage_x(cpu, ram);
  uint8_t result = ror(cpu, ram[address]);
  ram[address] = result;
}

void f7E(cpu_6510_t *cpu, memory_t ram)
{
  uint16_t address = decode_address_absolute_x(cpu, ram);
  uint8_t result = ror(cpu, ram[address]);
  ram[address] = result;
}