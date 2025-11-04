#ifndef C64_SYSTEM_H
#define C64_SYSTEM_H

#include <stdbool.h>
#include <stdint.h>
#include "core/bus.h"

typedef struct c64_system c64_system_t;

c64_system_t* c64_create();

bool c64_load_program(c64_system_t*, const char*, uint16_t);

void c64_reset(c64_system_t*);
bool c64_step(c64_system_t*);
uint64_t c64_get_cycles(c64_system_t*);
void c64_dump_state(c64_system_t*, const char*);
void c64_debug(c64_system_t*);
c64_bus_t* c64_get_bus(c64_system_t*);

void c64_destroy(c64_system_t*);

uint32_t *c64_get_framebuffer(c64_system_t* sys);

uint8_t c64_read_vic(c64_system_t* sys, uint8_t reg);

#endif // C64_SYSTEM_H