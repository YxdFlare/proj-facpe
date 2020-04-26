#include "type.h"

void dut_tick(u32* current_value, u32* current_state)
{
  if (*current_state < 9) {
    (*current_state) ++;
    (*current_value) ++;
  }
  else
    *current_state = 0;  
}

void dut_run(u32* current_value)
{
  *current_value += 8;
}
