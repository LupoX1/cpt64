#include <string.h>
#include <stdlib.h>
#include "c64.h"

struct memory
{
  uint8_t ram[RAM_SIZE];
  uint8_t basic[BASIC_SIZE];
  uint8_t kernal[KERNAL_SIZE];
  uint8_t chargen[CHARGEN_SIZE];
};

bool load_file(const char *path, uint8_t *dest, size_t max_size);

memory_t *memory_create()
{
  memory_t *mem = malloc(sizeof(memory_t));
  if (!mem)
    return NULL;
  return mem;
}

uint8_t read_direct(memory_t *mem, uint16_t address)
{
  return mem->ram[address];
}

uint8_t read_basic(memory_t *mem, uint16_t address)
{
  return mem->basic[address - BASIC_ADDRESS];
}

uint8_t read_kernal(memory_t *mem, uint16_t address)
{
  return mem->kernal[address - KERNAL_ADDRESS];
}

uint8_t read_chargen(memory_t *mem, uint16_t address)
{
  return mem->chargen[address - CHARGEN_ADDRESS];
}

void write_direct(memory_t *mem, uint16_t address, uint8_t value)
{
  mem->ram[address] = value;
}

bool memory_load_roms(memory_t *mem)
{
  bool ok = true;

  // BASIC ROM $A000-$BFFF
  ok &= load_file("assets/roms/basic.bin", mem->basic, BASIC_SIZE);

  // KERNAL ROM $E000-$FFFF
  ok &= load_file("assets/roms/kernal.bin", mem->kernal, KERNAL_SIZE);

  // Character ROM $D000-$DFFF
  ok &= load_file("assets/roms/chargen.bin", mem->chargen, CHARGEN_SIZE);

  if (ok)
  {
    printf("ROMs loaded successfully\n");
  }

  return ok;
}

bool memory_load_binary(memory_t *mem, const char *path, uint16_t address)
{
  FILE *f = fopen(path, "rb");
  if (!f)
  {
    fprintf(stderr, "Error: Cannot open %s\n", path);
    return false;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  size_t available = RAM_SIZE - address;
  if (size > (long)available)
  {
    fprintf(stderr, "Warning: File truncated (%ld > %zu)\n", size, available);
    size = available;
  }

  size_t read = fread(&mem->ram[address], 1, size, f);
  fclose(f);

  printf("Loaded %zu bytes at $%04X\n", read, address);
  return read == (size_t)size;
}

void memory_dump_(memory_t *mem, FILE *file)
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

void memory_dump(c64_bus_t *bus, FILE *out)
{
  fprintf(out, "      00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F\n");

  for (uint32_t i = 0; i < RAM_SIZE; i += 16)
  {
    fprintf(out, "%04X  ", i);

    // Hex dump
    for (int j = 0; j < 8; j++)
    {
      fprintf(out, "%02X ", bus_read(bus, i + j));
    }
    fprintf(out, " ");
    for (int j = 8; j < 16; j++)
    {
      fprintf(out, "%02X ", bus_read(bus, i + j));
    }

    fprintf(out, " ");

    // ASCII dump
    for (int j = 0; j < 16; j++)
    {
      char c = bus_read(bus, i + j);
      if (c < 32 || c > 126)
        c = '.';
      fprintf(out, "%c", c);
    }

    fprintf(out, "\n");
  }
}

void memory_destroy(memory_t *mem)
{
  if (!mem)
    return;
  free(mem);
}

bool load_file(const char *path, uint8_t *dest, size_t max_size)
{
  FILE *f = fopen(path, "rb");
  if (!f)
  {
    fprintf(stderr, "Error: Cannot open %s\n", path);
    return false;
  }

  fseek(f, 0, SEEK_END);
  long size = ftell(f);
  fseek(f, 0, SEEK_SET);

  if (size > (long)max_size)
  {
    fprintf(stderr, "Error: File too large (%ld > %zu)\n", size, max_size);
    fclose(f);
    return false;
  }

  size_t read = fread(dest, 1, size, f);
  fclose(f);

  return read == (size_t)size;
}