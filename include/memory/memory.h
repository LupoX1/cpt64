#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define RAM_SIZE 0x10000

typedef struct memory memory_t;

struct memory *memory_create();
uint8_t read_direct(struct memory*, uint16_t);
void write_direct(struct memory*, uint16_t, uint8_t);
bool memory_load_roms(struct memory*);
bool memory_load_binary(struct memory*, const char*, uint16_t);
void memory_dump(struct memory*, FILE*);
void memory_destroy(struct memory*);

#endif /* MEMORY_H */
