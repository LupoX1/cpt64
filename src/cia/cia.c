#include "core/bus.h"
#include "cia/cia.h"
#include "log/log.h"
#include <stdlib.h>
#include <string.h>

struct cia
{
    uint16_t latch_a;
    uint16_t latch_b;
    uint8_t ier;
    uint8_t registers[16];
};

cia_t *cia_create()
{
    cia_t *cia = malloc(sizeof(cia_t));
    if(!cia) {
        log_error("Cannot create CIA");
        return NULL;
    }
    cia->latch_a = 0;
    cia->ier = 0;
    memset(cia->registers, 0xFF, 16);
    return cia;
}

void cia_destroy(cia_t *cia)
{
    if(!cia) return;
    free(cia);
}

uint8_t cia_read_icr(cia_t *cia)
{
    uint8_t result = cia->registers[0xD]; // ritorna fonti + bit7 se IRQ attivo
    cia->registers[0xD] = 0;              // resetta pending dopo la lettura
    return result;
}

uint8_t cia_read(cia_t *cia, uint16_t addr)
{
    uint8_t i = (addr & 0x000F);
    if(i == 0xD)
    {
        return cia_read_icr(cia);
    }
    //log_debug("CIA1 READ Address: %04X Register: %u Value: %02X ", addr, i, cia->a[i]);
    return cia->registers[i];
}

void cia_write_ier(cia_t *cia, uint8_t value)
{
    if (value & 0x80)
        cia->ier |= (value & 0x7F);   // bit7=1 → abilita interrupt
    else
        cia->ier &= ~(value & 0x7F);  // bit7=0 → disabilita interrupt
}

void cia_write(cia_t *cia, uint16_t addr, uint8_t value)
{
    uint8_t i = (addr & 0x000F);
    if(i == 4)
    {
        cia->latch_a  = (cia->latch_a & 0xFF00) | value;
        return;
    }
    if(i == 5)
    {
        cia->latch_a  = (value << 8) | (cia->latch_a & 0x00FF);
        return;
    }
    if(i == 0xD)
    {
        cia_write_ier(cia, value);
        return;
    }
    cia->registers[i] = value;
    //log_debug("CIA1 WRITE Address: %04X Value: %02X Register: %u", addr, value, i);
}

void cia_raise_irq(cia_t *cia, uint8_t source_bit)
{
    cia->registers[0x0D] |= source_bit;    // memorizza la fonte (pending)
    if (cia->ier & source_bit) // se è abilitata in IER
        cia->registers[0x0D] |= 0x80;      // setta anche bit7 → "IRQ line active"
}

void cia_tick(cia_t *cia, c64_bus_t *bus)
{
    uint16_t timer_a = (cia->registers[5] << 8) | cia->registers[4];
    timer_a--;
    cia->registers[5] = (timer_a >> 8) & 0x00FF;
    cia->registers[4] = timer_a & 0x00FF;
    if (timer_a == 0) {
        cia->registers[5] = (cia->latch_a >> 8) & 0x00FF;
        cia->registers[4] = cia->latch_a & 0x00FF;
        // genera IRQ
        cia->registers[0xD] = cia->registers[0xD] | 0x01;          // Timer A underflow
        if (cia->ier & 0x01) {
            bus_trigger_cpu_irq(bus);
        }
    }
}