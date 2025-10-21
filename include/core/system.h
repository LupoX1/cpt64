#ifndef C64_SYSTEM_H
#define C64_SYSTEM_H

#include <stdbool.h>

typedef struct c64_system c64_system_t;

c64_system_t *c64_create();

bool c64_load_program(c64_system_t*, char*, uint16_t);

void c64_reset(c64_system_t*);
bool c64_step(c64_system_t*);
uint64_t c64_get_cycles(c64_system_t*);
void c64_dump_state(c64_system_t*, const char*);

void c64_destroy(c64_system_t*);

#endif // C64_SYSTEM_H