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
  cpu_6510_t *cpu;
  C64_Framebuffer framebuffer;
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

  dump(core->cpu, file);

  fprintf(file, "      00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F                   \n");

  char buffer[75];
  for(uint32_t i = 0; i <= 0xFFFF; i+= 16)
  {
    int pos = 0;

    pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%04X  ", i);
    for(uint16_t j = 0; j<8; j++)
    {
      pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%02X ", core->ram[i+j]);
    }
    pos += snprintf( buffer + pos, sizeof(buffer) - pos, " ");
    for(uint16_t j = 8; j<16; j++)
    {
       pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%02X ", core->ram[i+j]);
    }

    pos += snprintf( buffer + pos, sizeof(buffer) - pos, " ");
    for(uint16_t j = 0; j<8; j++)
    {
       char c = core->ram[i+j];
       if(c < 32 || c > 126) c = '.'; 
       pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%c", c);
    }
    pos += snprintf( buffer + pos, sizeof(buffer) - pos, " ");
    for(uint16_t j = 8; j<16; j++)
    {
       char c = core->ram[i+j];
       if(c < 32 || c > 126) c = '.';
       pos += snprintf( buffer + pos, sizeof(buffer) - pos, "%c", c);
    }
    pos += snprintf( buffer + pos, sizeof(buffer) - pos, "\n");
    fputs(buffer, file);
  }

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
  uint8_t pcl = read(core->ram, 0xFFFC);
  uint8_t pch = read(core->ram, 0xFFFD);
  uint16_t pc = pch << 8 | pcl;
  write_program_counter(core->cpu, pc);

  
  c64_core_load_roms(core);
}

bool c64_core_step(C64_Core *core)
{
  uint8_t opcode = fetch_instruction(core->cpu, core->ram);
  instruction_t execute = decode_instruction(opcode);
  execute(core->cpu, core->ram);
  return true;
}

C64_Framebuffer *get_c64_framebuffer(C64_Core *core)
{
  if (core)
    return &core->framebuffer;
  return NULL;
}
