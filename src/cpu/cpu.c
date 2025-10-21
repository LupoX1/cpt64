#include <stddef.h>
#include <stdlib.h>

#include "core/bus.h"
#include "cpu/cpu.h"
#include "cpu/opcodes.h"
#include "cpu/addressing.h"
#include "cpu/instructions.h"

#define INT_RESET 0x04
#define INT_NMI   0x02
#define INT_IRQ   0x01

struct cpu
{
    uint64_t cycles;
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sr;
    uint8_t sp;
    uint8_t int_pending;
};

extern bool cpu_execute_instruction(cpu_t *cpu, c64_bus_t *bus);

cpu_t *cpu_create()
{
    cpu_t *cpu = malloc(sizeof(cpu_t));
    if(!cpu) return NULL;

    cpu->sp = 0xFD;
    cpu->sr = FLAG_U | FLAG_I;
    cpu->int_pending = 0;

    return cpu;
}

void cpu_destroy(cpu_t *cpu)
{
    if(!cpu) return;
    free(cpu);
}

void cpu_dump(cpu_t *cpu, FILE *out)
{
    char flags[9];
    flags[0] = (cpu->sr & FLAG_N) ? 'N' : 'n';
    flags[1] = (cpu->sr & FLAG_V) ? 'V' : 'v';
    flags[2] = '-';
    flags[3] = (cpu->sr & FLAG_B) ? 'B' : 'b';
    flags[4] = (cpu->sr & FLAG_D) ? 'D' : 'd';
    flags[5] = (cpu->sr & FLAG_I) ? 'I' : 'i';
    flags[6] = (cpu->sr & FLAG_Z) ? 'Z' : 'z';
    flags[7] = (cpu->sr & FLAG_C) ? 'C' : 'c';
    flags[8] = '\0';
    
    fprintf(out, "PC: %04X  SP: %02X  A: %02X  X: %02X  Y: %02X\n",
            cpu->pc, cpu->sp, cpu->a, cpu->x, cpu->y);
    fprintf(out, "SR: %02X [%s]  Cycles: %lu\n",
            cpu->sr, flags, cpu->cycles);
}

void cpu_log(cpu_t *cpu, c64_bus_t *bus)
{
    uint8_t opcode = bus_read(bus, cpu->pc);
    
    printf("PC:%04X A:%02X X:%02X Y:%02X SP:%02X SR:%02X OP:%02X CY:%lu\n",
           cpu->pc, cpu->a, cpu->x, cpu->y, cpu->sp, cpu->sr, 
           opcode, cpu->cycles);
}

void cpu_reset(cpu_t *cpu, c64_bus_t *bus)
{
    cpu->int_pending |= INT_RESET;
    cpu_step(cpu, bus);
}

void cpu_trigger_irq(cpu_t *cpu)
{
    cpu->int_pending |= INT_IRQ;
}

void cpu_trigger_nmi(cpu_t *cpu)
{
    cpu->int_pending |= INT_NMI;
}

void cpu_get_state(cpu_t *cpu, cpu_state_t *state)
{
    if (!cpu || !state) return;
    state->pc = cpu->pc;
    state->sp = cpu->sp;
    state->a = cpu->a;
    state->x = cpu->x;
    state->y = cpu->y;
    state->sr = cpu->sr;
    state->cycles = cpu->cycles;
}

void cpu_set_state(cpu_t *cpu, cpu_state_t *state)
{
    if (!cpu || !state) return;
    cpu->pc = state->pc;
    cpu->sp = state->sp;
    cpu->a = state->a;
    cpu->x = state->x;
    cpu->y = state->y;
    cpu->sr = state->sr;
    cpu->cycles = state->cycles;
}


uint16_t cpu_get_pc(cpu_t *cpu) { return cpu->pc; }
void cpu_set_pc(cpu_t *cpu, uint16_t val) { cpu->pc = val; }
void cpu_inc_pc(cpu_t *cpu, uint16_t val) { cpu->pc += val; }

uint8_t cpu_get_a(cpu_t *cpu) { return cpu->a; }
void cpu_set_a(cpu_t *cpu, uint8_t val) { cpu->a = val; }

uint8_t cpu_get_x(cpu_t *cpu) { return cpu->x; }
void cpu_set_x(cpu_t *cpu, uint8_t val) { cpu->x = val; }

uint8_t cpu_get_y(cpu_t *cpu) { return cpu->y; }
void cpu_set_y(cpu_t *cpu, uint8_t val) { cpu->y = val; }

uint8_t cpu_get_sp(cpu_t *cpu) { return cpu->sp; }
void cpu_set_sp(cpu_t *cpu, uint8_t val) { cpu->sp = val; }

uint8_t cpu_get_sr(cpu_t *cpu) { return cpu->sr; }
void cpu_set_sr(cpu_t *cpu, uint8_t val) { cpu->sr = val; }

void cpu_inc_cycles(cpu_t *cpu, uint8_t val) { cpu->cycles += val; }

void cpu_set_flag(cpu_t *cpu, uint8_t flag, bool val) {
    cpu->sr = val ? (cpu->sr | flag) : (cpu->sr & ~flag);
}

bool cpu_get_flag(cpu_t *cpu, uint8_t flag) {
    return (cpu->sr & flag) != 0;
}

void cpu_push(cpu_t *cpu, c64_bus_t *bus, uint8_t value)
{
    uint16_t addr = 0x0100 | cpu->sp;
    bus_write(bus, addr, value);
    cpu->sp = (cpu->sp - 1) & 0xFF;
}

uint8_t cpu_pop(cpu_t *cpu, c64_bus_t *bus)
{
    cpu->sp = (cpu->sp + 1) & 0xFF;
    uint16_t addr = 0x0100 | cpu->sp;
    return bus_read(bus, addr);
}

void handle_reset(cpu_t *cpu, c64_bus_t *bus)
{
    cpu->int_pending &= ~INT_RESET;
    
    // Leggi vector RESET
    uint8_t lo = bus_read(bus, 0xFFFC);
    uint8_t hi = bus_read(bus, 0xFFFD);
    cpu->pc = (hi << 8) | lo;
    
    cpu->sr |= FLAG_I;
    cpu->sp = 0xFD;
    cpu->cycles += 7;
}

void handle_nmi(cpu_t *cpu, c64_bus_t *bus)
{
    cpu->int_pending &= ~INT_NMI;
    
    // Push PC e SR
    cpu_push(cpu, bus, (cpu->pc >> 8) & 0xFF);
    cpu_push(cpu, bus, cpu->pc & 0xFF);
    
    uint8_t sr = cpu->sr & ~FLAG_B;
    sr |= FLAG_U;
    cpu_push(cpu, bus, sr);
    
    cpu->sr |= FLAG_I;
    
    // Leggi vector NMI
    uint8_t lo = bus_read(bus, 0xFFFA);
    uint8_t hi = bus_read(bus, 0xFFFB);
    cpu->pc = (hi << 8) | lo;
    
    cpu->cycles += 7;
}

void handle_irq(cpu_t *cpu, c64_bus_t *bus)
{
    if (cpu->sr & FLAG_I) return;  // IRQ disabled
    
    cpu->int_pending &= ~INT_IRQ;
    
    // Push PC e SR
    cpu_push(cpu, bus, (cpu->pc >> 8) & 0xFF);
    cpu_push(cpu, bus, cpu->pc & 0xFF);
    
    uint8_t sr = cpu->sr & ~FLAG_B;
    sr |= FLAG_U;
    cpu_push(cpu, bus, sr);
    
    cpu->sr |= FLAG_I;
    
    // Leggi vector IRQ
    uint8_t lo = bus_read(bus, 0xFFFE);
    uint8_t hi = bus_read(bus, 0xFFFF);
    cpu->pc = (hi << 8) | lo;
    
    cpu->cycles += 7;
}

bool cpu_execute_instruction(cpu_t *cpu, c64_bus_t *bus)
{
    // Fetch opcode
    uint8_t opcode = bus_read(bus, cpu->pc);
    
    // Decode
    addr_mode_t addr_mode = get_addressing_mode(opcode);
    opcode_t handler = get_opcode_handler(opcode);
    
    if (!addr_mode || !handler) {
        // Opcode illegale
        return false;
    }
    
    // Calcola indirizzo
    uint16_t addr = addr_mode(cpu, bus);
    
    // Avanza PC
    cpu->pc += get_instruction_size(opcode);
    
    // Conta cicli base
    cpu->cycles += get_cycles_count(opcode);
    
    // Esegui
    return handler(cpu, bus, addr);
}

bool cpu_step(cpu_t *cpu, c64_bus_t *bus)
{
   // Gestisci interrupt in ordine di priorità
    if (cpu->int_pending & INT_RESET) {
        handle_reset(cpu, bus);
        return true;
    }
    
    if (cpu->int_pending & INT_NMI) {
        handle_nmi(cpu, bus);
        return true;
    }
    
    if (cpu->int_pending & INT_IRQ) {
        handle_irq(cpu, bus);
        return true;
    }
 
    // Esegui istruzione normale
    return cpu_execute_instruction(cpu, bus);
}