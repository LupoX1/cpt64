#include <stdlib.h>
#include "c64.h"

#define BLACK       0x000000
#define WHITE       0xFFFFFF
#define RED         0x813338
#define CYAN        0x75CEC8
#define VIOLET      0x8E3C97
#define GREEN       0x65AC4D
#define BLUE        0x2E2C9B
#define YELLOW      0xEDF171
#define ORANGE      0x8E5029
#define BROWN       0x553800
#define LT_RED      0xC46C71
#define DK_GREY     0x4A4A4A
#define GREY        0x7B7B7B
#define LT_GREEN    0xA9FF9F
#define LT_BLUE     0x706BED
#define LT_GREY     0xB2B2B2

char charset[64] = {
    '@',
    'A',
    'B',
    'C',
    'D',
    'E',
    'F',
    'G',
    'H',
    'I',
    'J',
    'K',
    'L',
    'M',
    'N',
    'O',
    'P',
    'Q',
    'R',
    'S',
    'T',
    'U',
    'V',
    'W',
    'X',
    'Y',
    'Z',
    '[',
    '_',
    ']',
    '_',
    '_',
    ' ',
    '!',
    '"',
    '#',
    '$',
    '%',
    '&',
    '\'',
    '(',
    ')',
    '*',
    '+',
    ',',
    '-',
    '.',
    '/',
    '0',
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    ':',
    ';',
    '<',
    '=',
    '>',
    '?'
};

// color ram $D800-$DBE7 -> color = 0-15 = x0 - xF
// unused $DBE8-$DBFF

/*

https://www.cebix.net/VIC-Article.txt

                               The area at $d000-$dfff with
                                  CHAREN=1     CHAREN=0

 $ffff +--------------+  /$e000 +----------+  +----------+
       |  Kernal ROM  | /       |  I/O  2  |  |          |
 $e000 +--------------+/  $df00 +----------+  |          |
       |I/O, Char ROM |         |  I/O  1  |  |          |
 $d000 +--------------+\  $de00 +----------+  |          |
       |     RAM      | \       |  CIA  2  |  |          |
 $c000 +--------------+  \$dd00 +----------+  |          |
       |  Basic ROM   |         |  CIA  1  |  |          |
 $a000 +--------------+   $dc00 +----------+  | Char ROM |
       |              |         |Color RAM |  |          |
       .     RAM      .         |          |  |          |
       .              .   $d800 +----------+  |          |
       |              |         |   SID    |  |          |
 $0002 +--------------+         |registers |  |          |
       | I/O port DR  |   $d400 +----------+  |          |
 $0001 +--------------+         |   VIC    |  |          |
       | I/O port DDR |         |registers |  |          |
 $0000 +--------------+   $d000 +----------+  +----------+


 $ffff +----------+   --
       |          |
       |          |
       |          |
       |   RAM    | Bank 3
       |          |
       |          |
       |          |
 $c000 +----------+   --
       |          |
       |   RAM    |
       |          |
 $a000 +----------+ Bank 2
       | Char ROM |
 $9000 +----------+
       |   RAM    |
 $8000 +----------+   --
       |          |
       |          |
       |          |
       |   RAM    | Bank 1
       |          |
       |          |
       |          |
 $4000 +----------+   --
       |          |
       |   RAM    |
       |          |
 $2000 +----------+ Bank 0
       | Char ROM |
 $1000 +----------+
       |   RAM    |
 $0000 +----------+   --


#| Adr.  |Bit7|Bit6|Bit5|Bit4|Bit3|Bit2|Bit1|Bit0| Function
--+-------+----+----+----+----+----+----+----+----+------------------------
 0| $d000 |                  M0X                  | X coordinate sprite 0
--+-------+---------------------------------------+------------------------
 1| $d001 |                  M0Y                  | Y coordinate sprite 0
--+-------+---------------------------------------+------------------------
 2| $d002 |                  M1X                  | X coordinate sprite 1
--+-------+---------------------------------------+------------------------
 3| $d003 |                  M1Y                  | Y coordinate sprite 1
--+-------+---------------------------------------+------------------------
 4| $d004 |                  M2X                  | X coordinate sprite 2
--+-------+---------------------------------------+------------------------
 5| $d005 |                  M2Y                  | Y coordinate sprite 2
--+-------+---------------------------------------+------------------------
 6| $d006 |                  M3X                  | X coordinate sprite 3
--+-------+---------------------------------------+------------------------
 7| $d007 |                  M3Y                  | Y coordinate sprite 3
--+-------+---------------------------------------+------------------------
 8| $d008 |                  M4X                  | X coordinate sprite 4
--+-------+---------------------------------------+------------------------
 9| $d009 |                  M4Y                  | Y coordinate sprite 4
--+-------+---------------------------------------+------------------------
10| $d00a |                  M5X                  | X coordinate sprite 5
--+-------+---------------------------------------+------------------------
11| $d00b |                  M5Y                  | Y coordinate sprite 5
--+-------+---------------------------------------+------------------------
12| $d00c |                  M6X                  | X coordinate sprite 6
--+-------+---------------------------------------+------------------------
13| $d00d |                  M6Y                  | Y coordinate sprite 6
--+-------+---------------------------------------+------------------------
14| $d00e |                  M7X                  | X coordinate sprite 7
--+-------+---------------------------------------+------------------------
15| $d00f |                  M7Y                  | Y coordinate sprite 7
--+-------+----+----+----+----+----+----+----+----+------------------------
16| $d010 |M7X8|M6X8|M5X8|M4X8|M3X8|M2X8|M1X8|M0X8| MSBs of X coordinates
--+-------+----+----+----+----+----+----+----+----+------------------------
17| $d011 |RST8| ECM| BMM| DEN|RSEL|    YSCROLL   | Control register 1
--+-------+----+----+----+----+----+--------------+------------------------
18| $d012 |                 RASTER                | Raster counter
--+-------+---------------------------------------+------------------------
19| $d013 |                  LPX                  | Light pen X
--+-------+---------------------------------------+------------------------
20| $d014 |                  LPY                  | Light pen Y
--+-------+----+----+----+----+----+----+----+----+------------------------
21| $d015 | M7E| M6E| M5E| M4E| M3E| M2E| M1E| M0E| Sprite enabled
--+-------+----+----+----+----+----+----+----+----+------------------------
22| $d016 |  - |  - | RES| MCM|CSEL|    XSCROLL   | Control register 2
--+-------+----+----+----+----+----+----+----+----+------------------------
23| $d017 |M7YE|M6YE|M5YE|M4YE|M3YE|M2YE|M1YE|M0YE| Sprite Y expansion
--+-------+----+----+----+----+----+----+----+----+------------------------
24| $d018 |VM13|VM12|VM11|VM10|CB13|CB12|CB11|  - | Memory pointers
--+-------+----+----+----+----+----+----+----+----+------------------------
25| $d019 | IRQ|  - |  - |  - | ILP|IMMC|IMBC|IRST| Interrupt register
--+-------+----+----+----+----+----+----+----+----+------------------------
26| $d01a |  - |  - |  - |  - | ELP|EMMC|EMBC|ERST| Interrupt enabled
--+-------+----+----+----+----+----+----+----+----+------------------------
27| $d01b |M7DP|M6DP|M5DP|M4DP|M3DP|M2DP|M1DP|M0DP| Sprite data priority
--+-------+----+----+----+----+----+----+----+----+------------------------
28| $d01c |M7MC|M6MC|M5MC|M4MC|M3MC|M2MC|M1MC|M0MC| Sprite multicolor
--+-------+----+----+----+----+----+----+----+----+------------------------
29| $d01d |M7XE|M6XE|M5XE|M4XE|M3XE|M2XE|M1XE|M0XE| Sprite X expansion
--+-------+----+----+----+----+----+----+----+----+------------------------
30| $d01e | M7M| M6M| M5M| M4M| M3M| M2M| M1M| M0M| Sprite-sprite collision
--+-------+----+----+----+----+----+----+----+----+------------------------
31| $d01f | M7D| M6D| M5D| M4D| M3D| M2D| M1D| M0D| Sprite-data collision
--+-------+----+----+----+----+----+----+----+----+------------------------
32| $d020 |  - |  - |  - |  - |         EC        | Border color
--+-------+----+----+----+----+-------------------+------------------------
33| $d021 |  - |  - |  - |  - |        B0C        | Background color 0
--+-------+----+----+----+----+-------------------+------------------------
34| $d022 |  - |  - |  - |  - |        B1C        | Background color 1
--+-------+----+----+----+----+-------------------+------------------------
35| $d023 |  - |  - |  - |  - |        B2C        | Background color 2
--+-------+----+----+----+----+-------------------+------------------------
36| $d024 |  - |  - |  - |  - |        B3C        | Background color 3
--+-------+----+----+----+----+-------------------+------------------------
37| $d025 |  - |  - |  - |  - |        MM0        | Sprite multicolor 0
--+-------+----+----+----+----+-------------------+------------------------
38| $d026 |  - |  - |  - |  - |        MM1        | Sprite multicolor 1
--+-------+----+----+----+----+-------------------+------------------------
39| $d027 |  - |  - |  - |  - |        M0C        | Color sprite 0
--+-------+----+----+----+----+-------------------+------------------------
40| $d028 |  - |  - |  - |  - |        M1C        | Color sprite 1
--+-------+----+----+----+----+-------------------+------------------------
41| $d029 |  - |  - |  - |  - |        M2C        | Color sprite 2
--+-------+----+----+----+----+-------------------+------------------------
42| $d02a |  - |  - |  - |  - |        M3C        | Color sprite 3
--+-------+----+----+----+----+-------------------+------------------------
43| $d02b |  - |  - |  - |  - |        M4C        | Color sprite 4
--+-------+----+----+----+----+-------------------+------------------------
44| $d02c |  - |  - |  - |  - |        M5C        | Color sprite 5
--+-------+----+----+----+----+-------------------+------------------------
45| $d02d |  - |  - |  - |  - |        M6C        | Color sprite 6
--+-------+----+----+----+----+-------------------+------------------------
46| $d02e |  - |  - |  - |  - |        M7C        | Color sprite 7
--+-------+----+----+----+----+-------------------+------------------------
*/

// read 0400-07e7 (1024-2023) = 40 col x 25 row
// rom chars $D000–$DFFF
// color ram $D800–$DBFF
// $D021 background color

struct vic
{
    uint64_t cycles;
    uint16_t raster_counter;
    uint8_t line;
    uint8_t registers[47];
    uint8_t color_ram[0x800];
};

vic_t *vic_create()
{
    vic_t *vic = malloc(sizeof(vic_t));
    if (!vic)
        return NULL;

    vic->cycles = 0;
    return vic;
}

void vic_reset(vic_t *vic)
{
}

void vic_tick(vic_t *vic, cpu_t * cpu)
{
    vic->cycles++;
    if(vic->cycles % 63 == 0) vic->raster_counter++;
    uint16_t raster_compare = vic->registers[0x11] & 0x80;
    raster_compare = (raster_compare << 1) | vic->registers[0x12];
    if(vic->raster_counter == raster_compare && (vic->registers[0x2a] & 0x01) ) {
        vic->registers[0x19] = vic->registers[0x19] | 0x80;
        cpu_interrupt(cpu);
    }
    if(vic->raster_counter >= 312) vic->raster_counter = 0;
}

void vic_write(vic_t *vic, uint16_t addr, uint8_t value)
{
    vic->registers[addr-VIC_ROM_ADDRESS] = value;
}

uint8_t vic_read(vic_t *vic, uint16_t addr)
{
    uint8_t reg = (addr-VIC_ROM_ADDRESS) % 64;
    if(reg == 0x12) return (uint8_t) (vic->raster_counter & 0x00FF);
    if(reg >= 47) return 0xFF;
    return vic->registers[reg];
}

void vic_destroy(vic_t *vic)
{
    if (vic)
        free(vic);
}

uint16_t vic_decode_screen_address(vic_t *vic, c64_bus_t *bus)
{
    // BASE = read IO CIA $DD02 :  (~V) << 6
    // OFF  = read VIC ($D018 & 0xF0) << 10
    // ADDR = BASE + OFF
    uint16_t bank = bus_read(bus, 0xDD02);
    uint16_t base = ~bank << 6;
    uint16_t reg = vic->registers[0x18];
    uint16_t offset = (reg & 0x00F0) << 10;

    return base + offset;
}

uint16_t vic_decode_char_rom_address(vic_t *vic, c64_bus_t *bus)
{
    // bank0 = read VIC ($018 & 0x0E) << 10     
    // bank2 = bank0 + 0x8000
    uint16_t reg = vic->registers[0xD018 - VIC_ROM_ADDRESS];
    
    return (reg & 0x000E) << 10;
}

void decode_char_address(vic_t *vic, c64_bus_t *bus)
{
    char buf[41];
    uint8_t index = 0;
    for(uint16_t i = 0x0400; i< 0x07E9; i++)
    {
        uint8_t code = bus_read(bus, i);
        if(code > 63) code = 63;
        char c = charset[code];
        buf[index] = c;
        index++;
        if(index == 40)
        {
            buf[index] = 0;
            index = 0;
            printf("%s\n", buf);
        }
        
    }
}