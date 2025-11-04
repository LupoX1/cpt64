#include <stdlib.h>
#include <string.h>
#include "c64.h"
#include "log/log.h"

#define C64_COLOR_BLACK 0x000000FF       //  0 Nero
#define C64_COLOR_WHITE 0xFFFFFFFF       //  1 Bianco
#define C64_COLOR_RED 0x813338FF         //  2 Rosso
#define C64_COLOR_CYAN 0x75CEC8FF        //  3 Ciano
#define C64_COLOR_PURPLE 0x8E3C97FF      //  4 Viola
#define C64_COLOR_GREEN 0x56AC4DFF       //  5 Verde
#define C64_COLOR_BLUE 0x2E2C9BFF        //  6 Blu
#define C64_COLOR_YELLOW 0xEDF171FF      //  7 Giallo
#define C64_COLOR_ORANGE 0x8E5029FF      //  8 Arancione
#define C64_COLOR_BROWN 0x553800FF       //  9 Marrone
#define C64_COLOR_LIGHT_RED 0xC46C71FF   // 10 Rosso chiaro
#define C64_COLOR_DARK_GRAY 0x4A4A4AFF   // 11 Grigio scuro
#define C64_COLOR_MEDIUM_GRAY 0x7B7B7BFF // 12 Grigio medio
#define C64_COLOR_LIGHT_GREEN 0xA9FF9FFF // 13 Verde chiaro
#define C64_COLOR_LIGHT_BLUE 0x706DEFFF  // 14 Blu chiaro
#define C64_COLOR_LIGHT_GRAY 0xB2B2B2FF  // 15 Grigio chiaro

uint32_t palette[16] = {
    C64_COLOR_BLACK,
    C64_COLOR_WHITE,
    C64_COLOR_RED,
    C64_COLOR_CYAN,
    C64_COLOR_PURPLE,
    C64_COLOR_GREEN,
    C64_COLOR_BLUE,
    C64_COLOR_YELLOW,
    C64_COLOR_ORANGE,
    C64_COLOR_BROWN,
    C64_COLOR_LIGHT_RED,
    C64_COLOR_DARK_GRAY,
    C64_COLOR_MEDIUM_GRAY,
    C64_COLOR_LIGHT_GREEN,
    C64_COLOR_LIGHT_BLUE,
    C64_COLOR_LIGHT_GRAY};

struct vic
{
    uint64_t cycles;
    uint16_t raster;
    uint8_t cycle;
    uint8_t registers[0x40];
    bool badline;
    uint32_t framebuffer[VIC_FRAME_WIDTH * VIC_FRAME_HEIGHT];
};

struct c64_framebuffer
{
    uint32_t data[VIC_FRAME_WIDTH * VIC_FRAME_HEIGHT];
};

static inline uint32_t to_raylib_color(uint32_t color)
{
    uint8_t r = (color >> 24) & 0xFF;
    uint8_t g = (color >> 16) & 0xFF;
    uint8_t b = (color >> 8) & 0xFF;
    uint8_t a = (color) & 0xFF;
    return (uint32_t)((r) | (g << 8) | (b << 16) | (a << 24));
}

// Read VIC bank from CIA2 ($DD00) — but here we read a mem location the emulator should map.
// The typical CIA2 register for VIC bank is $DD00 bits 0-1 (but actual port may differ).
// For safety, caller should ensure $DD00 is wired to mem_read callback.
// We'll read mem_read(0xDD00) directly.
uint16_t vic_get_bank_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t cia2 = bus_read(bus, 0xDD00);
    uint8_t bank = 3 - (cia2 & 0x03);        // 0..3
    uint16_t base = (uint16_t)bank * 0x4000; // 16KB banks
    return base;
}

// Compute screen base (offset inside selected bank) and return absolute 16-bit address
uint16_t vic_get_screen_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t d018 = vic->registers[0x18];
    uint16_t bank_base = vic_get_bank_base(vic, bus);
    uint16_t screen_offset_kb = (d018 >> 4) & 0x0F; // bits 4-7
    uint16_t addr = bank_base + (screen_offset_kb * 1024);
    return addr;
}

// Compute charset base (offset inside bank). According to docs bits 1..3 specify 2KB blocks.
// Bits weights 2,4,8 — so ( (d018 & 0x0E) >> 1 ) * 2048
uint16_t vic_get_charset_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t d018 = vic->registers[0x18];
    uint16_t bank_base = vic_get_bank_base(vic, bus);
    uint16_t char_block = ((d018 & 0x0E) >> 1) & 0x07; // 0..7
    uint16_t char_base = bank_base + (char_block * 2048);
    return char_base;
}

uint16_t get_raster_compare(vic_t *vic)
{
    uint16_t high = (uint16_t)vic->registers[0x11] & 0x00FF;
    uint16_t low = (uint16_t)vic->registers[0x12] & 0x00FF;
    return ((high & 0x0080) << 1) | low;
}

vic_t *vic_create()
{
    vic_t *vic = malloc(sizeof(vic_t));
    if (!vic)
        return NULL;

    vic->cycles = 0;
    vic->raster = 0;
    vic->cycle = 0;
    vic->badline = false;
    memset(vic->registers, 0, 0x40);
    memset(vic->framebuffer, 0, VIC_FRAME_WIDTH * VIC_FRAME_HEIGHT);

    return vic;
}

void vic_reset(vic_t *vic)
{
}

void vic_destroy(vic_t *vic)
{
    if (vic)
        free(vic);
}

void vic_tick(vic_t *vic, c64_bus_t *bus)
{
    if (vic->cycle == 0)
    {
        // Check raster IRQ
        uint16_t raster_cmp = get_raster_compare(vic);
        if (vic->raster == raster_cmp)
        {
            if (vic->registers[0x1A] & 0x01)
            {
                vic->registers[0x19] |= 0x81; // Raster IRQ + IRQ flag
                bus_trigger_cpu_irq(bus);
            }
        }

        // Determina badline
        uint8_t yscroll = vic->registers[0x11] & 0x07;
        bool den = vic->registers[0x11] & 0x10;
        vic->badline = den &&
                       (vic->raster >= 48 && vic->raster <= 247) &&
                       ((vic->raster & 0x07) == yscroll);
    }

    if (vic->cycle >= 14 && vic->cycle <= 53)
    {
        int char_index = vic->cycle - 14; // 0-39

        bool den = vic->registers[0x11] & 0x10;
        if (den && vic->raster >= 48 && vic->raster <= 247)
        {
            // Calcola indirizzi
            uint16_t screen_base = vic_get_screen_base(vic, bus);
            uint16_t charset_base = vic_get_charset_base(vic, bus);

            uint8_t char_row = (vic->raster - 48) / 8;
            uint8_t char_line = (vic->raster - 48) & 0x07;

            // Fetch carattere e colore
            uint16_t screen_addr = screen_base + (char_row * 40) + char_index;
            uint8_t char_code = bus_read(bus, screen_addr);

            uint16_t color_addr = 0xD800 + (char_row * 40) + char_index;
            uint8_t color = bus_read(bus, color_addr) & 0x0F;

            // Fetch bitmap carattere
            uint16_t charset_addr = charset_base + (char_code * 8) + char_line;
            uint8_t char_bitmap = bus_read(bus, charset_addr);

            // Colori
            uint32_t fg_color = to_raylib_color(palette[color]);
            uint32_t bg_color = to_raylib_color(palette[vic->registers[0x21] & 0x0F]);

            // Coordinate framebuffer
            uint16_t screen_x = char_index * 8;
            uint16_t screen_y = vic->raster - 48;

            if (screen_y >= 0 && screen_y < 200)
            {
                // Renderizza 8 pixel
                for (uint8_t pixel = 0; pixel < 8; pixel++)
                {
                    uint16_t pixel_x = screen_x + pixel;

                    if (pixel_x < 320)
                    {
                        bool pixel_on = (char_bitmap >> (7 - pixel)) & 1;
                        uint32_t pixel_color = pixel_on ? fg_color : bg_color;

                        vic->framebuffer[screen_y * 320 + pixel_x] = pixel_color;
                    }
                }
            }
        }
    }

    vic->cycle++;

    if (vic->cycle >= 63)
    {
        vic->cycle = 0;
        vic->raster++;

        if (vic->raster >= 312)
        {
            vic->raster = 0;
        }
    }
}

void vic_tickss(vic_t *vic, c64_bus_t *bus)
{
    // ========================================
    // CYCLE 0: Inizio linea
    // ========================================
    if (vic->cycle == 0)
    {
        // Aggiorna registro raster $D012
        vic->registers[0x12] = vic->raster & 0xFF;

        // Bit 8 del raster in $D011 bit 7
        if (vic->raster & 0x100)
        {
            vic->registers[0x11] |= 0x80;
        }
        else
        {
            vic->registers[0x11] &= ~0x80;
        }

        // Check raster IRQ
        uint16_t raster_irq = ((vic->registers[0x11] & 0x80) << 1) | vic->registers[0x12];
        if (vic->raster == raster_irq)
        {
            vic->registers[0x19] |= 0x81; // Raster IRQ + IRQ flag
            if (vic->registers[0x1A] & 0x01)
            {
                bus_trigger_cpu_irq(bus);
            }
        }

        // Determina badline
        uint8_t yscroll = vic->registers[0x11] & 0x07;
        bool den = vic->registers[0x11] & 0x10;
        vic->badline = den &&
                       (vic->raster >= 48 && vic->raster <= 247) &&
                       ((vic->raster & 0x07) == yscroll);
    }

    // ========================================
    // CYCLE 12-51: C-ACCESS (badline only)
    // ========================================
    if (vic->badline && vic->cycle >= 12 && vic->cycle <= 51)
    {
        // Fetch video matrix e color RAM
        // TODO: Implementare cache video_matrix[40] e color_line[40]
    }

    // ========================================
    // CYCLE 14-53: G-ACCESS e RENDERING
    // ========================================
    if (vic->cycle >= 14 && vic->cycle <= 53)
    {
        int char_index = vic->cycle - 14; // 0-39

        bool den = vic->registers[0x11] & 0x10;
        if (den && vic->raster >= 48 && vic->raster <= 247)
        {

            // Calcola indirizzi
            uint8_t mem_setup = vic->registers[0x18];
            uint16_t screen_base = ((mem_setup >> 4) & 0x0F) * 0x0400;
            uint16_t charset_base = ((mem_setup >> 1) & 0x07) * 0x0800;

            uint8_t char_row = (vic->raster - 48) / 8;
            uint8_t char_line = (vic->raster - 48) & 0x07;

            // Fetch carattere e colore
            uint16_t screen_addr = screen_base + (char_row * 40) + char_index;
            uint8_t char_code = bus_read(bus, screen_addr);

            uint16_t color_addr = 0xD800 + (char_row * 40) + char_index;
            uint8_t color = bus_read(bus, color_addr) & 0x0F;

            // Fetch bitmap carattere
            uint16_t charset_addr = charset_base + (char_code * 8) + char_line;
            uint8_t char_bitmap = bus_read(bus, charset_addr);

            // Colori
            uint32_t fg_color = palette[color];
            uint32_t bg_color = palette[vic->registers[0x21] & 0x0F];

            // Coordinate framebuffer
            int screen_x = char_index * 8;
            int screen_y = vic->raster - 48;

            if (screen_y >= 0 && screen_y < 200)
            {
                // Renderizza 8 pixel
                for (int pixel = 0; pixel < 8; pixel++)
                {
                    int pixel_x = screen_x + pixel;

                    if (pixel_x < 320)
                    {
                        bool pixel_on = (char_bitmap >> (7 - pixel)) & 1;
                        uint32_t pixel_color = pixel_on ? fg_color : bg_color;

                        vic->framebuffer[screen_y * 320 + pixel_x] = pixel_color;
                    }
                }
            }
        }
    }
    /*
    // ========================================
    // BORDER RENDERING
    // ========================================
    if (vic->cycle >= 14 && vic->cycle <= 53) {
        uint32_t border_color = palette[vic->registers[0x20] & 0x0F];
        int screen_y = vic->raster;

        // Top border (raster 0-47)
        if (screen_y < 48) {
            int start_x = (vic->cycle - 14) * 8;
            for (int x = start_x; x < start_x + 8 && x < 320; x++) {
                vic->framebuffer[screen_y * 320 + x] = border_color;
            }
        }

        // Bottom border (raster 248-299)
        if (screen_y >= 248 && screen_y < 300) {
            int start_x = (vic->cycle - 14) * 8;
            for (int x = start_x; x < start_x + 8 && x < 320; x++) {
                vic->framebuffer[screen_y * 320 + x] = border_color;
            }
        }

        // Left border
        if (screen_y >= 48 && screen_y < 248 && vic->cycle == 14) {
            for (int x = 0; x < 24; x++) {
                vic->framebuffer[screen_y * 320 + x] = border_color;
            }
        }

        // Right border
        if (screen_y >= 48 && screen_y < 248 && vic->cycle == 53) {
            for (int x = 296; x < 320; x++) {
                vic->framebuffer[screen_y * 320 + x] = border_color;
            }
        }
    }
    */
    // ========================================
    // AVANZAMENTO
    // ========================================
    vic->cycle++;

    if (vic->cycle >= 63)
    {
        vic->cycle = 0;
        vic->raster++;

        if (vic->raster >= 312)
        {
            vic->raster = 0;
        }
    }
}

uint8_t vic_read(vic_t *vic, uint16_t addr)
{
    uint8_t reg = (addr - VIC_ROM_ADDRESS) % 0x40;
    if (reg == 0x12)
        return (uint8_t)(vic->raster & 0x00FF);
    if (reg == 0x13 || reg == 0x14)
        return vic->registers[reg];
    if (reg >= 47)
        return 0xFF;
    return vic->registers[reg];
}

uint8_t vic_read_internal(vic_t *vic, uint8_t reg)
{
    return vic->registers[reg];
}

void vic_write(vic_t *vic, uint16_t addr, uint8_t value)
{
    uint8_t reg = (addr - VIC_ROM_ADDRESS) % 0x40;
    if (reg == 0x19)
    { /* IRQ trigger */
    }
    if (reg == 0x1A)
    { /* IRQ enable */
    }
    vic->registers[reg] = value;
}

uint32_t *vic_get_framebuffer(vic_t *vic)
{
    return vic->framebuffer;
}

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
    '?'};

void vic_log_screen(vic_t *vic, c64_bus_t *bus)
{
    char buf[41];
    uint8_t index = 0;
    for (uint16_t i = 0x0400; i < 0x07E9; i++)
    {
        uint8_t code = bus_read(bus, i);
        if (code > 63)
            code = 63;
        char c = charset[code];
        buf[index] = c;
        index++;
        if (index == 40)
        {
            buf[index] = 0;
            index = 0;
            printf("%s\n", buf);
        }
    }
}

bool vic_badline(vic_t *vic)
{
    return vic->badline;
}