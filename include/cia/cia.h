#ifndef CIA_H
#define CIA_H

#include <stdint.h>

typedef struct cia cia_t;

cia_t *cia_create();
void cia_destroy(cia_t *cia);

uint8_t cia_read(cia_t *cia, uint16_t addr);
void cia_write(cia_t *cia, uint16_t addr, uint8_t value);

void cia_tick(cia_t *cia, struct c64_bus *bus);

#endif // CIA_H