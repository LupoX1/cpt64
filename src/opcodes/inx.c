#include "inx.h"

void fE8(cpu_6510_t *cpu, memory_t ram)
{
   uint8_t result = read_xr(cpu) + 1;
   write_xr(cpu, result);

   set_negative_flag(cpu, (result & 0x80) != 0);
   set_zero_flag(cpu, result == 0);
   increment_cycles(cpu, 2);
}

