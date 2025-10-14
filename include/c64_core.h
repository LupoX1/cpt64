#ifndef C64_CORE_H
#define C64_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

typedef struct
{
  uint8_t data[C64_FRAME_BUFFER_SIZE];
  bool dirty;
} C64_Framebuffer;

typedef struct C64_Core C64_Core;

C64_Core *c64_core_create(void);
void c64_core_destroy(C64_Core *core);
bool c64_core_step(C64_Core *core);
void c64_core_reset(C64_Core *core);

C64_Framebuffer *get_c64_framebuffer(C64_Core *core);

#endif // C64_CORE_H
