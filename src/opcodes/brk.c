#include "brk.h"

void f00(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t pc = read_program_counter(cpu);
    uint8_t sp = read_sp(cpu);
    uint8_t sr = read_sr(cpu);

    write(ram, sp, pc + 2);
    write(ram, sp-1, sr);
    write_sp(cpu, sp-2);

    set_interrupt_flag(cpu, true);
    set_break_flag(cpu, true);

    write_program_counter(cpu, pc + 1);
    increment_cycles(cpu, 7);
}
