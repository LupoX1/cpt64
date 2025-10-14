#include "../memory.h"

uint8_t read(memory_t memory, uint16_t address)
{
  return memory[address];
}

void write(memory_t memory, uint16_t address, uint8_t value)
{
  memory[address] = value;
}

