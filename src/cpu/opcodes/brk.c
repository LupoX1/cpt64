#include "brk.h"

void f00(cpu_6510_t *cpu, memory_t ram)
{
/*
 *BRK (short for "BReaKpoint") is the mnemonic for a machine language instruction which sets the break and interrupt flags, increments the program counter by two and stores it along with the processor status flags onto the stack. Finally it raises an IRQ interrupt event.

BRK is intended for use as a debugging tool which a programmer may place at specific points in a program, to check the state of processor flags at these points in the code. Most machine language monitors are "hooked" onto the warm start, and set up to fetch and display the program counter and processor status flags that were pushed onto the stack by a BRK instruction.

Note that since BRK increments the program counter by 2 instead of 1, it is advisable to use a NOP after it to avoid issues.

 */


    uint16_t pc = read_program_counter(cpu);
    write_program_counter(cpu, pc + 1);

    increment_cycles(cpu, 7);
}
