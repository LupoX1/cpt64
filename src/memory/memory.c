#include <string.h>
#include <stdlib.h>
#include "memory/memory.h"

struct memory
{
  uint8_t ram[RAM_SIZE];
  uint8_t banking;
};

memory_t *memory_create()
{
  memory_t *mem = malloc(sizeof(memory_t));
  if (!mem)
    return NULL;
  memset(mem->ram, 0, RAM_SIZE);
  mem->banking = 0x37;
}

void memory_dump(memory_t *mem, FILE *file)
{
  fprintf(file, "      00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F                   \n");

  char buffer[75];
  for (uint32_t i = 0; i <= 0xFFFF; i += 16)
  {
    int pos = 0;

    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%04X  ", i);
    for (uint16_t j = 0; j < 8; j++)
    {
      pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%02X ", mem->ram[i + j]);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, " ");
    for (uint16_t j = 8; j < 16; j++)
    {
      pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%02X ", mem->ram[i + j]);
    }

    pos += snprintf(buffer + pos, sizeof(buffer) - pos, " ");
    for (uint16_t j = 0; j < 8; j++)
    {
      char c = mem->ram[i + j];
      if (c < 32 || c > 126)
        c = '.';
      pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%c", c);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, " ");
    for (uint16_t j = 8; j < 16; j++)
    {
      char c = mem->ram[i + j];
      if (c < 32 || c > 126)
        c = '.';
      pos += snprintf(buffer + pos, sizeof(buffer) - pos, "%c", c);
    }
    pos += snprintf(buffer + pos, sizeof(buffer) - pos, "\n");
    fputs(buffer, file);
  }
}

uint8_t read_direct(memory_t *mem, uint16_t address)
{
  return mem->ram[address];
}

void write_direct(memory_t *mem, uint16_t address, uint8_t value)
{
  mem->ram[address] = value;
}

void memory_destroy(memory_t *mem)
{
  if (!mem)
    return;
  free(mem);
}

/*




void load_data(char *path, uint8_t *location, size_t size)
{
  FILE *data = fopen(path, "rb");
  if(!data) return;

  uint8_t buffer[size];
  fread(buffer, sizeof(uint8_t), size, data);

  fclose(data);

  memcpy(location, buffer, size);
}

void c64_core_load_roms(C64_Core *core)
{
  load_data("roms/basic.bin",core->ram + 0xA000, 0x2000);
  load_data("roms/kernal.bin",core->ram + 0xE000, 0x2000);
  load_data("roms/chargen.bin",core->ram+0xD000, 0x1000);
}

bool c64_load_binary(C64_Core *core, const char *filename, uint16_t addr)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return false;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    size_t read = fread(&core->ram[addr], 1, size, f);
    fclose(f);

    printf("Loaded %ld bytes at $%04X\n", read, addr);
    return read == size;
}


*/