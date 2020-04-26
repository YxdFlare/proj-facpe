#include "common.h"
#include "wrapper_constants.h"
#include "wrapper_datastruct.h"
#include "top.h"
#include "encoder.h"
#include "wrapper_datastruct.h"
#include "top_constants.h"
#include "directed_test.h"

#include <stdio.h>
#include <stdlib.h>

RF _rf;
u32 _dut_value[1];
u32 _dut_state[1];
int cycle_cnt = 0;

#define ITEM_NBR 100
#define RANDOM_TEST 1

//-------------------------------------------------------------------------------
// DUFT helper functions
//-------------------------------------------------------------------------------

int send_op(u32 operation, u32 target_state, int timer[1])
{
  top(DUFT,OPCODE_BASE,operation,WRITE,0,0);
  top(DUFT,OPCODE_BASE,NONE,WRITE,0,0);
  u32 landed_state = INVALID_STATE;
  while (landed_state != target_state) {
    landed_state = (top(DUFT,STATE_BASE,0,READ,0,0) & WRAPPER_FSM_CS);
    if (*timer < 1000)
      (*timer) ++;
    else {
      printf("DUFT Response Timed Out! {op:%d =>%d, stuck in %d} ",operation,target_state,landed_state);
      return 1;
    }   
  }
  return 0;
}

int call_dut(u32 input, u32* output)
{
  int err = 0;
  int tim = 0;
  top(DUFT,DUT_IN_BASE,input,WRITE,0,0);
  tim = 0;
  err = send_op(INPUT,INPUT_RDY,&tim);
  if(err) return err;
  tim = 0;
  err = send_op(RUN,OUTPUT_VAL,&tim);
  if(err) return err;
  tim = 0;
  err = send_op(ENDR,IDLE,&tim);
  if(err) return err;
  *output = top(DUFT,DUT_OUT_BASE,0,READ,0,0);
  return 0;  
}

int call_dft(u32 input,u32* dft_buf)
{
  int err = 0;
  int lat = 0;
  int i = 0;
  int tim = 0;
  top(DUFT,DUT_IN_BASE,input,WRITE,0,0);
  tim = 0;
  err = send_op(INPUT,INPUT_RDY,&tim);
  if(err) return err;
  tim = 0;
  err = send_op(TEST,SCAN_RD,&tim);
  if(err) return err;
  
  do {
    for(i = 0; i < DUMP_NBR; i++)
      *(dft_buf + lat * DUMP_NBR + i) = top(DUFT,DFT_OUT_BASE + i,0,READ,0,0);
    tim = 0;
    err = send_op(NEXT,SCAN_RD,&tim);
    if(err) return err;
    lat++;
  } while (!(top(DUFT,STATE_BASE,0,READ,0,0) & DUT_OP_CM));
    
  for(i = 0; i < DUMP_NBR; i++)
    *(dft_buf + lat * DUMP_NBR + i) = top(DUFT,DFT_OUT_BASE + i,0,READ,0,0);
  tim = 0;
  err = send_op(ENDT,IDLE,&tim);
  if(err) return err;
  return 0;
}


//-------------------------------------------------------------------------------
// Test Harness
//-------------------------------------------------------------------------------

int main()
{
  printf("\n\nIN MAIN : \n=================\n");
  // define data structures (memory allocation)
  /* ai */                  u32 test_inputs[ITEM_NBR];
                            u32* test_inputs_ptr = &test_inputs[0];
  /* ai+8*/                 u32 dut_outputs[ITEM_NBR];
                            u32* dut_outputs_ptr = &dut_outputs[0];
  /* ai,ai+1,ai+2...*/      u32 dcs[ITEM_NBR][MAX_LATENCY][DUMP_NBR]; // dft_collected_states
                            u32* dcs_ptr = &dcs[0][0][0];
  /* pwr0i,pwr1i,pwr2i...*/ float final_results[ITEM_NBR][MAX_LATENCY-1];
                            float* final_results_ptr = &final_results[0][0];
  printf("Data structure initialized.\n");
  printf("Initializing inputs......");
  int i = 0; // index for test items
  for (i = 0; i < ITEM_NBR; i++)
    if(RANDOM_TEST)
      test_inputs[i] = rand();
    else
      test_inputs[i] = test_inputgen(i);
    
  printf("Inputs initialized.\nInitializing test harness......");

  int all_passing = 1;
  int err_dft = 0;
  int err_dut = 0;
  i = 0;
  printf("Test harness initialized. Start testing.\n");
  printf("--------------------------------------\n");
  while (i < ITEM_NBR && all_passing) {
    printf("Test item %d ...",i);
    // input a number to the DUFT and get dcs
    err_dft = call_dft(*test_inputs_ptr,dcs_ptr);
    // input a number to the DUT and get output
    err_dut = call_dut(*test_inputs_ptr,dut_outputs_ptr);
    // encode dcs into images
    top(ENCODE,0,0,0,dcs_ptr,0);
    // process these images
    top(PROCESS,0,0,0,0,final_results_ptr);
    // update testsource status
    all_passing = !err_dft && !err_dut;       
    printf(" %s\n",all_passing ? "[NO ERROR]\0" : "[ERROR]\0");
    i++;
    // pointer bumping
    dcs_ptr += MAX_LATENCY * DUMP_NBR;
    test_inputs_ptr += 1;
    dut_outputs_ptr += 1;
    final_results_ptr += (MAX_LATENCY-1);
  }
  int tested_items =0;
  // error message
  if(!all_passing) {
    printf("[ERROR] : Test FAILED at case %d ",i-1);
    if(err_dft) printf("[DFT ERROR] ");
    if(err_dut) printf("[DUT ERROR]");
    printf("\n");
  }
  
  tested_items = all_passing ? ITEM_NBR : i;


  //print results
  int j = 0; // index for cycles
  int k = 0; // index for dumps
  for (i = 0; i < tested_items; i++) {
    printf("--------------------------------------\nTest item %d\n",i);
    printf("Data input and output : %0x => %0x\n",test_inputs[i],dut_outputs[i]);
    printf("Analysed results :\t");
    for (j = 0; j < MAX_LATENCY-1; j++) {
      printf("%5.3f ",final_results[i][j]);
    }
    printf("\n");
  }
  printf("--------------------------------------\n");
  return 1 - all_passing;
}