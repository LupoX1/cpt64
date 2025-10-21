#ifndef C64_BUS_H
#define C64_BUS_H

#include <stdbool.h>
#include <stdint.h>
#include "cpu/cpu.h"
#include "memory/memory.h"

typedef struct c64_bus c64_bus_t;

c64_bus_t *bus_create();

void bus_load_roms(c64_bus_t*);
bool bus_load_binary(c64_bus_t*, const char*, uint16_t);
void bus_reset(c64_bus_t*);
bool bus_clock(c64_bus_t*);

cpu_t *bus_get_cpu(c64_bus_t*);
memory_t *bus_get_ram(c64_bus_t*);

void bus_destroy(c64_bus_t*);

#endif // C64_BUS_H

