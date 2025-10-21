#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "core/system.h"

struct c64_system_t
{
    bool ok;
};

c64_system_t *c64_create()
{
    printf("c64_create\n");
    c64_system_t *c64_system = malloc(sizeof(c64_system_t));

    return c64_system;
}

bool c64_load_program(c64_system_t *c64_system, char *program_file, uint16_t address)
{
    printf("c64_load_program\n");
    return false;
}

void c64_reset(c64_system_t *c64_system)
{
    printf("c64_reset\n");
}

bool c64_step(c64_system_t *c64_system)
{
    printf("c64_step\n");
    return false;
}

uint64_t c64_get_cycles(c64_system_t *c64_system)
{
    printf("c64_step\n");
    return 0;
}

void c64_dump_state(c64_system_t *c64_system, char* log_file)
{
    printf("c64_dump_state to %s\n", log_file);
}

void c64_destroy(c64_system_t *c64_system)
{
    printf("\n");
    if(!c64_system) return;
    free(c64_system);
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

void dump_data(C64_Core *core)
{
  FILE *file = fopen("./dump.txt", "w+");
  if(!file) return;

  dump_cpu(core->cpu, file);
  dump_memory(core->ram, file);
  
  fclose(file);
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

bool c64_core_step(C64_Core *core)
{
  return cpu_step(core->cpu, core->ram);
}

C64_Framebuffer *get_c64_framebuffer(C64_Core *core)
{
  if (core)
    return &core->framebuffer;
  return NULL;
}

void c64_log_status(C64_Core *core)
{
  log_cpu(core->cpu, core->ram);
}



*/