#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "c64_core.h"
#include "cpu.h"
#include "memory.h"

struct C64_Core
{
  memory_t ram;
  uint8_t basic[0x2000];
  uint8_t kernal[0x2000];
  uint8_t chars[0x1000];
  C64_Framebuffer framebuffer;
  cpu_6510_t *cpu;
};

void load_data(char *path,uint8_t *location, size_t size)
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

C64_Core *c64_core_create()
{
  C64_Core *core = calloc(1, sizeof(C64_Core));
  if (!core)
    return NULL;
  core->cpu = create_cpu();
  return core;
}

void c64_core_destroy(C64_Core *core)
{
  if (!core) return;
  destroy_cpu(core->cpu);
  free(core);
}

void c64_core_load_roms(C64_Core *core)
{
  load_data("roms/basic.bin",core->ram + 0xA000, 0x2000);
  load_data("roms/kernal.bin",core->ram + 0xE000, 0x2000);
  load_data("roms/chargen.bin",core->ram+0xD000, 0x1000);
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
  uint8_t opcode = fetch_instruction(core->cpu, core->ram);
  instruction_t execute = decode_instruction(opcode);
  printf("Execute: %02X @ %04X\n", opcode, read_program_counter(core->cpu));
  execute(core->cpu, core->ram);
  return true;
}

C64_Framebuffer *get_c64_framebuffer(C64_Core *core)
{
  if (core)
    return &core->framebuffer;
  return NULL;
}

void c64_log_status(C64_Core *core)
{ 
  printf("");
}