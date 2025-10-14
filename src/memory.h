#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef uint8_t memory_t[0xFFFF];

uint8_t read(memory_t memory, uint16_t address);
void write(memory_t memory, uint16_t address, uint8_t value);

#endif /* MEMORY_H */
