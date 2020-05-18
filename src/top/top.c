#include "common.h"
#include "dataproc.h"
#include "encoder.h"
#include "DUFT_ap_ctrl_chain.h"
#include "wrapper_constants.h"
#include "wrapper_datastruct.h"
#include "top_constants.h"

#include <stdio.h>

// #include "wrapper_datastruct.h"

// extern RF _rf;
// extern u32 _dut_value[1];
// extern u32 _dut_state[1];
// extern int cycle_cnt;


//-------------------------------------------------------------------------------
// Top level
//-------------------------------------------------------------------------------


int top(int func, u32 addr, u32 data, int rd_wr, u32 dcs[MAX_LATENCY*DUMP_NBR], 
        u32 encoded_imgset[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR], float final_results[MAX_LATENCY-1])
{
  int DUFT_return = 0;
  switch (func) {
  case DUFT:
    DUFT_return = DUFT_ap_ctrl_chain(addr,data,rd_wr);
    break;
  case ENCODE:
    batch_encode(dcs,encoded_imgset);
    break;
  // case PROCESS:
  //   dataproc_avg(encoded_imgset,final_results);
  //   break;  
  default:
    break;
  } 
  return DUFT_return;
}