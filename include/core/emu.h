#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    struct c64_system *c64;
    const char *name;
    const char *version;
    uint64_t clock;
    uint16_t window_height;
    uint16_t window_width;
    bool running;
    bool stepping;
    bool paused;
} emu_t;

emu_t* emu_create();
void emu_shutdown(emu_t *emu);
void emu_update(emu_t *emu, uint64_t t, uint64_t dt);

// Funzioni da implementare (dichiarazioni)
// Emulatore
void emu_load_rom(emu_t *emu, const char *type, const char *path);  // type: "basic", "kernal", "charset"
bool emu_load_program(emu_t *emu, const char *path, uint16_t addr);
void emu_start(emu_t *emu);
void emu_pause(emu_t *emu);
void emu_resume(emu_t *emu);
void emu_stop(emu_t *emu);
void emu_step(emu_t *emu);
bool emu_is_running(emu_t *emu);
bool emu_is_paused(emu_t *emu);
void emu_save_state(emu_t *emu, const char *path);
void emu_load_state(emu_t *emu, const char *path);
void emu_reset(emu_t *emu);

uint8_t emu_read_vic(emu_t *emu, uint8_t reg);
// Informazioni stato
uint16_t emu_get_pc(emu_t *emu);
uint8_t emu_get_a(emu_t *emu);
uint8_t emu_get_x(emu_t *emu);
uint8_t emu_get_y(emu_t *emu);
uint8_t emu_get_sp(emu_t *emu);
uint8_t emu_get_status(emu_t *emu);
void emu_get_interrupt(emu_t *emu, char *irq);
void emu_get_flags(emu_t *emu, char *flags);
uint64_t emu_get_cycle(emu_t *emu);
const char* emu_get_current_instruction(emu_t *emu);
uint8_t emu_read_memory(emu_t *emu, uint16_t addr);
uint32_t *emu_get_framebuffer(emu_t *emu);  // Ritorna texture 320x200 del C64

#endif // EMU_H
