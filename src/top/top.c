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

int top(int func, u32 addr, u32 data, int rd_wr, u32 dcs[MAX_LATENCY*DUMP_NBR], float final_results[MAX_LATENCY-1])
{
  // define data structures (memory allocation)
  static u32 encoded_imgset[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR];
  int DUFT_return = 0;
  switch (func) {
  case DUFT:
    DUFT_return = DUFT_ap_ctrl_chain(addr,data,rd_wr);
    break;
  case ENCODE:
    batch_encode(dcs,encoded_imgset);
    #ifndef __SYNTHESIS__
      printf("\nIN TOP:\n================\n");
      for (int i = 0; i < MAX_LATENCY-1; i++) {
        printf("%d->%d\n",i,i+1);
        print_array3d(encoded_imgset + i * SIZE * SIZE * CH_NBR,SIZE,SIZE,CH_NBR);
      }            
    #endif
    break;
  case PROCESS:
    dataproc_avg(encoded_imgset,final_results);
    break;  
  default:
    break;
  } 
  return DUFT_return;
}