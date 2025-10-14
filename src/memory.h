#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdio.h>

typedef uint8_t memory_t[0xFFFF];

uint8_t read(memory_t memory, uint16_t address);
void write(memory_t memory, uint16_t address, uint8_t value);

void dump_memory(memory_t, FILE *file);

#endif /* MEMORY_H */
