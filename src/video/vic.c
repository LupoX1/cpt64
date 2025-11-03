#include <stdlib.h>
#include <string.h>
#include "c64.h"
#include "log/log.h"

#define BLACK 0x000000FF
#define WHITE 0xFFFFFFFF
#define RED 0x813338FF
#define CYAN 0x75CEC8FF
#define VIOLET 0x8E3C97FF
#define GREEN 0x65AC4DFF
#define BLUE 0x2E2C9BFF
#define YELLOW 0xEDF171FF
#define ORANGE 0x8E5029FF
#define BROWN 0x553800FF
#define LT_RED 0xC46C71FF
#define DK_GREY 0x4A4A4AFF
#define GREY 0x7B7B7BFF
#define LT_GREEN 0xA9FF9FFF
#define LT_BLUE 0x706BEDFF
#define LT_GREY 0xB2B2B2FF

/* Inizializza la palette del C64
void InitC64Palette(void) {
    palette[0]  = (Color){0x00,0x00,0x00,255}; // Nero
    palette[1]  = (Color){0xFF,0xFF,0xFF,255}; // Bianco
    palette[2]  = (Color){0x68,0x37,0x2B,255}; // Rosso
    palette[3]  = (Color){0x70,0xA4,0xB2,255}; // Ciano
    palette[4]  = (Color){0x6F,0x3D,0x86,255}; // Viola
    palette[5]  = (Color){0x58,0x8D,0x43,255}; // Verde
    palette[6]  = (Color){0x35,0x28,0x79,255}; // Blu
    palette[7]  = (Color){0xB8,0xC7,0x6F,255}; // Giallo
    palette[8]  = (Color){0x6F,0x4F,0x25,255}; // Arancio
    palette[9]  = (Color){0x43,0x39,0x00,255}; // Marrone
    palette[10] = (Color){0x9A,0x67,0x59,255}; // Rosa chiaro
    palette[11] = (Color){0x44,0x44,0x44,255}; // Grigio scuro
    palette[12] = (Color){0x6C,0x6C,0x6C,255}; // Grigio medio
    palette[13] = (Color){0x9A,0xD2,0x84,255}; // Verde chiaro
    palette[14] = (Color){0x6C,0x5E,0xB5,255}; // Azzurro
    palette[15] = (Color){0x95,0x95,0x95,255}; // Grigio chiaro
}*/

uint32_t palette[16] = {
    BLACK,
    WHITE,
    RED,
    CYAN,
    VIOLET,
    GREEN,
    BLUE,
    YELLOW,
    ORANGE,
    BROWN,
    LT_RED,
    DK_GREY,
    GREY,
    LT_GREEN,
    LT_BLUE,
    LT_GREY};

struct vic
{
    uint64_t cycles;
    uint16_t raster_counter;
    uint8_t cycle_in_line;
    uint8_t line;
    uint8_t subcycle;
    uint8_t registers[0x40];
    uint8_t color_ram[0x800];
    uint32_t framebuffer[VIC_FRAME_WIDTH * VIC_FRAME_HEIGHT];
};

struct c64_framebuffer
{
    uint32_t data[VIC_FRAME_WIDTH * VIC_FRAME_HEIGHT];
};

// Read VIC bank from CIA2 ($DD00) — but here we read a mem location the emulator should map.
// The typical CIA2 register for VIC bank is $DD00 bits 0-1 (but actual port may differ).
// For safety, caller should ensure $DD00 is wired to mem_read callback.
// We'll read mem_read(0xDD00) directly.
uint32_t vic_get_bank_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t cia2 = bus_read(bus, 0xDD00);
    uint8_t bank = cia2 & 0x03;     // 0..3
    return (uint32_t)bank * 0x4000; // 16KB banks
}

// Compute screen base (offset inside selected bank) and return absolute 16-bit address
uint32_t vic_get_screen_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t d018 = vic->registers[0x18];
    uint32_t bank_base = vic_get_bank_base(vic, bus);
    uint32_t screen_offset_kb = (d018 >> 4) & 0x0F; // bits 4-7
    return bank_base + (screen_offset_kb * 1024);
}

// Compute charset base (offset inside bank). According to docs bits 1..3 specify 2KB blocks.
// Bits weights 2,4,8 — so ( (d018 & 0x0E) >> 1 ) * 2048
uint32_t vic_get_charset_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t d018 = vic->registers[0x18];
    uint32_t bank_base = vic_get_bank_base(vic, bus);
    uint32_t char_block = ((d018 & 0x0E) >> 1) & 0x07; // 0..7
    return bank_base + (char_block * 2048);
}

/* -------------------------
   Core: render one character cell (8 pixels) at given raster line and column
   ------------------------- */
void vic_render_charcell_at(vic_t *vic, c64_bus_t *bus, int raster_line, int column)
{
    // determine which row of characters and which subrow inside character
    int vis_row = (raster_line - VIC_FIRST_VISIBLE_LINE) / 8;     // 0..24
    int row_in_char = (raster_line - VIC_FIRST_VISIBLE_LINE) % 8; // 0..7
    if (vis_row < 0 || vis_row >= 25)
        return; // outside text area

    // compute addresses
    uint32_t screen_base = vic_get_screen_base(vic, bus);
    uint16_t screen_addr = (uint16_t)(screen_base + (vis_row * 40) + column);
    uint8_t char_index = bus_read(bus, screen_addr);

    // color RAM: always at $D800 + offset (CPU view)
    uint16_t color_addr = 0xD800 + (vis_row * 40) + column;
    uint8_t color_byte = bus_read(bus, color_addr) & 0x0F;
    uint8_t fg_color = color_byte;
    uint8_t bg_color = vic->registers[0x21] & 0x0F; // $D021 background color

    // charset fetch
    uint32_t charset_base = vic_get_charset_base(vic, bus);
    uint16_t glyph_addr = (uint16_t)(charset_base + (char_index * 8) + row_in_char);
    uint8_t glyph = bus_read(bus, glyph_addr);

    log_debug("screen_base %u screen_addr %u char_index %u\n", screen_base, screen_addr, char_index);
    log_debug("color_addr %u color_byte %u\n", color_addr, color_byte);
    log_debug("charset_base %u glyph_addr %u glyph %u\n", charset_base, glyph_addr, glyph);
    log_debug("fg_color %u palette %u bg_color %u palette %u\n", fg_color, palette[fg_color], bg_color, palette[bg_color]);

    // destination in framebuffer
    int x0 = column * 8;
    int y = raster_line - VIC_FIRST_VISIBLE_LINE;
    if (y < 0 || y >= VIC_FRAME_HEIGHT)
        return;

        // expand bits: bit7 -> leftmost pixel
    for (int bit = 7; bit >= 0; --bit)
    {
        uint8_t pixel = (glyph & (1 << bit)) ? fg_color : bg_color;
        vic->framebuffer[y * VIC_FRAME_WIDTH + x0 + (7-bit)] = palette[pixel];
    }
}

/* -------------------------
   Render one raster line (all 40 columns) in text mode
   ------------------------- */
void vic_render_raster_line_textmode(vic_t *vic, c64_bus_t *bus, int raster_line)
{
    // Skip if DEN (display enable) is 0: draw border color instead
    bool den = (vic->registers[0x11] & 0x10) != 0; // $D011 bit4
    uint8_t border_color = vic->registers[0x20] & 0x0F;
    int y = raster_line - VIC_FIRST_VISIBLE_LINE;
    if (y < 0 || y >= VIC_FRAME_HEIGHT)
        return;

    if (!den)
    {
        // fill whole scanline with border color
        for (int i = 0; i < VIC_FRAME_WIDTH; ++i){
            vic->framebuffer[y * VIC_FRAME_WIDTH + i] = palette[border_color];
        }
        return;
    }

    // for text mode, render 40 columns
    for (int col = 0; col < 40; ++col)
    {
        vic_render_charcell_at(vic, bus, raster_line, col);
    }
}

uint16_t get_raster_compare(vic_t *vic)
{
    uint16_t high = (uint16_t)vic->registers[0x11] & 0x00FF;
    uint16_t low = (uint16_t)vic->registers[0x12] & 0x00FF;
    return ((high & 0x0080) << 1) | low;
}

/* -------------------------
   Advance VIC by a given number of VIC cycles (subcycles).
   The caller can convert CPU cycles -> vic cycles: vic_cycles = cpu_cycles * VIC_SUBCYCLES_PER_CPU
   This advances raster counters, optionally renders visible lines, and handles raster IRQs.
   ------------------------- */
void vic_step_vic_cycles(vic_t *vic, c64_bus_t *bus, int vic_cycles)
{
    while (vic_cycles > 0)
    {
        // advance one VIC cycle
        vic->cycle_in_line++;
        if (vic->cycle_in_line >= VIC_CYCLES_PER_LINE)
        {
            // end of line: render if visible, advance raster
            // Render the line: we render when the line falls in visible area
            int current_line = vic->raster_counter;
            if (current_line >= VIC_FIRST_VISIBLE_LINE && current_line < (VIC_FIRST_VISIBLE_LINE + VIC_VISIBLE_LINES))
            {
                vic_render_raster_line_textmode(vic, bus, current_line);
            }
            vic->cycle_in_line = 0;
            vic->raster_counter++;
            if (vic->raster_counter >= VIC_TOTAL_LINES)
                vic->raster_counter = 0;

            // update $D012 low byte and $D011 bit7 (read-only bit stored in regs[0x11]'s bit7)
            // Note: the hardware stores the raster counter internally; reading $D012 returns the low byte,
            // and bit7 high part is reflected by reading D011's bit7 (we keep D011 as CPU-writable but don't override bit7 here)
            // For simplicity we don't modify D011's bit7 on our own. We update the read-return via vic_reg_read earlier.

            // Set the D012 register value for writes/reads to use (we keep regs[0x12] as the comparator low byte)
            // Important: hardware updates internal raster counter; reading of $D012 will return (v->raster & 0xFF)
            // and we already handle that in vic_reg_read().

            // Check raster compare -> set IRQ flag if match
            uint16_t raster_value = (uint16_t)vic->raster_counter;
            // Note: raster_compare already computed on writes to D011/D012
            uint16_t raster_compare = get_raster_compare(vic);
            if (raster_value == raster_compare)
            {
                // set raster flag (bit0 of $D019)
                vic->registers[0x19] |= 0x01;
                // if enabled in IRQEN ($D01A bit0), assert IRQ
                if (vic->registers[0x1A] & 0x01)
                {
                    bus_trigger_cpu_irq(bus);
                }
            }
        }
        vic_cycles--;
    }
}

vic_t *vic_create()
{
    vic_t *vic = malloc(sizeof(vic_t));
    if (!vic)
        return NULL;

    vic->cycles = 0;
    vic->raster_counter = 0;
    vic->cycle_in_line = 0;
    vic->line = 0;
    vic->subcycle = 0;
    memset(vic->registers, 0, 0x40);
    memset(vic->color_ram, 0, 0x800);
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
    //vic_step_vic_cycles(vic, bus, 56);

    vic->cycles++;
    if(vic->cycles % 63 == 0) vic->raster_counter++;
    uint16_t raster_compare = vic->registers[0x11] & 0x80;
    raster_compare = (raster_compare << 1) | vic->registers[0x12];
    if(vic->raster_counter == raster_compare && (vic->registers[0x2a] & 0x01) ) {
        vic->registers[0x19] = vic->registers[0x19] | 0x80;
        bus_trigger_cpu_irq(bus);
    }
    if(vic->raster_counter >= 312) vic->raster_counter = 0;
}

uint8_t vic_read(vic_t *vic, uint16_t addr)
{
    uint8_t reg = (addr - VIC_ROM_ADDRESS) % 0x40;
    if (reg == 0x12)
        return (uint8_t)(vic->raster_counter & 0x00FF);
    if (reg == 0x13 || reg == 0x14)
        return vic->registers[reg];
    if (reg >= 47)
        return 0xFF;
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
    '?'
};

void vic_log_screen(vic_t *vic, c64_bus_t *bus)
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