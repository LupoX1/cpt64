#ifndef VIC_H
#define VIC_H

#define VIC_ROM_ADDRESS     0xD000
#define VIC_ROM_SIZE        0x0400
#define COLOR_RAM_ADDRESS   0xD800
#define COLOR_RAM_SIZE      0x0800

typedef struct vic vic_t;
typedef struct c64_framebuffer c64_framebuffer_t;

vic_t* vic_create();
void vic_destroy(vic_t *);

void vic_reset(vic_t *);
void vic_tick(vic_t *, c64_bus_t *);

void vic_write(vic_t *, uint16_t , uint8_t);
uint8_t vic_read(vic_t *, uint16_t);

uint32_t *vic_get_framebuffer(vic_t *vic);

void vic_log_screen(vic_t *, c64_bus_t *);

bool vic_badline(vic_t *vic);

#endif // VIC_H