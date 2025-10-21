#ifndef C64_BUS_H
#define C64_BUS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct c64_bus c64_bus_t;

c64_bus_t* bus_create();
void bus_destroy(c64_bus_t*);

bool bus_load_roms(c64_bus_t*);
bool bus_load_binary(c64_bus_t*, const char*, uint16_t);
void bus_reset(c64_bus_t*);
bool bus_clock(c64_bus_t*);

struct cpu* bus_get_cpu(c64_bus_t*);
struct memory* bus_get_ram(c64_bus_t*);

void bus_write(c64_bus_t*, uint16_t, uint8_t);
uint8_t bus_read(c64_bus_t*, uint16_t);

#endif // C64_BUS_H

