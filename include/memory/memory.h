#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define RAM_SIZE 0x10000

typedef struct memory memory_t;

memory_t *memory_create();
uint8_t read_direct(memory_t*, uint16_t);
void write_direct(memory_t*, uint16_t, uint8_t);
bool memory_load_roms(memory_t*);
bool memory_load_binary(memory_t*, const char*, uint16_t);
void memory_dump(memory_t*, FILE*);
void memory_destroy(memory_t*);

#endif /* MEMORY_H */
