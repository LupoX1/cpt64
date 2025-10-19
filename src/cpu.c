#include <stddef.h>
#include <stdlib.h>

#include "cpu.h"
#include "opcodes.h"

struct cpu_6510_t
{
    uint64_t cycles;
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sr;
    uint8_t sp;
    bool reset;
    bool nmi;
    bool irq;
};

execute_t instruction_set[256] = {
    //    0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
    /*0*/ brk, ora, bad, bad, bad, ora, asl, bad, php, ora, asl, bad, bad, ora, asl, bad,
    /*1*/ bpl, ora, bad, bad, bad, ora, asl, bad, clc, ora, bad, bad, bad, ora, asl, bad,
    /*2*/ jsr, and, bad, bad, bit, and, rol, bad, plp, and, rol, bad, bit, and, rol, bad,
    /*3*/ bmi, and, bad, bad, bad, and, rol, bad, sec, and, bad, bad, bad, and, rol, bad,
    /*4*/ rti, eor, bad, bad, bad, eor, lsr, bad, pha, eor, lsr, bad, jmp, eor, lsr, bad,
    /*5*/ bvc, eor, bad, bad, bad, eor, lsr, bad, cli, eor, bad, bad, bad, eor, lsr, bad,
    /*6*/ rts, adc, bad, bad, bad, adc, ror, bad, pla, adc, ror, bad, jmp, adc, ror, bad,
    /*7*/ bvs, adc, bad, bad, bad, adc, ror, bad, sei, adc, bad, bad, bad, adc, ror, bad,
    /*8*/ bad, sta, bad, bad, sty, sta, stx, bad, dey, sta, txa, bad, sty, sta, stx, bad,
    /*9*/ bcc, sta, bad, bad, sty, sta, stx, bad, tya, sta, txs, bad, bad, sta, bad, bad,
    /*A*/ ldy, lda, ldx, bad, ldy, lda, ldx, bad, tay, lda, tax, bad, ldy, lda, ldx, bad,
    /*B*/ bsc, lda, bad, bad, ldy, lda, ldx, bad, clv, lda, tsx, bad, ldy, lda, ldx, bad,
    /*C*/ cpy, cmp, bad, bad, cpy, cmp, dec, bad, iny, cmp, dex, bad, cpy, cmp, dec, bad,
    /*D*/ bne, cmp, bad, bad, bad, cmp, dec, bad, cld, cmp, bad, bad, bad, cmp, dec, bad,
    /*E*/ cpx, sbc, bad, bad, cpx, sbc, inc, bad, inx, sbc, nop, bad, cpx, sbc, inc, bad,
    /*F*/ beq, sbc, bad, bad, bad, sbc, inc, bad, sed, sbc, bad, bad, bad, sbc, inc, bad};

address_mode_t address_modes[256] = {
    //    0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7  0x8  0x9  0xA  0xB  0xC  0xD  0xE  0xF
    /*0*/ IMP, INX, N_D, N_D, N_D, ZPG, ZPG, N_D, IMP, IMM, ACC, N_D, N_D, ABS, ABS, N_D,
    /*1*/ REL, INY, N_D, N_D, N_D, ZPX, ZPX, N_D, IMP, ABY, N_D, N_D, N_D, ABX, ABX, N_D,
    /*2*/ ABS, INX, N_D, N_D, ZPG, ZPG, ZPG, N_D, IMP, IMM, ACC, N_D, ABS, ABS, ABS, N_D,
    /*3*/ REL, INY, N_D, N_D, N_D, ZPX, ZPX, N_D, IMP, ABY, N_D, N_D, N_D, ABX, ABX, N_D,
    /*4*/ IMP, INY, N_D, N_D, N_D, ZPG, ZPG, N_D, IMP, IMM, ACC, N_D, ABS, ABS, ABS, N_D,
    /*5*/ REL, INY, N_D, N_D, N_D, ZPX, ZPX, N_D, IMP, ABY, N_D, N_D, N_D, ABX, ABX, N_D,
    /*6*/ IMP, INX, N_D, N_D, N_D, ZPG, ZPG, N_D, IMP, IMM, ACC, N_D, IND, ABS, ABS, N_D,
    /*7*/ REL, INY, N_D, N_D, N_D, ZPX, ZPX, N_D, IMP, ABY, N_D, N_D, N_D, ABX, ABX, N_D,
    /*8*/ N_D, INX, N_D, N_D, ZPG, ZPG, ZPG, N_D, IMP, N_D, IMP, N_D, ABS, ABS, ABS, N_D,
    /*9*/ REL, INY, N_D, N_D, ZPX, ZPX, ZPY, N_D, IMP, ABY, IMP, N_D, N_D, ABX, N_D, N_D,
    /*A*/ IMM, INX, IMM, N_D, ZPG, ZPG, ZPG, N_D, IMP, IMM, IMP, N_D, ABS, ABS, ABS, N_D,
    /*B*/ REL, INY, N_D, N_D, ZPX, ZPX, ZPY, N_D, IMP, ABY, IMP, N_D, ABX, ABX, ABY, N_D,
    /*C*/ IMM, INX, N_D, N_D, ZPG, ZPG, ZPG, N_D, IMP, IMM, IMP, N_D, ABS, ABS, ABS, N_D,
    /*D*/ REL, INY, N_D, N_D, N_D, ZPX, ZPX, N_D, IMP, ABY, N_D, N_D, N_D, ABX, ABX, N_D,
    /*E*/ IMM, INX, N_D, N_D, ZPG, ZPG, ZPG, N_D, IMP, IMM, IMP, N_D, ABS, ABS, ABS, N_D,
    /*F*/ REL, INY, N_D, N_D, N_D, ZPX, ZPX, N_D, IMP, ABY, N_D, N_D, N_D, ABX, ABX, N_D};

uint8_t instruction_cycles[256] = {
    /*    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F */
    /*0*/ 7, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 0, 4, 6, 0,
    /*1*/ 2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /*2*/ 6, 6, 0, 0, 3, 3, 5, 0, 4, 2, 2, 0, 4, 4, 6, 0,
    /*3*/ 2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /*4*/ 6, 6, 0, 0, 0, 3, 5, 0, 3, 2, 2, 0, 3, 4, 3, 0,
    /*5*/ 2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /*6*/ 6, 6, 0, 0, 0, 3, 5, 0, 4, 2, 2, 0, 5, 4, 6, 0,
    /*7*/ 2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /*8*/ 0, 6, 0, 0, 3, 3, 3, 0, 2, 0, 2, 0, 4, 4, 4, 0,
    /*9*/ 2, 6, 0, 0, 4, 4, 4, 0, 2, 5, 2, 0, 0, 5, 0, 0,
    /*A*/ 2, 6, 2, 0, 3, 3, 3, 0, 2, 2, 2, 0, 4, 4, 4, 0,
    /*B*/ 2, 5, 0, 0, 4, 4, 4, 0, 2, 4, 2, 0, 4, 4, 4, 0,
    /*C*/ 2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    /*D*/ 2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0,
    /*E*/ 2, 6, 0, 0, 3, 3, 5, 0, 2, 2, 2, 0, 4, 4, 6, 0,
    /*F*/ 2, 5, 0, 0, 0, 4, 6, 0, 2, 4, 0, 0, 0, 4, 7, 0};

uint8_t instruction_sizes[256] = {
    /*    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F */
    /*0*/ 1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 0, 3, 3, 0,
    /*1*/ 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,
    /*2*/ 3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 4, 3, 3, 0,
    /*3*/ 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,
    /*4*/ 1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*5*/ 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,
    /*6*/ 1, 2, 0, 0, 0, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*7*/ 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,
    /*8*/ 0, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
    /*9*/ 2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
    /*A*/ 2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*B*/ 2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    /*C*/ 2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*D*/ 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0,
    /*E*/ 2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    /*F*/ 2, 2, 0, 0, 0, 2, 2, 0, 1, 3, 0, 0, 0, 3, 3, 0};

cpu_6510_t *create_cpu()
{
    cpu_6510_t *cpu = malloc(sizeof(cpu_6510_t));
    if (cpu)
        return cpu;
    return NULL;
}

void destroy_cpu(cpu_6510_t *cpu)
{
    if (cpu)
        free(cpu);
}

void set_flag(cpu_6510_t *cpu, uint8_t flag, bool value) {if (value) cpu->sr = cpu->sr | flag; else cpu->sr = cpu->sr & ~flag; }
bool get_flag(cpu_6510_t *cpu, uint8_t flag){ return cpu->sr & flag; }

void set_carry_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_C, value); }
void set_zero_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_Z, value); }
void set_interrupt_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_I, value); }
void set_decimal_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_D, value); }
void set_break_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_B, value); }
void set_overflow_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_V, value); }
void set_negative_flag(cpu_6510_t *cpu, bool value) { set_flag(cpu, FLAG_N, value); }

bool get_carry_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_C); }
bool get_zero_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_Z); }
bool get_interrupt_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_I); }
bool get_decimal_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_D); }
bool get_break_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_B); }
bool get_overflow_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_V); }
bool get_negative_flag(cpu_6510_t *cpu) { return get_flag(cpu, FLAG_N); }

uint8_t read_accumulator(cpu_6510_t *cpu)
{
    return cpu->a;
}

void write_accumulator(cpu_6510_t *cpu, uint8_t value)
{
    cpu->a = value;
}

uint8_t read_xr(cpu_6510_t *cpu)
{
    return cpu->x;
}

void write_xr(cpu_6510_t *cpu, uint8_t value)
{
    cpu->x = value;
}

uint8_t read_yr(cpu_6510_t *cpu)
{
    return cpu->y;
}

void write_yr(cpu_6510_t *cpu, uint8_t value)
{
    cpu->y = value;
}

uint8_t read_sp(cpu_6510_t *cpu)
{
    return cpu->sp;
}

void write_sp(cpu_6510_t *cpu, uint8_t value)
{
    cpu->sp = value;
}

uint8_t read_sr(cpu_6510_t *cpu)
{
    return cpu->sr;
}

void write_sr(cpu_6510_t *cpu, uint8_t value)
{
    cpu->sr = value;
}

uint16_t read_program_counter(cpu_6510_t *cpu)
{
    return cpu->pc;
}

void write_program_counter(cpu_6510_t *cpu, uint16_t value)
{
    cpu->pc = value;
}

void increment_program_counter(cpu_6510_t *cpu, uint16_t value)
{
    cpu->pc = cpu->pc + value;
}

void push(cpu_6510_t *cpu, memory_t ram, uint8_t value)
{
    ram[cpu->sp] = value;
    cpu->sp = cpu->sp - 1;
}

uint8_t pop(cpu_6510_t *cpu, memory_t ram)
{
    cpu->sp = cpu->sp + 1;
    return ram[cpu->sp];
}

uint64_t read_cycles(cpu_6510_t *cpu)
{
    return cpu->cycles;
}

void increment_cycles(cpu_6510_t *cpu, uint8_t value)
{
    cpu->cycles += value;
}

uint8_t fetch_instruction(cpu_6510_t *cpu, memory_t ram)
{
    return ram[cpu->pc];
}

execute_t decode_instruction(uint8_t opcode)
{
    return instruction_set[opcode];
}

uint16_t decode_address_accumulator(cpu_6510_t *cpu, memory_t ram){return 0;}
uint16_t decode_address_implied(cpu_6510_t *cpu, memory_t ram){return 0;}

uint16_t decode_address_immediate(cpu_6510_t *cpu, memory_t ram)
{
    return cpu->pc + 1;
}

uint16_t decode_address_absolute(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    return high_addr << 8 | low_addr;
}

uint16_t decode_address_absolute_x(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->x;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

uint16_t decode_address_absolute_y(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

uint16_t decode_address_zeropage(cpu_6510_t *cpu, memory_t ram)
{
    return (uint16_t)ram[cpu->pc + 1];
}

uint16_t decode_address_zeropage_x(cpu_6510_t *cpu, memory_t ram)
{
    return (uint16_t)ram[cpu->pc + 1] + cpu->x;
}

uint16_t decode_address_zeropage_y(cpu_6510_t *cpu, memory_t ram)
{
    return (uint16_t)ram[cpu->pc + 1] + cpu->y;
}

uint16_t decode_address_relative(cpu_6510_t *cpu, memory_t ram)
{
    int8_t offset = (int8_t)ram[cpu->pc + 1];
    uint16_t address = cpu->pc + 2;
    uint16_t new_address = address + offset;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

uint16_t decode_address_indirect(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t low_addr = ram[cpu->pc + 1];
    uint8_t high_addr = ram[cpu->pc + 2];
    uint16_t address = high_addr << 8 | low_addr;
    return ram[address + 1] << 8 | ram[address];
}

uint16_t decode_address_indirect_x(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t zp_addr = (uint16_t)ram[cpu->pc + 1];
    uint8_t low_addr = ram[zp_addr + cpu->x];
    uint8_t high_addr = ram[zp_addr + cpu->x + 1];
    return high_addr << 8 | low_addr;
}

uint16_t decode_address_indirect_y(cpu_6510_t *cpu, memory_t ram)
{
    uint16_t zp_addr = (uint16_t)ram[cpu->pc + 1];
    uint8_t low_addr = ram[zp_addr];
    uint8_t high_addr = ram[zp_addr + 1];
    uint16_t address = high_addr << 8 | low_addr;
    uint16_t new_address = address + cpu->y;
    if (((address ^ new_address) & 0xFF00) != 0)
        cpu->cycles++;
    return new_address;
}

void dump_cpu(cpu_6510_t *cpu, FILE *file)
{
    char flags[9];
    flags[0] = get_negative_flag(cpu) ? 'x' : '.';
    flags[1] = get_overflow_flag(cpu) ? 'x' : '.';
    flags[2] = '.';
    flags[3] = get_break_flag(cpu) ? 'x' : '.';
    flags[4] = get_decimal_flag(cpu) ? 'x' : '.';
    flags[5] = get_interrupt_flag(cpu) ? 'x' : '.';
    flags[6] = get_zero_flag(cpu) ? 'x' : '.';
    flags[7] = get_carry_flag(cpu) ? 'x' : '.';
    flags[8] = 0;

    fprintf(file, "AC:   %02X XR: %02X YR: %02X NV-BDIZC Cycle\n", cpu->a, cpu->x, cpu->y);
    fprintf(file, "PC: %04X SP: %02X SR: %02X %s %lu\n\n", cpu->pc, cpu->sp, cpu->sr, flags, cpu->cycles);
}

void log_cpu(cpu_6510_t *cpu, memory_t ram)
{
    char flags[9];
    flags[0] = get_negative_flag(cpu) ? 'x' : '.';
    flags[1] = get_overflow_flag(cpu) ? 'x' : '.';
    flags[2] = '.';
    flags[3] = get_break_flag(cpu) ? 'x' : '.';
    flags[4] = get_decimal_flag(cpu) ? 'x' : '.';
    flags[5] = get_interrupt_flag(cpu) ? 'x' : '.';
    flags[6] = get_zero_flag(cpu) ? 'x' : '.';
    flags[7] = get_carry_flag(cpu) ? 'x' : '.';
    flags[8] = 0;

    //printf("\033[2J\033[1;1H");
    printf("AC:   %02X XR: %02X YR: %02X NV-BDIZC Cycle\n", cpu->a, cpu->x, cpu->y);
    printf("PC: %04X SP: %02X SR: %02X %s %lu\n", cpu->pc, cpu->sp, cpu->sr, flags, cpu->cycles);
    printf("%04X %04X %04X %04X\n", cpu->pc, cpu->pc + 1, cpu->pc + 2, cpu->pc + 3);
    printf("  %02X   %02X   %02X   %02X\n", ram[cpu->pc], ram[cpu->pc + 1], ram[cpu->pc + 2], ram[cpu->pc + 3]);
}

bool cpu_step(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t opcode = ram[cpu->pc];
    execute_t execute = instruction_set[opcode];
    address_mode_t decode_address = address_modes[opcode];
    uint16_t address = decode_address(cpu, ram);
    increment_cycles(cpu, instruction_cycles[opcode]);
    increment_program_counter(cpu, instruction_sizes[opcode]);
    return execute(cpu, ram, address);
}