#include "cpu/opcodes.h"

bool adc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    bool decimal_mode = get_decimal_flag(cpu);
    bool carry = get_carry_flag(cpu);
    uint8_t acc = read_accumulator(cpu);

    if (decimal_mode)
    {
       // BCD addition
        int al = (acc & 0x0F) + (value & 0x0F) + (carry ? 1 : 0);
        int ah = (acc >> 4) + (value >> 4);
        
        if (al > 9) {
            al -= 10;
            ah++;
        }
        
        // Note: Z and N flags are based on binary result
        uint16_t binary_result = acc + value + (carry ? 1 : 0);
        set_zero_flag(cpu, (binary_result & 0xFF) == 0);
        set_negative_flag(cpu, (binary_result & 0x80) != 0);
        
        bool overflow = ((acc ^ binary_result) & (value ^ binary_result) & 0x80) != 0;
        set_overflow_flag(cpu, overflow);
        
        if (ah > 9) {
            ah -= 10;
            set_carry_flag(cpu, true);
        } else {
            set_carry_flag(cpu, false);
        }
        
        uint8_t result = ((ah & 0x0F) << 4) | (al & 0x0F);
        write_accumulator(cpu, result);
    }
    else
    {
        uint16_t result = acc + value + (carry ? 1 : 0);
        write_accumulator(cpu, (uint8_t)(result & 0x00FF));
        set_overflow_flag(cpu, (~(acc ^ value) & (acc ^ result) & FLAG_N) != 0);
        set_zero_flag(cpu, (result & 0xFF) == 0);
        set_carry_flag(cpu, result > 0x00FF);
        set_negative_flag(cpu, (result & FLAG_N) != 0);
    }

    return true;
}

bool and(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t result = read_accumulator(cpu) & value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & FLAG_N) != 0);
    return true;
}

bool asl(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = address ? read_direct(mem, address) : read_accumulator(cpu);
    set_carry_flag(cpu, (value & FLAG_N) != 0);
    uint8_t result = value << 1;

    if (address)
        write_direct(mem, address, result);
    else
        write_accumulator(cpu, result);

    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & FLAG_N) != 0);

    return true;
}

bool bad(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t opcode = read_direct(mem, address);
    printf("BAD %02X @ %04X\n", opcode, address);
    return false;
}

bool bcc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool carry = get_carry_flag(cpu);
    if (!carry)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool bsc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool carry = get_carry_flag(cpu);
    if (carry)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool beq(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool zero = get_zero_flag(cpu);
    if (zero)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool bit(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t result = read_accumulator(cpu) & value;
    
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (value & FLAG_N) != 0);
    set_overflow_flag(cpu, (value & FLAG_V) != 0);
    return true;
}

bool bmi(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool negative = get_negative_flag(cpu);
    if (negative)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool bne(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool zero = get_zero_flag(cpu);
    if (!zero)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool bpl(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool negative = get_negative_flag(cpu);
    if (!negative)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool brk(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t sr = read_sr(cpu);
    sr = sr | FLAG_B;
    sr = sr | FLAG_U;

    uint16_t pc = read_program_counter(cpu) + 1;
    push(cpu, mem, (uint8_t)(pc >> 8));
    push(cpu, mem, (uint8_t)(pc & 0xFF));
    push(cpu, mem, sr);

    set_interrupt_flag(cpu, true);
    set_break_flag(cpu, true);

    uint16_t vector = (read_direct(mem, 0xFFFF) << 8) | read_direct(mem, 0xFFFE);
    write_program_counter(cpu, vector);

    return true;
}

bool bvc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool overflow = get_overflow_flag(cpu);
    if (!overflow)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool bvs(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    bool overflow = get_overflow_flag(cpu);
    if (overflow)
    {
        increment_cycles(cpu, 1);
        write_program_counter(cpu, address);
    }
    return true;
}

bool clc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_carry_flag(cpu, false);
    return true;
}

bool cld(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_decimal_flag(cpu, false);
    return true;
}

bool cli(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_interrupt_flag(cpu, false);
    return true;
}

bool clv(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_overflow_flag(cpu, false);
    return true;
}

bool cmp(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t acc = read_accumulator(cpu);
    uint8_t result = acc - value;

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);
    set_carry_flag(cpu, acc >= value);

    return true;
}

bool cpx(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t xr = read_xr(cpu);
    uint8_t result = xr - value;

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);
    set_carry_flag(cpu, xr >= value);

    return true;
}

bool cpy(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t yr = read_yr(cpu);
    uint8_t result = yr - value;

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);
    set_carry_flag(cpu, yr >= value);

    return true;
}

bool dec(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t result = value - 1;

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);

    write_direct(mem, address, result);

    return true;
}

bool dex(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t result = read_xr(cpu) - 1;
    write_xr(cpu, result);

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);

    return true;
}

bool dey(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t result = read_yr(cpu) - 1;
    write_yr(cpu, result);

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);

    return true;
}

bool eor(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t result = read_accumulator(cpu) ^ value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & FLAG_N) != 0);
    return true;
}

bool inc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t result = value + 1;

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);
    write_direct(mem, address, result);
    return true;
}

bool inx(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t result = read_xr(cpu) + 1;
    write_xr(cpu, result);

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);
    return true;
}

bool iny(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t result = read_yr(cpu) + 1;
    write_yr(cpu, result);

    set_negative_flag(cpu, (result & FLAG_N) != 0);
    set_zero_flag(cpu, result == 0);
    return true;
}

bool jmp(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    write_program_counter(cpu, address);
    return true;
}

bool jsr(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint16_t pc = read_program_counter(cpu);
    push(cpu, mem, (uint8_t)((pc - 1) >> 8));
    push(cpu, mem, (uint8_t)((pc - 1) & 0xFF));
    write_program_counter(cpu, address);
    return true;
}

bool lda(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    write_accumulator(cpu, value);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, (value & FLAG_N) != 0);
    return true;
}

bool ldx(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    write_xr(cpu, value);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, (value & FLAG_N) != 0);
    return true;
}

bool ldy(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    write_yr(cpu, value);
    set_zero_flag(cpu, value == 0);
    set_negative_flag(cpu, (value & FLAG_N) != 0);
    return true;
}

bool lsr(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = address ? read_direct(mem, address) : read_accumulator(cpu);
    uint8_t result = value >> 1;
    if (address)
        write_direct(mem, address, result);
    else
        write_accumulator(cpu, result);
    set_carry_flag(cpu, (value & FLAG_C) != 0);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, false);
    return true;
}

bool nop(cpu_t *cpu, memory_t *mem, uint16_t address) {
    return true;
}

bool ora(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    uint8_t result = read_accumulator(cpu) | value;
    write_accumulator(cpu, result);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & FLAG_N) != 0);
    return true;
}

bool pha(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t acc = read_accumulator(cpu);
    push(cpu, mem, acc);
    return true;
}

bool php(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t status = read_sr(cpu);
    status = status | FLAG_B;
    status = status | FLAG_U;
    push(cpu, mem, status);
    return true;
}

bool pla(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t acc = pop(cpu, mem);
    write_accumulator(cpu, acc);
    set_zero_flag(cpu, acc == 0);
    set_negative_flag(cpu, (acc & FLAG_N) != 0);
    return true;
}

bool plp(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t status = pop(cpu, mem);
    status = status & ~FLAG_B;
    status = status | FLAG_U;
    write_sr(cpu, status);
    return true;
}

bool rol(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = address ? read_direct(mem, address) : read_accumulator(cpu);
    bool new_carry = (value & FLAG_N) != 0;
    bool old_carry = get_carry_flag(cpu);
    uint8_t result = value << 1;
    if (old_carry)
    {
        result = result | 0x01;
    }
    if (address)
        write_direct(mem, address, result);
    else
        write_accumulator(cpu, result);
    set_carry_flag(cpu, new_carry);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & FLAG_N) != 0);
    return true;
}

bool ror(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = address ? read_direct(mem, address) : read_accumulator(cpu);
    bool new_carry = (value & 0x01) != 0;
    bool old_carry = get_carry_flag(cpu);
    uint8_t result = value >> 1;
    if (old_carry)
    {
        result = result | FLAG_N;
    }
    if (address)
        write_direct(mem, address, result);
    else
        write_accumulator(cpu, result);
    set_carry_flag(cpu, new_carry);
    set_zero_flag(cpu, result == 0);
    set_negative_flag(cpu, (result & FLAG_N) != 0);
    return true;
}

bool rti(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t status = pop(cpu, mem);
    status = status & ~FLAG_B;
    status = status | FLAG_U;

    uint8_t low = pop(cpu, mem);
    uint8_t high = pop(cpu, mem);
    uint16_t pc = high << 8 | low;
 
    write_program_counter(cpu, pc);
    write_sr(cpu, status);

    return true;
}

bool rts(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t low = pop(cpu, mem);
    uint8_t high = pop(cpu, mem);
    uint16_t pc = high << 8 | low;
    write_program_counter(cpu, pc + 1);
    return true;
}

bool sbc(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t value = read_direct(mem, address);
    bool decimal_mode = get_decimal_flag(cpu);
    bool carry = get_carry_flag(cpu);
    uint8_t acc = read_accumulator(cpu);
    
    if (decimal_mode)
    {
        // BCD subtraction
        int al = (acc & 0x0F) - (value & 0x0F) - (carry ? 0 : 1);
        int ah = (acc >> 4) - (value >> 4);
        
        if (al < 0) {
            al += 10;
            ah--;
        }
        
        // Note: Z and N flags are based on binary result
        int16_t binary_result = acc - value - (carry ? 0 : 1);
        set_zero_flag(cpu, (binary_result & 0xFF) == 0);
        set_negative_flag(cpu, (binary_result & 0x80) != 0);
        
        bool overflow = ((acc ^ value) & (acc ^ binary_result) & 0x80) != 0;
        set_overflow_flag(cpu, overflow);
        
        if (ah < 0) {
            ah += 10;
            set_carry_flag(cpu, false);  // Borrow occurred
        } else {
            set_carry_flag(cpu, true);   // No borrow
        }
        
        uint8_t result = ((ah & 0x0F) << 4) | (al & 0x0F);
        write_accumulator(cpu, result);
    }
    else
    {
        int16_t result = acc - value - (carry ? 0 : 1);
        write_accumulator(cpu, (uint8_t)(result & 0xFF));
        set_overflow_flag(cpu, ((acc ^ value) & (acc ^ result) & FLAG_N) != 0);
        set_zero_flag(cpu, (result & 0xFF) == 0);
        set_carry_flag(cpu, result >= 0);
        set_negative_flag(cpu, (result & FLAG_N) != 0);
    }
    return true;
}

bool sec(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_carry_flag(cpu, true);
    return true;
}

bool sed(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_decimal_flag(cpu, true);
    return true;
}

bool sei(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    set_interrupt_flag(cpu, true);
    return true;
}

bool sta(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t acc = read_accumulator(cpu);
    write_direct(mem, address, acc);
    return true;
}

bool stx(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t xr = read_xr(cpu);
    write_direct(mem, address, xr);
    return true;
}

bool sty(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t yr = read_yr(cpu);
    write_direct(mem, address, yr);
    return true;
}

bool tax(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t acc = read_accumulator(cpu);
    write_xr(cpu, acc);
    set_zero_flag(cpu, acc == 0);
    set_negative_flag(cpu, (acc & FLAG_N) != 0);
    return true;
}

bool tay(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t acc = read_accumulator(cpu);
    write_yr(cpu, acc);
    set_zero_flag(cpu, acc == 0);
    set_negative_flag(cpu, (acc & FLAG_N) != 0);
    return true;
}

bool tsx(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t sp = read_sp(cpu);
    write_xr(cpu, sp);
    set_zero_flag(cpu, sp == 0);
    set_negative_flag(cpu, (sp & FLAG_N) != 0);
    return true;
}

bool txa(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t xr = read_xr(cpu);
    write_accumulator(cpu, xr);
    set_zero_flag(cpu, xr == 0);
    set_negative_flag(cpu, (xr & FLAG_N) != 0);
    return true;
}

bool txs(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t xr = read_xr(cpu);
    write_sp(cpu, xr);
    return true;
}

bool tya(cpu_t *cpu, memory_t *mem, uint16_t address)
{
    uint8_t yr = read_yr(cpu);
    write_accumulator(cpu, yr);
    set_zero_flag(cpu, yr == 0);
    set_negative_flag(cpu, (yr & FLAG_N) != 0);
    return true;
}