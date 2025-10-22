#ifndef C64_H
#define C64_H

#include "config.h"
#include "core/system.h"
#include "core/bus.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include "cpu/addressing.h"
#include "cpu/opcodes.h"
#include "cpu/instructions.h"
#include "video/vic.h"

#define C64_CLOCK_PAL       985248
#define C64_CLOCK_NTSC      1022727
#define C64_CYCLES_PER_LINE 63
#define C64_LINES_PER_FRAME 312

#define RAM_SIZE        0x10000
#define BASIC_SIZE      0x2000
#define KERNAL_SIZE     0x2000
#define CHARGEN_SIZE    0x1000
#define BASIC_ADDRESS   0xA000
#define KERNAL_ADDRESS  0xE000
#define CHARGEN_ADDRESS 0xD000
#define LORAM       0x01
#define HIRAM       0x02
#define CHAREN      0x04
#define CASS_WRITE  0x08
#define CASS_MOTOR  0x10
#define CASS_SENSE  0x20

#endif