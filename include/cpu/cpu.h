#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_U 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

typedef struct
{
    uint64_t cycles;
    uint16_t pc;
    uint8_t sp;
    uint8_t sr;
    uint8_t x;
    uint8_t y;
    uint8_t a;
} cpu_state_t;

typedef struct cpu cpu_t;

cpu_t *cpu_create();
void cpu_destroy(cpu_t *);
void cpu_dump(cpu_t *, FILE *);
void cpu_log(cpu_t *, c64_bus_t *);
void cpu_get_state(cpu_t *, cpu_state_t *);
void cpu_set_state(cpu_t *, cpu_state_t *);
void cpu_reset(cpu_t *, struct c64_bus *);
bool cpu_step(cpu_t *, struct c64_bus *);

uint16_t cpu_get_pc(cpu_t *);
void cpu_set_pc(cpu_t *, uint16_t);
void cpu_inc_pc(cpu_t *, uint16_t);
uint8_t cpu_get_a(cpu_t *);
void cpu_set_a(cpu_t *, uint8_t);
uint8_t cpu_get_x(cpu_t *);
void cpu_set_x(cpu_t *, uint8_t);
uint8_t cpu_get_y(cpu_t *);
void cpu_set_y(cpu_t *, uint8_t);
uint8_t cpu_get_sp(cpu_t *);
void cpu_set_sp(cpu_t *, uint8_t);
uint8_t cpu_get_sr(cpu_t *);
void cpu_set_sr(cpu_t *, uint8_t);
void cpu_inc_cycles(cpu_t *, uint8_t);
void cpu_set_flag(cpu_t *, uint8_t, bool);
bool cpu_get_flag(cpu_t *, uint8_t);
void cpu_push(cpu_t *, struct c64_bus *, uint8_t);
uint8_t cpu_pop(cpu_t *, struct c64_bus *);

void cpu_trigger_irq(cpu_t *cpu);

bool cpu_reset_pending(cpu_t *cpu);
bool cpu_nmi_pending(cpu_t *cpu);
bool cpu_irq_pending(cpu_t *cpu);

#endif // CPU_H