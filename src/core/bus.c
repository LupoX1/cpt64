/*
typedef struct
{
  memory_t ram;
  uint8_t basic[0x2000];
  uint8_t kernal[0x2000];
  uint8_t chars[0x1000];
  cpu_6510_t *cpu;
} C64_Core;
 */

#include <stdio.h>
#include <stdlib.h>
#include "core/bus.h"

struct c64_bus
{
    int c;
};

c64_bus_t *bus_create()
{
    printf("bus_create\n");
    c64_bus_t *c64_bus = malloc(sizeof(c64_bus_t));
    if (!c64_bus)
        return NULL;
    return c64_bus;
}

void bus_load_roms(c64_bus_t *bus)
{
    if (!bus)
        return;
    printf("bus_load_roms\n");
}

bool bus_load_binary(c64_bus_t *bus, const char *program_file, uint16_t address)
{
    if (!bus)
        return false;
    printf("bus_load_binary\n");
    return false;
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

void c64_core_reset(C64_Core *core)
{
  c64_core_load_roms(core);

  uint8_t pcl = read(core->ram, 0xFFFC);
  uint8_t pch = read(core->ram, 0xFFFD);
  uint16_t pc = pch << 8 | pcl;
  write_program_counter(core->cpu, pc);
}
*/

void bus_reset(c64_bus_t *bus)
{
    if (!bus)
        return;
    printf("bus_reset\n");
}

bool bus_clock(c64_bus_t *bus)
{
    if (!bus)
        return false;
    printf("bus_clock\n");
    return false;
}

cpu_t *bus_get_cpu(c64_bus_t *bus)
{
    if (!bus)
        return false;
    printf("bus_get_cpu\n");
    return NULL;
}

memory_t *bus_get_ram(c64_bus_t *bus)
{
    if (!bus)
        return false;
    printf("bus_clock\n");
    return NULL;
}

void bus_destroy(c64_bus_t *bus)
{
    printf("bus_destroy\n");
    if (!bus)
        return;
    free(bus);
}