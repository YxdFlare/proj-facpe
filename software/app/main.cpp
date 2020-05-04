extern "C" {
  #include "common.h"
  #include "wrapper_constants.h"
  #include "wrapper_datastruct.h"
  #include "top.h"
  #include "encoder.h"
  #include "wrapper_datastruct.h"
  #include "top_constants.h"
  #include "directed_test.h"
}

#include "app_chai.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>

RF _rf;
u32 _dut_value[1];
u32 _dut_state[1];
int cycle_cnt = 0;

#define ITEM_NBR 1
#define RANDOM_TEST 0

using namespace std;
//-------------------------------------------------------------------------------
// DUFT helper functions
//-------------------------------------------------------------------------------

int send_op(u32 operation, u32 target_state, int timer[1])
{
  top(DUFT,OPCODE_BASE,operation,WRITE,0,0,0);
  top(DUFT,OPCODE_BASE,NONE,WRITE,0,0,0);
  u32 landed_state = INVALID_STATE;
  while (landed_state != target_state) {
    landed_state = (top(DUFT,STATE_BASE,0,READ,0,0,0) & WRAPPER_FSM_CS);
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
  top(DUFT,DUT_IN_BASE,input,WRITE,0,0,0);
  tim = 0;
  err = send_op(INPUT,INPUT_RDY,&tim);
  if(err) return err;
  tim = 0;
  err = send_op(RUN,OUTPUT_VAL,&tim);
  if(err) return err;
  tim = 0;
  err = send_op(ENDR,IDLE,&tim);
  if(err) return err;
  *output = top(DUFT,DUT_OUT_BASE,0,READ,0,0,0);
  return 0;  
}

int call_dft(u32 input,u32* dft_buf)
{
  int err = 0;
  int lat = 0;
  int i = 0;
  int tim = 0;

  top(DUFT,DUT_IN_BASE,input,WRITE,0,0,0);

  tim = 0;
  err = send_op(INPUT,INPUT_RDY,&tim);

  if(err) return err;
  tim = 0;
  err = send_op(TEST,SCAN_RD,&tim);

  if(err) return err;
  
  do {
    for(i = 0; i < DUMP_NBR; i++)
      *(dft_buf + lat * DUMP_NBR + i) = top(DUFT,DFT_OUT_BASE + i,0,READ,0,0,0);
    tim = 0;
    err = send_op(NEXT,SCAN_RD,&tim);

    if(err) return err;
    lat++;
  } while (!(top(DUFT,STATE_BASE,0,READ,0,0,0) & DUT_OP_CM));

  for(i = 0; i < DUMP_NBR; i++)
    *(dft_buf + lat * DUMP_NBR + i) = top(DUFT,DFT_OUT_BASE + i,0,READ,0,0,0);
  tim = 0;

  err = send_op(ENDT,IDLE,&tim);

  if(err) return err;
  return 0;
}

// CHaiDNN paths
char* dirpath = "/home/yd383/proj-facpe/model";
char* prototxt = "base1_c_qtz.prototxt";
char* caffemodel = "base1_c.caffemodel";


int main()
{
  //-------------------------------------------------------------------------------
  // DUFT
  //-------------------------------------------------------------------------------
  cout << endl << endl << "IN MAIN : \n=================" << endl;
  cout << "Initializing data structure (Memory allocation)" << endl;
  cout << left;
  // define data structures (memory allocation)
  /* ai */                  u32 test_inputs[ITEM_NBR];
                            u32* test_inputs_ptr = (u32*)test_inputs;
  cout << setw(22) << "    test inputs at "     << hex << test_inputs_ptr   << endl;

  /* ai+8*/                 u32 dut_outputs[ITEM_NBR];
                            u32* dut_outputs_ptr = (u32*)dut_outputs;
  cout << setw(22) << "    dut outputs at "     << hex << dut_outputs_ptr   << endl;

  /* ai,ai+1,ai+2...*/      u32 dcs[ITEM_NBR][MAX_LATENCY][DUMP_NBR]; // dft_collected_states
                            u32* dcs_ptr = &(dcs[0][0][0]);
  cout << setw(22) << "    dcs at "             << hex << dcs_ptr           << endl;
                            u32 encoded_imgset[ITEM_NBR][(MAX_LATENCY-1)][SIZE][SIZE][CH_NBR];
                            u32* imgset_ptr = &(encoded_imgset[0][0][0][0][0]);
  cout << setw(22) << "    encoded imgset at "  << hex << imgset_ptr        << endl;

  /* pwr0i,pwr1i,pwr2i...*/ float final_results[ITEM_NBR][MAX_LATENCY-1];
                            float* final_results_ptr = &(final_results[0][0]);
  cout << setw(22) << "    final results at "   << hex << final_results_ptr << endl;

  int null = 0;
  null  = null || test_inputs_ptr == nullptr;
  null  = null || dut_outputs_ptr == nullptr;
  null  = null || dcs_ptr == nullptr;
  null  = null || imgset_ptr == nullptr;
  null  = null || final_results_ptr == nullptr;
  if (null) {
    cout << "[ERROR] Data structure initializaion faild! (nullptr)" << endl;
    return -1;
  }  
  int IMG_SIZE = SIZE*SIZE*CH_NBR;
  cout << "Data structure initialized." << endl;
  cout << "Initializing inputs......\n";
  int i = 0; // index for test items
  for (i = 0; i < ITEM_NBR; i++)
    if(RANDOM_TEST)
      test_inputs[i] = rand();
    else
      test_inputs[i] = test_inputgen(i);
  if (ITEM_NBR < 20) {
    cout << "[ ";
    for(i = 0;i<ITEM_NBR;i++)
      cout << hex << test_inputs[i] << " ";
    cout << "]" << endl;
  }
      
  cout << "Inputs initialized.\nInitializing test harness......";

  int all_passing = 1;
  int err_dft = 0;
  int err_dut = 0;
  int err_encode = 0;
  int err_chai = 0;
  i = 0;
  cout << "Test harness initialized. Start testing." << endl;
  cout << "--------------------------------------" << endl;
  while (i < ITEM_NBR && all_passing) {
    cout << "Test item " << i << " . " << endl;
    
    // input a number to the DUFT and get dcs
    err_dft = call_dft(*test_inputs_ptr,dcs_ptr);
    
    // input a number to the DUT and get output
    err_dut = call_dut(*test_inputs_ptr,dut_outputs_ptr);
    
    // encode dcs into images
    err_encode = top(ENCODE,0,0,0,dcs_ptr,imgset_ptr,0);
    // process these images

    //-------------------------------------------------------------------------------
    // CHaiDNN
    //-------------------------------------------------------------------------------
    int bound = 0;
    u32* img_path1;
    u32* img_path2;
    float* dest;
    if ((MAX_LATENCY-1)%2)
      bound = MAX_LATENCY-2; // odd number of images
    else
      bound = MAX_LATENCY-1; // even number of images
    int group_id = 0;
    while (2*group_id < bound && !err_chai) {
      cerr << "___________________________________________________" << endl;
      img_path1 = imgset_ptr + group_id * 2 * IMG_SIZE;
      img_path2 = img_path1 + IMG_SIZE;
      cerr << 2*group_id << "," << 2*group_id + 1 << " of total " << MAX_LATENCY-1 << " images " << endl;
      cerr << "Image at " << hex << img_path1 << " " << hex << img_path2 << endl;
      dest = final_results_ptr + group_id * 2;
      cerr << "Result at " << hex << dest << endl;
      err_chai = dataproc_chai(dirpath,prototxt,caffemodel,img_path1,img_path2,dest);
      group_id ++;
      cerr << endl;
    }
    // update testsource status
    all_passing = !err_dft && !err_dut && !err_chai && !err_encode;       
    printf(" %s\n",all_passing ? "[NO ERROR]\0" : "[ERROR]\0");
    i++;
    // pointer bumping
    dcs_ptr += MAX_LATENCY * DUMP_NBR;
    test_inputs_ptr += 1;
    dut_outputs_ptr += 1;
    imgset_ptr += (MAX_LATENCY-1)*SIZE*SIZE*CH_NBR;
    final_results_ptr += (MAX_LATENCY-1);
  }
  int tested_items =0;
  // error message
  if(!all_passing) {
    printf("[ERROR] : Test FAILED at case %d ",i-1);
    if(err_dft) printf("[DFT ERROR] ");
    if(err_dut) printf("[DUT ERROR]");
    if(err_chai) printf("[CHAIDNN ERROR]");
    printf("\n");
  }
  
  tested_items = all_passing ? ITEM_NBR : i;


  //print results
  int j = 0; // index for cycles
  int k = 0; // index for dumps
  for (i = 0; i < tested_items; i++) {
    printf("--------------------------------------\nTest item %d\n",i);
    printf("Data input and output : %0x => %0x\n",test_inputs[i],dut_outputs[i]);
    printf("Analysed results :");
    for (j = 0; j < MAX_LATENCY-1; j++) {
      printf("%5.2f ",final_results[i][j]);
    }
    printf("\n");
  }
  printf("--------------------------------------\n");





  return 1 - all_passing;
}