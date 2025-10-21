#include "memory/memory.h"

uint8_t read(memory_t memory, uint16_t address)
{
  return memory[address];
}

void write(memory_t memory, uint16_t address, uint8_t value)
{
  memory[address] = value;
}

void dump_memory(memory_t ram, FILE *file)
{
  fprintf(file, "      00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F                   \n");

  char buffer[75];
  for(uint32_t i = 0; i <= 0xFFFF; i+= 16)
  {
    int pos = 0;

    pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%04X  ", i);
    for(uint16_t j = 0; j<8; j++)
    {
      pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%02X ", ram[i+j]);
    }
    pos += snprintf( buffer + pos, sizeof(buffer) - pos, " ");
    for(uint16_t j = 8; j<16; j++)
    {
       pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%02X ", ram[i+j]);
    }

    pos += snprintf( buffer + pos, sizeof(buffer) - pos, " ");
    for(uint16_t j = 0; j<8; j++)
    {
       char c = ram[i+j];
       if(c < 32 || c > 126) c = '.'; 
       pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%c", c);
    }
    pos += snprintf( buffer + pos, sizeof(buffer) - pos, " ");
    for(uint16_t j = 8; j<16; j++)
    {
       char c = ram[i+j];
       if(c < 32 || c > 126) c = '.';
       pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%c", c);
    }
    pos += snprintf( buffer + pos, sizeof(buffer) - pos, "\n");
    fputs(buffer, file);
  }
}

void memory_dump(memory_t *ram, FILE *file)
{
  printf("memory_dump\n");
}