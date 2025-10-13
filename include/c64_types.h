#ifndef C64_TYPES_H
#define C64_TYPES_H

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define APP_NAME "Captain64"
#define APP_VERSION "0.0.1a"
#define APP_ID "com.example.ctp64"
#define C64_SCREEN_WIDTH 320
#define C64_SCREEN_HEIGHT 200
#define C64_FRAME_BUFFER_SIZE C64_SCREEN_WIDTH * C64_SCREEN_HEIGHT * 4

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

typedef uint8_t Ram[65536];

typedef struct
{
  uint8_t data[C64_FRAME_BUFFER_SIZE];
  bool dirty;
} C64_Framebuffer;

typedef struct
{
  uint8_t a;
  uint8_t x;
  uint8_t y;
  uint8_t pcl;
  uint8_t pch;
  uint8_t sr;
  uint8_t p;
} Mos6510;

typedef struct
{
  uint8_t placeholder;
} VicII;

typedef struct
{
  uint8_t placeholder;
} Sid6581;

typedef struct
{
  uint8_t placeholder;
} Cia6526;

typedef void (*instruction_t)(Mos6510 *cpu, Ram ram);

#endif // C64_TYPES_H