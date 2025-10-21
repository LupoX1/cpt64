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

typedef struct
{
    execute_t code;
    address_mode_t mode;
    char name[4];
    uint8_t cycles;
    uint8_t size;
} intruction_t;

intruction_t instruction_set[256] = {
    {.code = brk, .mode = IMP, .name = "brk", .cycles = 7, .size = 1}, // 00
    {.code = ora, .mode = INX, .name = "ora", .cycles = 6, .size = 2}, // 01
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 02
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 03
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 04
    {.code = ora, .mode = ZPG, .name = "ora", .cycles = 3, .size = 2}, // 05
    {.code = asl, .mode = ZPG, .name = "asl", .cycles = 5, .size = 2}, // 06
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 07
    {.code = php, .mode = IMP, .name = "php", .cycles = 3, .size = 1}, // 08
    {.code = ora, .mode = IMM, .name = "ora", .cycles = 2, .size = 2}, // 09
    {.code = asl, .mode = ACC, .name = "asl", .cycles = 2, .size = 1}, // 0A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 0B
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 0C
    {.code = ora, .mode = ABS, .name = "ora", .cycles = 4, .size = 3}, // 0D
    {.code = asl, .mode = ABS, .name = "asl", .cycles = 6, .size = 3}, // 0E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 0F
    {.code = bpl, .mode = REL, .name = "bpl", .cycles = 2, .size = 2}, // 10
    {.code = ora, .mode = INY, .name = "ora", .cycles = 5, .size = 2}, // 11
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 12
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 13
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 14
    {.code = ora, .mode = ZPX, .name = "ora", .cycles = 4, .size = 2}, // 15
    {.code = asl, .mode = ZPX, .name = "asl", .cycles = 6, .size = 2}, // 16
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 17
    {.code = clc, .mode = IMP, .name = "clc", .cycles = 2, .size = 1}, // 18
    {.code = ora, .mode = ABY, .name = "ora", .cycles = 4, .size = 3}, // 19
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 1A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 1B
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 1C
    {.code = ora, .mode = ABX, .name = "ora", .cycles = 4, .size = 3}, // 1D
    {.code = asl, .mode = ABX, .name = "asl", .cycles = 7, .size = 3}, // 1E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 1F
    {.code = jsr, .mode = ABS, .name = "jsr", .cycles = 6, .size = 3}, // 20
    {.code = and, .mode = INX, .name = "and", .cycles = 6, .size = 2}, // 21
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 22
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 23
    {.code = bit, .mode = ZPG, .name = "bit", .cycles = 3, .size = 2}, // 24
    {.code = and, .mode = ZPG, .name = "and", .cycles = 3, .size = 2}, // 25
    {.code = rol, .mode = ZPG, .name = "rol", .cycles = 5, .size = 2}, // 26
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 27
    {.code = plp, .mode = IMP, .name = "plp", .cycles = 4, .size = 1}, // 28
    {.code = and, .mode = IMM, .name = "and", .cycles = 2, .size = 2}, // 29
    {.code = rol, .mode = ACC, .name = "rol", .cycles = 2, .size = 1}, // 2A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 2B
    {.code = bit, .mode = ABS, .name = "bit", .cycles = 4, .size = 3}, // 2C
    {.code = and, .mode = ABS, .name = "and", .cycles = 4, .size = 3}, // 2D
    {.code = rol, .mode = ABS, .name = "rol", .cycles = 6, .size = 3}, // 2E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 2F
    {.code = bmi, .mode = REL, .name = "bmi", .cycles = 2, .size = 2}, // 30
    {.code = and, .mode = INY, .name = "and", .cycles = 5, .size = 2}, // 31
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 32
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 33
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 34
    {.code = and, .mode = ZPX, .name = "and", .cycles = 4, .size = 2}, // 35
    {.code = rol, .mode = ZPX, .name = "rol", .cycles = 6, .size = 2}, // 36
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 37
    {.code = sec, .mode = IMP, .name = "sec", .cycles = 2, .size = 1}, // 38
    {.code = and, .mode = ABY, .name = "and", .cycles = 4, .size = 3}, // 39
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 3A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 3B
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 3C
    {.code = and, .mode = ABX, .name = "and", .cycles = 4, .size = 3}, // 3D
    {.code = rol, .mode = ABX, .name = "rol", .cycles = 7, .size = 3}, // 3E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 3F
    {.code = rti, .mode = IMP, .name = "rti", .cycles = 6, .size = 1}, // 40
    {.code = eor, .mode = INY, .name = "eor", .cycles = 6, .size = 2}, // 41
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 42
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 43
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 44
    {.code = eor, .mode = ZPG, .name = "eor", .cycles = 3, .size = 2}, // 45
    {.code = lsr, .mode = ZPG, .name = "lsr", .cycles = 5, .size = 2}, // 46
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 47
    {.code = pha, .mode = IMP, .name = "pha", .cycles = 3, .size = 1}, // 48
    {.code = eor, .mode = IMM, .name = "eor", .cycles = 2, .size = 2}, // 49
    {.code = lsr, .mode = ACC, .name = "lsr", .cycles = 2, .size = 1}, // 4A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 4B
    {.code = jmp, .mode = ABS, .name = "jmp", .cycles = 3, .size = 3}, // 4C
    {.code = eor, .mode = ABS, .name = "eor", .cycles = 4, .size = 3}, // 4D
    {.code = lsr, .mode = ABS, .name = "lsr", .cycles = 3, .size = 3}, // 4E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 4F
    {.code = bvc, .mode = REL, .name = "bvc", .cycles = 2, .size = 2}, // 50
    {.code = eor, .mode = INY, .name = "eor", .cycles = 5, .size = 2}, // 51
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 52
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 53
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 54
    {.code = eor, .mode = ZPX, .name = "eor", .cycles = 4, .size = 2}, // 55
    {.code = lsr, .mode = ZPX, .name = "lsr", .cycles = 6, .size = 2}, // 56
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 57
    {.code = cli, .mode = IMP, .name = "cli", .cycles = 2, .size = 1}, // 58
    {.code = eor, .mode = ABY, .name = "eor", .cycles = 4, .size = 3}, // 59
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 5A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 5B
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 5C
    {.code = eor, .mode = ABX, .name = "eor", .cycles = 4, .size = 3}, // 5D
    {.code = lsr, .mode = ABX, .name = "lsr", .cycles = 7, .size = 3}, // 5E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 5F
    {.code = rts, .mode = IMP, .name = "rts", .cycles = 6, .size = 1}, // 60
    {.code = adc, .mode = INX, .name = "adc", .cycles = 6, .size = 2}, // 61
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 62
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 63
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 64
    {.code = adc, .mode = ZPG, .name = "adc", .cycles = 3, .size = 2}, // 65
    {.code = ror, .mode = ZPG, .name = "ror", .cycles = 5, .size = 2}, // 66
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 67
    {.code = pla, .mode = IMP, .name = "pla", .cycles = 4, .size = 1}, // 68
    {.code = adc, .mode = IMM, .name = "adc", .cycles = 2, .size = 2}, // 69
    {.code = ror, .mode = ACC, .name = "ror", .cycles = 2, .size = 1}, // 6A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 6B
    {.code = jmp, .mode = IND, .name = "jmp", .cycles = 5, .size = 3}, // 6C
    {.code = adc, .mode = ABS, .name = "adc", .cycles = 4, .size = 3}, // 6D
    {.code = ror, .mode = ABS, .name = "ror", .cycles = 6, .size = 3}, // 6E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 6F
    {.code = bvs, .mode = REL, .name = "bvs", .cycles = 2, .size = 2}, // 70
    {.code = adc, .mode = INY, .name = "adc", .cycles = 5, .size = 2}, // 71
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 72
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 73
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 74
    {.code = adc, .mode = ZPX, .name = "adc", .cycles = 4, .size = 2}, // 75
    {.code = ror, .mode = ZPX, .name = "ror", .cycles = 6, .size = 2}, // 76
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 77
    {.code = sei, .mode = IMP, .name = "sei", .cycles = 2, .size = 1}, // 78
    {.code = adc, .mode = ABY, .name = "adc", .cycles = 4, .size = 3}, // 79
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 7A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 7B
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 7C
    {.code = adc, .mode = ABX, .name = "adc", .cycles = 4, .size = 3}, // 7D
    {.code = ror, .mode = ABX, .name = "ror", .cycles = 7, .size = 3}, // 7E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 7F
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 80
    {.code = sta, .mode = INX, .name = "sta", .cycles = 6, .size = 2}, // 81
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 82
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 83
    {.code = sty, .mode = ZPG, .name = "sty", .cycles = 3, .size = 2}, // 84
    {.code = sta, .mode = ZPG, .name = "sta", .cycles = 3, .size = 2}, // 85
    {.code = stx, .mode = ZPG, .name = "stx", .cycles = 3, .size = 2}, // 86
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 87
    {.code = dey, .mode = IMP, .name = "dey", .cycles = 2, .size = 1}, // 88
    {.code = sta, .mode = N_D, .name = "sta", .cycles = 0, .size = 0}, // 89
    {.code = txa, .mode = IMP, .name = "txa", .cycles = 2, .size = 1}, // 8A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 8B
    {.code = sty, .mode = ABS, .name = "sty", .cycles = 4, .size = 3}, // 8C
    {.code = sta, .mode = ABS, .name = "sta", .cycles = 4, .size = 3}, // 8D
    {.code = stx, .mode = ABS, .name = "stx", .cycles = 4, .size = 3}, // 8E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 8F
    {.code = bcc, .mode = REL, .name = "bcc", .cycles = 2, .size = 2}, // 90
    {.code = sta, .mode = INY, .name = "sta", .cycles = 6, .size = 2}, // 91
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 92
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 93
    {.code = sty, .mode = ZPX, .name = "sty", .cycles = 4, .size = 2}, // 94
    {.code = sta, .mode = ZPX, .name = "sta", .cycles = 4, .size = 2}, // 95
    {.code = stx, .mode = ZPY, .name = "stx", .cycles = 4, .size = 2}, // 96
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 97
    {.code = tya, .mode = IMP, .name = "tya", .cycles = 2, .size = 1}, // 98
    {.code = sta, .mode = ABY, .name = "sta", .cycles = 5, .size = 3}, // 99
    {.code = txs, .mode = IMP, .name = "txs", .cycles = 2, .size = 1}, // 9A
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 9B
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 9C
    {.code = sta, .mode = ABX, .name = "sta", .cycles = 5, .size = 3}, // 9D
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 9E
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // 9F
    {.code = ldy, .mode = IMM, .name = "ldy", .cycles = 2, .size = 2}, // A0
    {.code = lda, .mode = INX, .name = "lda", .cycles = 6, .size = 2}, // A1
    {.code = ldx, .mode = IMM, .name = "ldx", .cycles = 2, .size = 2}, // A2
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // A3
    {.code = ldy, .mode = ZPG, .name = "ldy", .cycles = 3, .size = 2}, // A4
    {.code = lda, .mode = ZPG, .name = "lda", .cycles = 3, .size = 2}, // A5
    {.code = ldx, .mode = ZPG, .name = "ldx", .cycles = 3, .size = 2}, // A6
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // A7
    {.code = tay, .mode = IMP, .name = "tay", .cycles = 2, .size = 1}, // A8
    {.code = lda, .mode = IMM, .name = "lda", .cycles = 2, .size = 2}, // A9
    {.code = tax, .mode = IMP, .name = "tax", .cycles = 2, .size = 1}, // AA
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // AB
    {.code = ldy, .mode = ABS, .name = "ldy", .cycles = 4, .size = 3}, // AC
    {.code = lda, .mode = ABS, .name = "lda", .cycles = 4, .size = 3}, // AD
    {.code = ldx, .mode = ABS, .name = "ldx", .cycles = 4, .size = 3}, // AE
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // AF
    {.code = bsc, .mode = REL, .name = "bsc", .cycles = 2, .size = 2}, // B0
    {.code = lda, .mode = INY, .name = "lda", .cycles = 5, .size = 2}, // B1
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // B2
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // B3
    {.code = ldy, .mode = ZPX, .name = "ldy", .cycles = 4, .size = 2}, // B4
    {.code = lda, .mode = ZPX, .name = "lda", .cycles = 4, .size = 2}, // B5
    {.code = ldx, .mode = ZPY, .name = "ldx", .cycles = 4, .size = 2}, // B6
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // B7
    {.code = clv, .mode = IMP, .name = "clv", .cycles = 2, .size = 1}, // B8
    {.code = lda, .mode = ABY, .name = "lda", .cycles = 4, .size = 3}, // B9
    {.code = tsx, .mode = IMP, .name = "tsx", .cycles = 2, .size = 1}, // BA
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // BB
    {.code = ldy, .mode = ABX, .name = "ldy", .cycles = 4, .size = 3}, // BC
    {.code = lda, .mode = ABX, .name = "lda", .cycles = 4, .size = 3}, // BD
    {.code = ldx, .mode = ABY, .name = "ldx", .cycles = 4, .size = 3}, // BE
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // BF
    {.code = cpy, .mode = IMM, .name = "cpy", .cycles = 2, .size = 2}, // C0
    {.code = cmp, .mode = INX, .name = "cmp", .cycles = 6, .size = 2}, // C1
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // C2
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // C3
    {.code = cpy, .mode = ZPG, .name = "cpy", .cycles = 3, .size = 2}, // C4
    {.code = cmp, .mode = ZPG, .name = "cmp", .cycles = 3, .size = 2}, // C5
    {.code = dec, .mode = ZPG, .name = "dec", .cycles = 5, .size = 2}, // C6
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // C7
    {.code = iny, .mode = IMP, .name = "iny", .cycles = 2, .size = 1}, // C8
    {.code = cmp, .mode = IMM, .name = "cmp", .cycles = 2, .size = 2}, // C9
    {.code = dex, .mode = IMP, .name = "dex", .cycles = 2, .size = 1}, // CA
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // CB
    {.code = cpy, .mode = ABS, .name = "cpy", .cycles = 4, .size = 3}, // CC
    {.code = cmp, .mode = ABS, .name = "cmp", .cycles = 4, .size = 3}, // CD
    {.code = dec, .mode = ABS, .name = "dec", .cycles = 6, .size = 3}, // CE
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // CF
    {.code = bne, .mode = REL, .name = "bne", .cycles = 2, .size = 2}, // D0
    {.code = cmp, .mode = INY, .name = "cmp", .cycles = 5, .size = 2}, // D1
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // D2
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // D3
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // D4
    {.code = cmp, .mode = ZPX, .name = "cmp", .cycles = 4, .size = 2}, // D5
    {.code = dec, .mode = ZPX, .name = "dec", .cycles = 6, .size = 2}, // D6
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // D7
    {.code = cld, .mode = IMP, .name = "cld", .cycles = 2, .size = 1}, // D8
    {.code = cmp, .mode = ABY, .name = "cmp", .cycles = 4, .size = 3}, // D9
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // DA
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // DB
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // DC
    {.code = cmp, .mode = ABX, .name = "cmp", .cycles = 4, .size = 3}, // DD
    {.code = dec, .mode = ABX, .name = "dec", .cycles = 7, .size = 3}, // DE
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // DF
    {.code = cpx, .mode = IMM, .name = "cpx", .cycles = 2, .size = 2}, // E0
    {.code = sbc, .mode = INX, .name = "sbc", .cycles = 6, .size = 2}, // E1
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // E2
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // E3
    {.code = cpx, .mode = ZPG, .name = "cpx", .cycles = 3, .size = 2}, // E4
    {.code = sbc, .mode = ZPG, .name = "sbc", .cycles = 3, .size = 2}, // E5
    {.code = inc, .mode = ZPG, .name = "inc", .cycles = 5, .size = 2}, // E6
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // E7
    {.code = inx, .mode = IMP, .name = "inx", .cycles = 2, .size = 1}, // E8
    {.code = sbc, .mode = IMM, .name = "sbc", .cycles = 2, .size = 2}, // E9
    {.code = nop, .mode = IMP, .name = "nop", .cycles = 2, .size = 1}, // EA
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // EB
    {.code = cpx, .mode = ABS, .name = "cpx", .cycles = 4, .size = 3}, // EC
    {.code = sbc, .mode = ABS, .name = "sbc", .cycles = 4, .size = 3}, // ED
    {.code = inc, .mode = ABS, .name = "inc", .cycles = 6, .size = 3}, // EE
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // EF
    {.code = beq, .mode = REL, .name = "beq", .cycles = 2, .size = 2}, // F0
    {.code = sbc, .mode = INY, .name = "sbc", .cycles = 5, .size = 2}, // F1
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // F2
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // F3
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // F4
    {.code = sbc, .mode = ZPX, .name = "sbc", .cycles = 4, .size = 2}, // F5
    {.code = inc, .mode = ZPX, .name = "inc", .cycles = 6, .size = 2}, // F6
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // F7
    {.code = sed, .mode = IMP, .name = "sed", .cycles = 2, .size = 1}, // F8
    {.code = sbc, .mode = ABY, .name = "sbc", .cycles = 4, .size = 3}, // F9
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // FA
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // FB
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // FC
    {.code = sbc, .mode = ABX, .name = "sbc", .cycles = 4, .size = 3}, // FD
    {.code = inc, .mode = ABX, .name = "inc", .cycles = 7, .size = 3}, // FE
    {.code = bad, .mode = N_D, .name = "bad", .cycles = 0, .size = 0}, // FF
};

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
    uint16_t address = 0x0100 | cpu->sp;
    ram[address] = value;
    cpu->sp = (cpu->sp - 1) & 0xFF;
}

uint8_t pop(cpu_6510_t *cpu, memory_t ram)
{
    cpu->sp = (cpu->sp + 1) & 0xFF;
    uint16_t addr = 0x0100 | cpu->sp;
    return ram[addr];
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
    return instruction_set[opcode].code;
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
    return (uint8_t)ram[cpu->pc + 1];
}

uint16_t decode_address_zeropage_x(cpu_6510_t *cpu, memory_t ram)
{
    return (uint8_t)(ram[cpu->pc + 1] + cpu->x);
}

uint16_t decode_address_zeropage_y(cpu_6510_t *cpu, memory_t ram)
{
    return (uint8_t)(ram[cpu->pc + 1] + cpu->y);
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
    
    uint8_t target_low = ram[address];
    uint8_t target_high = ram[(address & 0xFF00) | ((address + 1) & 0x00FF)];
    
    return (target_high << 8) | target_low;
}

uint16_t decode_address_indirect_x(cpu_6510_t *cpu, memory_t ram)
{
    uint8_t zp_base = ram[cpu->pc + 1];
    uint8_t zp_addr = zp_base + cpu->x;
    uint8_t low_addr = ram[zp_addr];
    uint8_t high_addr = ram[(uint8_t)(zp_addr + 1)];
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

    //printf("\033[1;1H");
    printf("AC:   %02X XR: %02X YR: %02X NV-BDIZC Instruction Cycles\n", cpu->a, cpu->x, cpu->y);
    printf("PC: %04X SP: %02X SR: %02X %s %s         %lu\n", cpu->pc, cpu->sp, cpu->sr, flags, instruction_set[ram[cpu->pc]].name, cpu->cycles);
    printf("%04X %04X %04X %04X\n", cpu->pc, cpu->pc + 1, cpu->pc + 2, cpu->pc + 3);
    printf("  %02X   %02X   %02X   %02X\n", ram[cpu->pc], ram[cpu->pc + 1], ram[cpu->pc + 2], ram[cpu->pc + 3]);
}

bool handle_reset(cpu_6510_t *cpu, memory_t ram)
{
    //TODO complete reset routine
    cpu->reset = false;

    uint8_t sr = read_sr(cpu);
    sr = sr & ~FLAG_B;
    sr = sr | FLAG_U;

    uint16_t pc = read_program_counter(cpu);
    push(cpu, ram, (uint8_t)(pc >> 8));
    push(cpu, ram, (uint8_t)(pc & 0xFF));
    push(cpu, ram, sr);

    set_interrupt_flag(cpu, true);
    
    uint16_t vector = (ram[0xFFFD] << 8) | ram[0xFFFC];
    write_program_counter(cpu, vector);

    return true;
}

bool handle_nmi(cpu_6510_t *cpu, memory_t ram)
{
    cpu->nmi = false;

    uint8_t sr = read_sr(cpu);
    sr = sr & ~FLAG_B;
    sr = sr | FLAG_U;

    uint16_t pc = read_program_counter(cpu);
    push(cpu, ram, (uint8_t)(pc >> 8));
    push(cpu, ram, (uint8_t)(pc & 0xFF));
    push(cpu, ram, sr);

    set_interrupt_flag(cpu, true);
    
    uint16_t vector = (ram[0xFFFB] << 8) | ram[0xFFFA];
    write_program_counter(cpu, vector);

    increment_cycles(cpu, 7);

    return true;
}

bool handle_irq(cpu_6510_t *cpu, memory_t ram)
{
    cpu->irq = false;

    uint8_t sr = read_sr(cpu);
    sr = sr & ~FLAG_B;
    sr = sr | FLAG_U;

    uint16_t pc = read_program_counter(cpu);
    push(cpu, ram, (uint8_t)(pc >> 8));
    push(cpu, ram, (uint8_t)(pc & 0xFF));
    push(cpu, ram, sr);

    set_interrupt_flag(cpu, true);
    
    uint16_t vector = (ram[0xFFFF] << 8) | ram[0xFFFE];
    write_program_counter(cpu, vector);

    increment_cycles(cpu, 7);

    return true;
}

bool cpu_step(cpu_6510_t *cpu, memory_t ram)
{
    if(cpu->reset) return handle_reset(cpu, ram);
    if(cpu->nmi) return handle_nmi(cpu, ram);
    if(cpu->irq && !get_interrupt_flag(cpu)) return handle_irq(cpu, ram);

    uint8_t opcode = ram[cpu->pc];
    execute_t execute = instruction_set[opcode].code;
    address_mode_t decode_address = instruction_set[opcode].mode;
    uint16_t address = decode_address(cpu, ram);
    increment_cycles(cpu, instruction_set[opcode].cycles);
    increment_program_counter(cpu, instruction_set[opcode].cycles);
    return execute(cpu, ram, address);
}

void reset_request(cpu_6510_t *cpu)
{
    cpu->reset = true;
}

void interrupt_request_non_maskable(cpu_6510_t *cpu)
{
    cpu->nmi = true;
}

void interrupt_request(cpu_6510_t *cpu)
{
    cpu->irq = true;
}