#include "brk.h"

void f00(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    uint8_t sp = read_sp(cpu);
    uint8_t sr = read_sr(cpu);

    push(cpu, ram, pc + 2);
    push(cpu, ram, sr);

    set_interrupt_flag(cpu, true);
    set_break_flag(cpu, true);

    increment_cycles(cpu, 7);
    increment_program_counter(cpu, 1);
}