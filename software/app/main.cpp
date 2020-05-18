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

//#include "main.hpp"
#include <ap_int.h>
#include "../include/hw_settings.h"

#if PORT_BITWIDTH_64BIT
#define GMEM_INPUTTYPE       unsigned long long int
#define GMEM_INTYPE_OTHER    ap_uint<64>
#define GMEM_WEIGHTTYPE      ap_uint<64>
#define GMEM_OUTTYPE         ap_uint<64>
#define GMEM_MAXPOOLTYPE     ap_uint<128>
#define GMEM_INTYPE_FC       ap_uint<128>
#define GMEM_BIASTYPE        unsigned long long int
#else
#define GMEM_INPUTTYPE       unsigned long long int
#define GMEM_INTYPE_OTHER    ap_uint<128>
#define GMEM_WEIGHTTYPE      ap_uint<128>
#define GMEM_OUTTYPE         ap_uint<128>
#define GMEM_MAXPOOLTYPE     ap_uint<128>
#define GMEM_INTYPE_FC       ap_uint<128>
#define GMEM_BIASTYPE        unsigned long long int
#endif  //#if PORT_BITWIDTH_64BIT

#define CONV_FLAG 0		
#define POOL_FLAG 1		
#define DECONV_FLAG 2		
#define __CONV_ENABLE__ 1		
#define __POOL_ENABLE__ 1		
#define __DECONV_ENABLE__ 0		

#define CHAR_TYPE     char
#define SHORT_TYPE    short
#define INT_TYPE      int


int DnnWrapper( 		
  #if __CONV_ENABLE__==1		
          GMEM_WEIGHTTYPE *weights1, GMEM_WEIGHTTYPE *weights2,		
  #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))		
          GMEM_WEIGHTTYPE *weights3, GMEM_WEIGHTTYPE *weights4,		
  #endif		
          GMEM_OUTTYPE *output1,		
  #if !SINGLE_IO_PORT_EN		
          GMEM_OUTTYPE *output2,		
  #endif		
          GMEM_INTYPE_OTHER *input_other1,		
  #if !SINGLE_IO_PORT_EN		
          GMEM_INTYPE_OTHER *input_other2,		
  #endif		
          GMEM_INPUTTYPE *input_1st, GMEM_BIASTYPE *bias,		
  #if !DISABLE_BN		
          GMEM_INPUTTYPE *input_norm2, GMEM_INPUTTYPE *input_norm3,		
  #endif		
          GMEM_INPUTTYPE *istg_out1,		
  #if !SINGLE_IO_PORT_EN		
          GMEM_INPUTTYPE *istg_out2,		
  #endif		
          int *scalar_conv_args,		
  #endif //CONV kernel		
  #if __POOL_ENABLE__==1		
          GMEM_MAXPOOLTYPE *in1,GMEM_MAXPOOLTYPE *in2,		
          GMEM_MAXPOOLTYPE *out1,GMEM_MAXPOOLTYPE *out2,		
          GMEM_MAXPOOLTYPE *wts,		
          int *scalar_pool_args,		
  #endif//POOL kernel		
  #if __DECONV_ENABLE__==1		
          short* deconvIN, short* deconvWT, 		
          short* deconvBias, unsigned long long int* deconvIDout, int *scalar_deconv_args,		
  #endif//DECONV kernel		
          int flag,
// duft top-level function arguments
int func, u32 addr, u32 data, int rd_wr, u32 dcs[MAX_LATENCY*DUMP_NBR],u32 encoded_imgset[(MAX_LATENCY-1)*SIZE*SIZE*CH_NBR], float final_results[MAX_LATENCY-1]
);	

#define ITEM_NBR 10
#define RANDOM_TEST 0

using namespace std;
//-------------------------------------------------------------------------------
// DUFT helper functions
//-------------------------------------------------------------------------------
int send_op(u32 operation, u32 target_state, int timer[1])
{
  DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,
  DUFT,OPCODE_BASE,operation,WRITE,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{wr(0x%x) > DUFT._rf.opcode}\n",operation);
  #endif
  DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                  #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                          (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                  #endif
                          (GMEM_OUTTYPE *)0,
                  #if !SINGLE_IO_PORT
                          (GMEM_OUTTYPE *)0,
                  #endif
                          (GMEM_INTYPE_OTHER *)0,
                  #if !SINGLE_IO_PORT
                          (GMEM_INTYPE_OTHER *)0,
                  #endif
                          (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                  #if !DISABLE_BN
                          (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                  #endif
                          (GMEM_INPUTTYPE *)0,
                  #if !SINGLE_IO_PORT
                          (GMEM_INPUTTYPE *)0,
                  #endif
                          0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,
  DUFT,OPCODE_BASE,NONE,WRITE,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{wr(0x%x) > DUFT._rf.opcode}\n",NONE);
  #endif
  u32 landed_state = INVALID_STATE;
  while (landed_state != target_state) {  
    landed_state = 
      DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,
      DUFT,STATE_BASE,0,READ,0,0,0);
    #ifdef USER_DEBUG
      fprintf(stderr,"\t\t{rd(0x%x) < DUFT._rf.state}\n",landed_state); 
    #endif
    landed_state = landed_state & WRAPPER_FSM_CS;
    if (*timer < 1000)
      (*timer) ++;
    else {
      #ifdef USER_DEBUG
        fprintf(stderr,"DUFT Response Timed Out! {op:%d =>%d, stuck in %d} ",operation,target_state,landed_state);
      #endif
      return 1;
    }   
  }
  return 0;
}

int call_dut(u32 input, u32* output)
{
  // reset DUFT
  DnnWrapper(               
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,                        
  DUFT,0,0,RESET,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{wr(RESET) > DUFT}\n");
  #endif
  int err = 0;
  int tim = 0;
  cerr << "\t[Write Input]\n";
  DnnWrapper(               
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,                        
  DUFT,DUT_IN_BASE,input,WRITE,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{wr(0x%x) > DUFT._rf.dut_in}\n",input);  
  #endif
  tim = 0;
  cerr << "\t[Op INPUT]\n";
  err = send_op(INPUT,INPUT_RDY,&tim); 
  if(err) return err;
  tim = 0;
  cerr << "\t[Op RUN]\n";
  err = send_op(RUN,OUTPUT_VAL,&tim);  
  if(err) return err;
  tim = 0;
  cerr << "\t[Op ENDR]\n";
  err = send_op(ENDR,IDLE,&tim);
  if(err) return err;
  cerr << "\t[Collect results]\n";
  int result;
  result = 
    DnnWrapper(               
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,                        
    DUFT,DUT_OUT_BASE,0,READ,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{rd(0x%x) < DUFT._rf.dut_out}\n",result);
  #endif
  *output = result;  
  return 0;
}

int call_dft(u32 input,u32* dft_buf)
{
  // reset DUFT
  DnnWrapper(               
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,                        
  DUFT,0,0,RESET,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{wr(RESET) > DUFT}\n");
  #endif
  int err = 0;
  int lat = 0;
  int i = 0;
  int tim = 0;
  u32 dut_done = 0;
  cerr << "\t[Write Input]\n";
  DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,                        
  DUFT,DUT_IN_BASE,input,WRITE,0,0,0);
  #ifdef USER_DEBUG
    fprintf(stderr,"\t\t{wr(0x%x) > DUFT._rf.dut_in}\n",input);
  #endif    


  tim = 0;
  cerr << "\t[Op INPUT]\n";
  err = send_op(INPUT,INPUT_RDY,&tim);
  
  if(err) return err;
  tim = 0;
  cerr << "\t[Op TEST]\n";
  err = send_op(TEST,SCAN_RD,&tim);
  
  if(err) return err;
  
  do {
    for(i = 0; i < DUMP_NBR; i++) {
      cerr << "\t[Collect dut status]\n";
      u32 dut_status;
      dut_status = 
      DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,
      DUFT,DFT_OUT_BASE + i,0,READ,0,0,0);
      #ifdef USER_DEBUG
        fprintf(stderr,"\t\t{rd(0x%x) < DUFT._rf.dft_out}\n",dut_status);
      #endif
      *(dft_buf + lat * DUMP_NBR + i) = dut_status;
    }

    dut_done = 
      DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,
      DUFT,STATE_BASE,0,READ,0,0,0);
    #ifdef USER_DEBUG
      fprintf(stderr,"\t\t{rd(0x%x) < DUFT._rf.state}\n",dut_done);
    #endif
    dut_done = dut_done & DUT_OP_CM;
       
    if(!dut_done) {
      tim = 0;
      cerr << "\t[Op NEXT]\n";
      err = send_op(NEXT,SCAN_RD,&tim);
      if(err) return err;
      lat++;
    }    

  } while (!dut_done);
      
  tim = 0;
  cerr << "\t[Op ENDT]\n";
  err = send_op(ENDT,IDLE,&tim);

  if(err) return err;
  return 0;
}

// CHaiDNN paths
char* dirpath = "/home/yd383/proj-facpe/model";
char* prototxt = "base2_c_qtz.prototxt";
char* caffemodel = "base2_c.caffemodel";


int main()
{
  //-------------------------------------------------------------------------------
  // DUFT
  //-------------------------------------------------------------------------------
  cerr << endl << endl << "IN MAIN : \n=================" << endl;
  cerr << "Initializing data structure (Memory allocation)" << endl;
  cerr << left;
  // used for print address of pointers
  u32**  pp;
  float**  ff;
  // define data structures (memory allocation)
  /* ai */                   u32 test_inputs[ITEM_NBR];
                             u32* test_inputs_ptr = (u32*)test_inputs;
  cerr << setw(22) << "    test inputs at "     << hex << test_inputs_ptr;
  pp = &test_inputs_ptr;
  fprintf(stderr," <-(%p)\n",pp);

  /* ai+8*/                  u32 dut_outputs[ITEM_NBR];
                             u32* dut_outputs_ptr = (u32*)dut_outputs;
  cerr << setw(22) << "    dut outputs at "     << hex << dut_outputs_ptr;
  pp = &dut_outputs_ptr;
  fprintf(stderr," <-(%p)\n",pp);

  /* ai,ai+1,ai+2...*/       u32 dcs[ITEM_NBR][MAX_LATENCY][DUMP_NBR]; // dft_collected_states
                             u32* dcs_ptr = &(dcs[0][0][0]);
  cerr << setw(22) << "    dcs at "             << hex << dcs_ptr;
  pp = &dcs_ptr;
  fprintf(stderr," <-(%p)\n",pp);
                             u32 encoded_imgset[ITEM_NBR][(MAX_LATENCY-1)][SIZE][SIZE][CH_NBR];
                             u32* imgset_ptr = &(encoded_imgset[0][0][0][0][0]);
  cerr << setw(22) << "    encoded imgset at "  << hex << imgset_ptr;
  pp = &imgset_ptr;
  fprintf(stderr," <-(%p)\n",pp);

  /* pwr0i,pwr1i,pwr2i...*/  float final_results[ITEM_NBR][MAX_LATENCY-1];
                             float* final_results_ptr = &(final_results[0][0]);
                             float final_results_before[ITEM_NBR][MAX_LATENCY-1];
  cerr << setw(22) << "    final results at "   << hex << final_results_ptr;
  ff = &final_results_ptr;
  fprintf(stderr," <-(%p)\n",ff);

  int null = 0;
  null  = null || test_inputs_ptr == nullptr;
  null  = null || dut_outputs_ptr == nullptr;
  null  = null || dcs_ptr == nullptr;
  null  = null || imgset_ptr == nullptr;
  null  = null || final_results_ptr == nullptr;
  if (null) {
    cerr << "[ERROR] Data structure initializaion faild! (nullptr)" << endl;
    return -1;
  }  
  int IMG_SIZE = SIZE*SIZE*CH_NBR;
  cerr << "Data structure initialized." << endl;
  cerr << "Initializing inputs......\n";

  int i = 0; // index for test items
  int j = 0; // index for cycles
  int k = 0; // index for dumps

  for (i = 0; i < ITEM_NBR; i++)
    if(RANDOM_TEST)
      test_inputs[i] = rand();
    else
      test_inputs[i] = test_inputgen(i);
  if (ITEM_NBR < 20) {
    cerr << "[ ";
    for(i = 0;i<ITEM_NBR;i++)
      cerr << hex << test_inputs[i] << " ";
    cerr << "]" << endl;
  }
  cerr << "Inputs initialized.\nInitializing outputs......";
  int empty_value = 9.99;
  for (i = 0; i < ITEM_NBR; i++)
    for (j = 0; j < MAX_LATENCY-1; j++) {
      final_results[i][j] = empty_value;
      final_results_before[i][j] = empty_value;
  }
  cerr << "Outputs initialized.\nInitializing test harness......";
  int all_passing = 1;
  int err_dft = 0;
  int err_dut = 0;
  int err_encode = 0;
  int err_chai = 0;
  i = 0;
  cerr << "Test harness initialized. Start testing." << endl;
  cerr << "--------------------------------------" << endl;
  while (i < ITEM_NBR && all_passing) {
    cerr << "Test item " << i << " . " << endl;
    
    // input a number to the DUFT and get dcs
    fprintf(stderr,"[DUFT] (main) DFT reading state : (I@{0x%x},O@{0x%x})\n",test_inputs_ptr,dcs_ptr);
    err_dft = call_dft(*test_inputs_ptr,dcs_ptr);
    cerr << "[DUFT] (main) DFT COMPLETE" << endl;

    // input a number to the DUT and get output
    fprintf(stderr,"[DUFT] (main) DUT test : (I@{0x%x},O@{0x%x})\n",test_inputs_ptr,dut_outputs_ptr);
    err_dut = call_dut(*test_inputs_ptr,dut_outputs_ptr);
    cerr << "[DUFT] (main) DUT COMPLETE" << endl;

    // encode dcs into images
    fprintf(stderr,"[ENCODE] (main) Encoding states : (I@{0x%x},O@{0x%x})\n",dcs_ptr,imgset_ptr);
    err_encode = 
    DnnWrapper(
                #if __CONV_ENABLE__==1
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #if (KER_PROC==16 || (PORT_BITWIDTH_64BIT==1 && KER_PROC==8))
                        (GMEM_WEIGHTTYPE *)0,(GMEM_WEIGHTTYPE *)0,
                #endif
                        (GMEM_OUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_OUTTYPE *)0,
                #endif
                        (GMEM_INTYPE_OTHER *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INTYPE_OTHER *)0,
                #endif
                        (GMEM_INPUTTYPE *)0, (GMEM_BIASTYPE *)0,
                #if !DISABLE_BN
                        (GMEM_INPUTTYPE *)0, (GMEM_INPUTTYPE *)0,
                #endif
                        (GMEM_INPUTTYPE *)0,
                #if !SINGLE_IO_PORT
                        (GMEM_INPUTTYPE *)0,
                #endif
                        0,
                #endif//CONV kernel
                #if __POOL_ENABLE__==1
                        (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE *)0, (GMEM_MAXPOOLTYPE *)0,
                                          (GMEM_MAXPOOLTYPE*) 0,
                                    0,
                #endif//POOL kernel
                #if __DECONV_ENABLE__==1
                        0, 0, 0, (unsigned long long int*)0, 0,
                #endif//DECONV kernel
                        0,
    ENCODE,0,0,0,dcs_ptr,imgset_ptr,0);
    cerr << "[ENCODE] (main) Encode COMPLETE" << endl;
    // process these images
    #ifndef DUFT_ONLY
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
      #ifdef USER_DEBUG        // debug mode
        bound = 2;
      #endif
      int group_id = 0;
      while (2*group_id < bound && !err_chai) {
        cerr << "----------------------------------------------------------" << endl;
        img_path1 = imgset_ptr + group_id * 2 * IMG_SIZE;
        img_path2 = img_path1 + IMG_SIZE;
        cerr << 2*group_id + 1 << "," << 2*group_id + 2 << " of total " << MAX_LATENCY-1 << " images " << endl;
        cerr << "Image at " << hex << img_path1 << " " << hex << img_path2 << endl;
        dest = final_results_ptr + group_id * 2;
        cerr << "Result at " << hex << dest << endl;
        err_chai = dataproc_chai(dirpath,prototxt,caffemodel,img_path1,img_path2,dest);
        group_id ++;
        cerr << endl;
      }
    #endif
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
  
  for (i = 0; i < tested_items; i++) {
    printf("--------------------------------------\nTest item %d\n",i);
    printf("Data input and output : %0x => %0x\n",test_inputs[i],dut_outputs[i]);
    printf("Result Buffer before execution :");
    for (j = 0; j < MAX_LATENCY-1; j++) {
      printf("%7.6f ",final_results_before[i][j]);
    }
    printf("\n");
    printf("Analysed results :");
    for (j = 0; j < MAX_LATENCY-1; j++) {
      printf("%7.6f ",final_results[i][j]);
    }
    printf("\n");
  }
  printf("--------------------------------------\n");
  return 1 - all_passing;
}