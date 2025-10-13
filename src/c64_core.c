#include "c64_core.h"
#include "mos6510.h"

struct C64_Core
{
  C64_Framebuffer framebuffer;
  Ram ram;
  Mos6510 cpu;
  VicII video;
  Sid6581 audio;
  Cia6526 io;
};

C64_Core *c64_core_create()
{
  C64_Core *core = calloc(1, sizeof(C64_Core));
  if (!core)
    return NULL;
  return core;
}

void c64_core_destroy(C64_Core *core)
{
  if (core)
    free(core);
}

void c64_core_load_roms(C64_Core *core)
{
  core->ram[0xFFFC] = 0x31; // start routine
  core->ram[0xFFFD] = 0xE0; // start routine

  core->ram[0x0400] = 'X'; // screen
  core->ram[0xD000] = 'X'; // character rom
}

void c64_core_reset(C64_Core *core)
{
  core->cpu.pcl = core->ram[0xFFFC];
  core->cpu.pch = core->ram[0xFFFD];
}

bool c64_core_step(C64_Core *core)
{
  uint8_t opcode = fetch_instruction(&core->cpu, core->ram);
  instruction_t execute = decode_instruction(opcode);
  execute(&core->cpu, core->ram);
  return true;
}

C64_Framebuffer *get_c64_framebuffer(C64_Core *core)
{
  if (core)
    return &core->framebuffer;
  return NULL;
}
