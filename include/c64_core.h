#ifndef C64_CORE_H
#define C64_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#include "cpu.h"

typedef struct
{
  uint8_t data[C64_FRAME_BUFFER_SIZE];
  bool dirty;
} C64_Framebuffer;

//typedef struct C64_Core C64_Core;

typedef struct
{
  memory_t ram;
  uint8_t basic[0x2000];
  uint8_t kernal[0x2000];
  uint8_t chars[0x1000];
  C64_Framebuffer framebuffer;
  cpu_6510_t *cpu;
} C64_Core;

void dump_data(C64_Core *core);

C64_Core *c64_core_create(void);
void c64_core_destroy(C64_Core *core);
bool c64_core_step(C64_Core *core);
void c64_core_reset(C64_Core *core);

bool c64_load_binary(C64_Core *core, const char *filename, uint16_t addr);

C64_Framebuffer *get_c64_framebuffer(C64_Core *core);

void c64_log_status(C64_Core *core);

#endif // C64_CORE_H
