#include <stdlib.h>

#include "c64_core.h"
#include "cpu.h"
#include "memory.h"

struct C64_Core
{
  memory_t ram;
  cpu_6510_t *cpu;
  C64_Framebuffer framebuffer;
};

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
  write(core->ram, 0xFFFC, 0x31); // start routine
  write(core->ram, 0xFFFD, 0xE0); // start routine

  write(core->ram, 0x0400, 0x31); // screen 
  write(core->ram, 0xD000, 0x31); // character rom 
}

void c64_core_reset(C64_Core *core)
{
  uint8_t pcl = read(core->ram, 0xFFFC);
  uint8_t pch = read(core->ram, 0xFFFD);
  uint16_t pc = pch << 8 | pcl;
  write_program_counter(core->cpu, pc);
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
