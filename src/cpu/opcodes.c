#include "core/bus.h"
#include "cpu/opcodes.h"
#include "cpu/cpu.h"
#include "memory/memory.h"

bool adc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    bool decimal_mode = cpu_get_flag(cpu, FLAG_D);
    bool carry = cpu_get_flag(cpu, FLAG_C);
    uint8_t acc = cpu_get_a(cpu);

    if (decimal_mode)
    {
        // BCD addition
        int al = (acc & 0x0F) + (value & 0x0F) + (carry ? 1 : 0);
        int ah = (acc >> 4) + (value >> 4);

        if (al > 9)
        {
            al -= 10;
            ah++;
        }

        // Note: Z and N flags are based on binary result
        uint16_t binary_result = acc + value + (carry ? 1 : 0);
        cpu_set_flag(cpu, FLAG_Z, (binary_result & 0xFF) == 0);
        cpu_set_flag(cpu, FLAG_N, (binary_result & 0x80) != 0);

        bool overflow = ((acc ^ binary_result) & (value ^ binary_result) & 0x80) != 0;
        cpu_set_flag(cpu, FLAG_V, overflow);

        if (ah > 9)
        {
            ah -= 10;
            cpu_set_flag(cpu, FLAG_C, true);
        }
        else
        {
            cpu_set_flag(cpu, FLAG_C, false);
        }

        uint8_t result = ((ah & 0x0F) << 4) | (al & 0x0F);
        cpu_set_a(cpu, result);
    }
    else
    {
        uint16_t result = acc + value + (carry ? 1 : 0);
        cpu_set_a(cpu, (uint8_t)(result & 0x00FF));
        cpu_set_flag(cpu, FLAG_V, (~(acc ^ value) & (acc ^ result) & FLAG_N) != 0);
        cpu_set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
        cpu_set_flag(cpu, FLAG_C, result > 0x00FF);
        cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    }

    return true;
}

bool and(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t result = cpu_get_a(cpu) & value;
    cpu_set_a(cpu, result);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    return true;
}

bool asl(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = address ? bus_read(bus, address) : cpu_get_a(cpu);
    cpu_set_flag(cpu, FLAG_C, (value & FLAG_N) != 0);
    uint8_t result = value << 1;

    if (address)
        bus_write(bus, address, result);
    else
        cpu_set_a(cpu, result);

    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);

    return true;
}

bool bad(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t opcode = bus_read(bus, address);
    printf("BAD %02X @ %04X\n", opcode, address);
    return false;
}

bool bcc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool carry = cpu_get_flag(cpu, FLAG_C);
    if (!carry)
    {
        cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool bsc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool carry = cpu_get_flag(cpu, FLAG_C);
    if (carry)
    {
        cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool beq(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool zero = cpu_get_flag(cpu, FLAG_Z);
    if (zero)
    {
        cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool bit(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t result = cpu_get_a(cpu) & value;

    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (value & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_V, (value & FLAG_V) != 0);
    return true;
}

bool bmi(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool negative = cpu_get_flag(cpu, FLAG_N);
    if (negative)
    {
        cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool bne(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool zero = cpu_get_flag(cpu, FLAG_Z);
    if (!zero)
    {
        cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool bpl(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool negative = cpu_get_flag(cpu, FLAG_N);
    if (!negative)
    {
        cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool brk(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t sr = cpu_get_sr(cpu);
    sr = sr | FLAG_B;
    sr = sr | FLAG_U;

    uint16_t pc = cpu_get_pc(cpu) + 1;
    cpu_push(cpu, bus, (uint8_t)(pc >> 8));
    cpu_push(cpu, bus, (uint8_t)(pc & 0xFF));
    cpu_push(cpu, bus, sr);

    cpu_set_flag(cpu, FLAG_I, true);
    cpu_set_flag(cpu, FLAG_B, true);

    uint16_t vector = (bus_read(bus, 0xFFFF) << 8) | bus_read(bus, 0xFFFE);
    cpu_set_pc(cpu, vector);

    return true;
}

bool bvc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool overflow = cpu_get_flag(cpu, FLAG_V);
    if (!overflow)
    {
cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool bvs(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    bool overflow = cpu_get_flag(cpu, FLAG_V);
    if (overflow)
    {
cpu_inc_cycles(cpu, 1);
        cpu_set_pc(cpu, address);
    }
    return true;
}

bool clc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_C,false);
    return true;
}

bool cld(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_D,false);
    return true;
}

bool cli(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_I,false);
    return true;
}

bool clv(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_V,false);
    return true;
}

bool cmp(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t acc = cpu_get_a(cpu);
    uint8_t result = acc - value;

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_C, acc >= value);

    return true;
}

bool cpx(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t xr = cpu_get_x(cpu);
    uint8_t result = xr - value;

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_C, xr >= value);

    return true;
}

bool cpy(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t yr = cpu_get_y(cpu);
    uint8_t result = yr - value;

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_C, yr >= value);

    return true;
}

bool dec(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t result = value - 1;

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);

    bus_write(bus, address, result);

    return true;
}

bool dex(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t result = cpu_get_x(cpu) - 1;
    cpu_set_x(cpu, result);

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);

    return true;
}

bool dey(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t result = cpu_get_y(cpu) - 1;
    cpu_set_y(cpu, result);

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);

    return true;
}

bool eor(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t result = cpu_get_a(cpu) ^ value;
    cpu_set_a(cpu, result);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    return true;
}

bool inc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t result = value + 1;

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    bus_write(bus, address, result);
    return true;
}

bool inx(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t result = cpu_get_x(cpu) + 1;
    cpu_set_x(cpu, result);

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    return true;
}

bool iny(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t result = cpu_get_y(cpu) + 1;
    cpu_set_y(cpu, result);

    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    return true;
}

bool jmp(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_pc(cpu, address);
    return true;
}

bool jsr(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint16_t pc = cpu_get_pc(cpu);
    cpu_push(cpu, bus, (uint8_t)((pc - 1) >> 8));
    cpu_push(cpu, bus, (uint8_t)((pc - 1) & 0xFF));
    cpu_set_pc(cpu, address);
    return true;
}

bool lda(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    cpu_set_a(cpu, value);
    cpu_set_flag(cpu, FLAG_Z, value == 0);
    cpu_set_flag(cpu, FLAG_N, (value & FLAG_N) != 0);
    return true;
}

bool ldx(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    cpu_set_x(cpu, value);
    cpu_set_flag(cpu, FLAG_Z, value == 0);
    cpu_set_flag(cpu, FLAG_N, (value & FLAG_N) != 0);
    return true;
}

bool ldy(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    cpu_set_y(cpu, value);
    cpu_set_flag(cpu, FLAG_Z, value == 0);
    cpu_set_flag(cpu, FLAG_N, (value & FLAG_N) != 0);
    return true;
}

bool lsr(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = address ? bus_read(bus, address) : cpu_get_a(cpu);
    uint8_t result = value >> 1;
    if (address)
        bus_write(bus, address, result);
    else
        cpu_set_a(cpu, result);
    cpu_set_flag(cpu, FLAG_C, (value & FLAG_C) != 0);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, false);
    return true;
}

bool nop(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    return true;
}

bool ora(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    uint8_t result = cpu_get_a(cpu) | value;
    cpu_set_a(cpu, result);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    return true;
}

bool pha(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t acc = cpu_get_a(cpu);
    cpu_push(cpu, bus, acc);
    return true;
}

bool php(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t status = cpu_get_sr(cpu);
    status = status | FLAG_B;
    status = status | FLAG_U;
    cpu_push(cpu, bus, status);
    return true;
}

bool pla(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t acc = cpu_pop(cpu, bus);
    cpu_set_a(cpu, acc);
    cpu_set_flag(cpu, FLAG_Z, acc == 0);
    cpu_set_flag(cpu, FLAG_N, (acc & FLAG_N) != 0);
    return true;
}

bool plp(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t status = cpu_pop(cpu, bus);
    status = status & ~FLAG_B;
    status = status | FLAG_U;
    cpu_set_sr(cpu, status);
    return true;
}

bool rol(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = address ? bus_read(bus, address) : cpu_get_a(cpu);
    bool new_carry = (value & FLAG_N) != 0;
    bool old_carry = cpu_get_flag(cpu, FLAG_C);
    uint8_t result = value << 1;
    if (old_carry)
    {
        result = result | 0x01;
    }
    if (address)
        bus_write(bus, address, result);
    else
        cpu_set_a(cpu, result);
    cpu_set_flag(cpu, FLAG_C, new_carry);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    return true;
}

bool ror(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = address ? bus_read(bus, address) : cpu_get_a(cpu);
    bool new_carry = (value & 0x01) != 0;
    bool old_carry = cpu_get_flag(cpu, FLAG_C);
    uint8_t result = value >> 1;
    if (old_carry)
    {
        result = result | FLAG_N;
    }
    if (address)
        bus_write(bus, address, result);
    else
        cpu_set_a(cpu, result);
    cpu_set_flag(cpu, FLAG_C, new_carry);
    cpu_set_flag(cpu, FLAG_Z, result == 0);
    cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    return true;
}

bool rti(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t status = cpu_pop(cpu, bus);
    status = status & ~FLAG_B;
    status = status | FLAG_U;

    uint8_t low = cpu_pop(cpu, bus);
    uint8_t high = cpu_pop(cpu, bus);
    uint16_t pc = high << 8 | low;

    cpu_set_pc(cpu, pc);
    cpu_set_sr(cpu, status);

    return true;
}

bool rts(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t low = cpu_pop(cpu, bus);
    uint8_t high = cpu_pop(cpu, bus);
    uint16_t pc = high << 8 | low;
    cpu_set_pc(cpu, pc + 1);
    return true;
}

bool sbc(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t value = bus_read(bus, address);
    bool decimal_mode = cpu_get_flag(cpu, FLAG_D);
    bool carry = cpu_get_flag(cpu, FLAG_C);
    uint8_t acc = cpu_get_a(cpu);

    if (decimal_mode)
    {
        // BCD subtraction
        int al = (acc & 0x0F) - (value & 0x0F) - (carry ? 0 : 1);
        int ah = (acc >> 4) - (value >> 4);

        if (al < 0)
        {
            al += 10;
            ah--;
        }

        // Note: Z and N flags are based on binary result
        int16_t binary_result = acc - value - (carry ? 0 : 1);
        cpu_set_flag(cpu, FLAG_Z, (binary_result & 0xFF) == 0);
        cpu_set_flag(cpu, FLAG_N, (binary_result & 0x80) != 0);

        bool overflow = ((acc ^ value) & (acc ^ binary_result) & 0x80) != 0;
        cpu_set_flag(cpu, FLAG_V, overflow);
        
        if (ah < 0) {
            ah += 10;
            cpu_set_flag(cpu, FLAG_C,false);  // Borrow occurred
        } else {
            cpu_set_flag(cpu, FLAG_C, true);   // No borrow
        }
        
        uint8_t result = ((ah & 0x0F) << 4) | (al & 0x0F);
        cpu_set_a(cpu, result);
    }
    else
    {
        int16_t result = acc - value - (carry ? 0 : 1);
        cpu_set_a(cpu, (uint8_t)(result & 0xFF));
        cpu_set_flag(cpu, FLAG_V,((acc ^ value) & (acc ^ result) & FLAG_N) != 0);
        cpu_set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
        cpu_set_flag(cpu, FLAG_C, result >= 0);
        cpu_set_flag(cpu, FLAG_N, (result & FLAG_N) != 0);
    }
    return true;
}

bool sec(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_C, true);
    return true;
}

bool sed(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_D, true);
    return true;
}

bool sei(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    cpu_set_flag(cpu, FLAG_I, true);
    return true;
}

bool sta(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t acc = cpu_get_a(cpu);
    bus_write(bus, address, acc);
    return true;
}

bool stx(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t xr = cpu_get_x(cpu);
    bus_write(bus, address, xr);
    return true;
}

bool sty(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t yr = cpu_get_y(cpu);
    bus_write(bus, address, yr);
    return true;
}

bool tax(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t acc = cpu_get_a(cpu);
    cpu_set_x(cpu, acc);
    cpu_set_flag(cpu, FLAG_Z, acc == 0);
    cpu_set_flag(cpu, FLAG_N, (acc & FLAG_N) != 0);
    return true;
}

bool tay(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t acc = cpu_get_a(cpu);
    cpu_set_y(cpu, acc);
    cpu_set_flag(cpu, FLAG_Z, acc == 0);
    cpu_set_flag(cpu, FLAG_N, (acc & FLAG_N) != 0);
    return true;
}

bool tsx(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t sp = cpu_get_sp(cpu);
    cpu_set_x(cpu, sp);
    cpu_set_flag(cpu, FLAG_Z, sp == 0);
    cpu_set_flag(cpu, FLAG_N, (sp & FLAG_N) != 0);
    return true;
}

bool txa(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t xr = cpu_get_x(cpu);
    cpu_set_a(cpu, xr);
    cpu_set_flag(cpu, FLAG_Z, xr == 0);
    cpu_set_flag(cpu, FLAG_N, (xr & FLAG_N) != 0);
    return true;
}

bool txs(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t xr = cpu_get_x(cpu);
    cpu_set_sp(cpu, xr);
    return true;
}

bool tya(cpu_t *cpu, c64_bus_t *bus, uint16_t address)
{
    uint8_t yr = cpu_get_y(cpu);
    cpu_set_a(cpu, yr);
    cpu_set_flag(cpu, FLAG_Z, yr == 0);
    cpu_set_flag(cpu, FLAG_N, (yr & FLAG_N) != 0);
    return true;
}