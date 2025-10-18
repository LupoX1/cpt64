#include "iny.h"

void fC8(cpu_6510_t *cpu, memory_t ram)
{
   uint8_t result = read_yr(cpu) + 1;
   write_yr(cpu, result);

   set_negative_flag(cpu, (result & 0x80) != 0);
   set_zero_flag(cpu, result == 0);
}
