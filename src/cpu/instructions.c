#include "core/bus.h"
#include "cpu/cpu.h"
#include "cpu/opcodes.h"
#include "cpu/addressing.h"
#include "cpu/instructions.h"

typedef struct
{
    opcode_t code;
    addr_mode_t mode;
    char name[4];
    uint8_t cycles;
    uint8_t size;
} instruction_t;

instruction_t instruction_set[256] = {
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
    {.code = lsr, .mode = ABS, .name = "lsr", .cycles = 6, .size = 3}, // 4E
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
    {.code = bcs, .mode = REL, .name = "bcs", .cycles = 2, .size = 2}, // B0
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

addr_mode_t get_addressing_mode(uint8_t opcode)
{
    return instruction_set[opcode].mode;
}

opcode_t get_opcode_handler(uint8_t opcode)
{
    return instruction_set[opcode].code;
}

uint8_t get_instruction_size(uint8_t opcode)
{
    return instruction_set[opcode].size;
}

uint8_t get_cycles_count(uint8_t opcode)
{
    return instruction_set[opcode].cycles;
}

char* get_instruction_name(uint8_t opcode)
{
    return instruction_set[opcode].name;
}