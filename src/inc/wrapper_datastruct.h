// typedef struct wrapper_resigter_file RF;
// typedef struct wrapper_fsm_state FSM_state;
#ifndef WRAPPER_DATASTRUCT_H
#define WRAPPER_DATASTRUCT_H

typedef struct wrapper_resigter_file {
  u32 opcode;
  u32 state;
  u32 config;
  u32 dut_in_ptr[8];
  u32 dut_out_ptr[8];
  u32 dft_out_ptr[64];
  u32 test_in;
  u32 test_out;
} RF;

typedef struct wrapper_fsm_state {
  u32 CS;
  int LAT;
} FSM_state;

#endif