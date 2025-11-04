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

// Read VIC bank from CIA2 ($DD00) — but here we read a mem location the emulator should map.
// The typical CIA2 register for VIC bank is $DD00 bits 0-1 (but actual port may differ).
// For safety, caller should ensure $DD00 is wired to mem_read callback.
// We'll read mem_read(0xDD00) directly.
uint16_t vic_get_bank_base(vic_t *vic, c64_bus_t *bus)
{
    uint8_t cia2 = bus_read(bus, 0xDD00);
    uint8_t bank = 3 - (cia2 & 0x03);     // 0..3
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

void vic_ticks(vic_t *vic, c64_bus_t *bus)
{
    vic->cycles++;
    if(vic->cycles % 63 == 0) vic->raster++;
    uint16_t raster_compare = vic->registers[0x11] & 0x80;
    raster_compare = (raster_compare << 1) | vic->registers[0x12];
    if(vic->raster == raster_compare && (vic->registers[0x2a] & 0x01) ) {
        vic->registers[0x19] = vic->registers[0x19] | 0x80;
        bus_trigger_cpu_irq(bus);
    }
    if(vic->raster >= 312) vic->raster = 0;
}

void vic_tick(vic_t *vic, c64_bus_t *bus)
{    
    // ========================================
    // CYCLE 0: Inizio linea
    // ========================================
    if (vic->cycle == 0) {
        // Aggiorna registro raster $D012
        vic->registers[0x12] = vic->raster & 0xFF;
        
        // Bit 8 del raster in $D011 bit 7
        if (vic->raster & 0x100) {
            vic->registers[0x11] |= 0x80;
        } else {
            vic->registers[0x11] &= ~0x80;
        }
        
        // Check raster IRQ
        uint16_t raster_irq = ((vic->registers[0x11] & 0x80) << 1) | vic->registers[0x12];
        if (vic->raster == raster_irq) {
            vic->registers[0x19] |= 0x81;  // Raster IRQ + IRQ flag
            if (vic->registers[0x1A] & 0x01) {
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
    if (vic->badline && vic->cycle >= 12 && vic->cycle <= 51) {
        // Fetch video matrix e color RAM
        // TODO: Implementare cache video_matrix[40] e color_line[40]
    }
    
    // ========================================
    // CYCLE 14-53: G-ACCESS e RENDERING
    // ========================================
    if (vic->cycle >= 14 && vic->cycle <= 53) {
        int char_index = vic->cycle - 14;  // 0-39
        
        bool den = vic->registers[0x11] & 0x10;
        if (den && vic->raster >= 48 && vic->raster <= 247) {
            
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
            
            if (screen_y >= 0 && screen_y < 200) {
                // Renderizza 8 pixel
                for (int pixel = 0; pixel < 8; pixel++) {
                    int pixel_x = screen_x + pixel;
                    
                    if (pixel_x < 320) {
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
    
    if (vic->cycle >= 63) {
        vic->cycle = 0;
        vic->raster++;
        
        if (vic->raster >= 312) {
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

bool vic_badline(vic_t *vic)
{
    return vic->badline;
}