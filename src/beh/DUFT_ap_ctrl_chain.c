#include "DUT.h"
#include "wrapper_constants.h"
#include "wrapper_datastruct.h"
#include <stdio.h>

// state definiton
FSM_state FSM_IDLE           = {.CS = IDLE,         .LAT = 0};
FSM_state FSM_INPUT_FLATTEN  = {.CS = INPUT_FLATTEN,.LAT = (DUT_ILEN+31)/32}; 
FSM_state FSM_INPUT_DUT      = {.CS = INPUT_DUT,    .LAT = DUT_IACK};
FSM_state FSM_INPUT_RDY      = {.CS = INPUT_RDY,    .LAT = 0};
FSM_state FSM_OUTPUT_WAIT    = {.CS = OUTPUT_WAIT,  .LAT = MAX_LATENCY};
FSM_state FSM_OUTPUT_VAL     = {.CS = OUTPUT_VAL,   .LAT = 0};
FSM_state FSM_OUTPUT_PACK    = {.CS = OUTPUT_PACK,  .LAT = (DUT_OLEN+31)/32};
FSM_state FSM_SCAN_PREP      = {.CS = SCAN_PREP,    .LAT = DFT_IACK};
FSM_state FSM_SCAN           = {.CS = SCAN,         .LAT = DUMP_NBR*37+4};
FSM_state FSM_SCAN_RD        = {.CS = SCAN_RD,      .LAT = 0};
FSM_state FSM_TICK           = {.CS = TICK,         .LAT = 1};

void dft_scan(RF* regfile, int dump_nbr, u32 _dut_value[1])
{
  for (int i = 0; i < dump_nbr; i++)
    regfile->dft_out[i] = _dut_value[i];
}

int rf_update(RF* regfile, int* cycle_cnt, u32 _dut_state[1], u32 _dut_value[1], int check, int op)
{
  int transition = 0;
  switch ((regfile->state) & WRAPPER_FSM_CS) {
    
  case IDLE:
    *cycle_cnt = 0;
    if (*cycle_cnt < FSM_IDLE.LAT) {
      transition = 0;
    }
    else {
      if (op && !check) {
        *cycle_cnt = *cycle_cnt - FSM_IDLE.LAT + POST_IO_LATENCY * (1-check);
        transition = 1;
      }
      else
        *cycle_cnt = 0;  
      
      dut_reset(_dut_value,_dut_state); 
      if(regfile->opcode == INPUT)      
        regfile->state = INPUT_FLATTEN;
    }    
    break;

  case INPUT_FLATTEN:
    if (*cycle_cnt < FSM_INPUT_FLATTEN.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_INPUT_FLATTEN.LAT + POST_IO_LATENCY * (1-check);
      transition = 1; 
      regfile->state = INPUT_DUT;
    }    
    break;
  
  case INPUT_DUT:
    if (*cycle_cnt < FSM_INPUT_DUT.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_INPUT_DUT.LAT + POST_IO_LATENCY * (1-check);
      transition = 1;
      _dut_value[0] = regfile->dut_in[0]; 
      _dut_state[0] = 0;
      regfile->state = INPUT_RDY;
    }    
    break;

  case INPUT_RDY:
    *cycle_cnt = 0;
    if (*cycle_cnt < FSM_INPUT_RDY.LAT) {

      transition = 0;
    }
    else {
      if (op && !check) {
        *cycle_cnt = *cycle_cnt - FSM_INPUT_RDY.LAT + POST_IO_LATENCY * (1-check);
        transition = 1;
      }
      else
        *cycle_cnt = 0;

      if(regfile->opcode == RUN) 
        regfile->state = OUTPUT_WAIT;
      else if (regfile->opcode == TEST) {
        regfile->state = SCAN_PREP;
      }
    }      
    break;
  
  case OUTPUT_WAIT:
    if (*cycle_cnt < FSM_OUTPUT_WAIT.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_OUTPUT_WAIT.LAT + POST_IO_LATENCY * (1-check);
      transition = 1;
      dut_run(_dut_value);
      regfile->state = OUTPUT_VAL;
    }      
    break;
  
  case OUTPUT_VAL:
    *cycle_cnt = 0;
    if (*cycle_cnt < FSM_OUTPUT_VAL.LAT) {  
      transition = 0;
    }
    else {
      if(op && !check) {
        *cycle_cnt = *cycle_cnt - FSM_OUTPUT_VAL.LAT + POST_IO_LATENCY * (1-check);
        transition = 1;
      }
      else 
        *cycle_cnt = 0;
      if(regfile->opcode == ENDR)
        regfile->state = OUTPUT_PACK;
    }      
    break;
  
  case OUTPUT_PACK:
    if (*cycle_cnt < FSM_OUTPUT_PACK.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_OUTPUT_PACK.LAT + POST_IO_LATENCY * (1-check);
      transition = 1;
      regfile->dut_out[0] = _dut_value[0];
      regfile->state = IDLE;
    }      
    break;
  
  case SCAN_PREP:
    if (*cycle_cnt < FSM_SCAN_PREP.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_SCAN_PREP.LAT + POST_IO_LATENCY * (1-check);
      transition = 1;
      regfile->state = SCAN;
    }      
    break;
  
  case SCAN:
    if (*cycle_cnt < FSM_SCAN.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_SCAN.LAT + POST_IO_LATENCY * (1-check);
      transition = 1;
      dft_scan(regfile,DUMP_NBR,_dut_value);
      regfile->state = SCAN_RD;
    }      
    break;
  
  case SCAN_RD:
    *cycle_cnt = 0;
    if (*cycle_cnt < FSM_SCAN_RD.LAT) {      
      transition = 0;
    }
    else {
      if(op && !check) {
        *cycle_cnt = *cycle_cnt - FSM_SCAN_RD.LAT + POST_IO_LATENCY * (1-check);
        transition = 1;
      }
      else
        *cycle_cnt = 0;

      if(regfile->opcode == ENDT) 
        regfile->state = IDLE;
      else if(regfile->opcode == NEXT) 
        regfile->state = TICK; 
      }
    break;

  case TICK:
    if (*cycle_cnt < FSM_TICK.LAT) {
      (*cycle_cnt) +=  POST_IO_LATENCY * (1-check);
      transition = 0;
    }
    else {
      *cycle_cnt = *cycle_cnt - FSM_TICK.LAT + POST_IO_LATENCY * (1-check);
      transition = 1;
      dut_tick(_dut_value,_dut_state);
      regfile->state = SCAN_PREP;
      }
    break;
  
  
  default:
    break;
  }
  // dut_op_commit signal
  if(_dut_state[0] == 8)
    regfile->state |= DUT_OP_CM;

  // test resgisters
  regfile->test_out = regfile->test_in;

  return transition;
}

u32* addr_map(RF* regfile, u32 addr)
{
  // locating register files
  if (DUT_IN_BASE <= addr && addr < DFT_OUT_BASE + 64) {
    if(DUT_IN_BASE <= addr && addr < DUT_IN_BASE + 8)
      return &(regfile->dut_in[0]) + addr - DUT_IN_BASE;
    else if (DUT_OUT_BASE <= addr && addr < DUT_OUT_BASE + 8)
      return &(regfile->dut_out[0]) + addr - DUT_OUT_BASE;
    else
      return &(regfile->dft_out[0]) + addr - DFT_OUT_BASE;
  }
  // locating single registers
  else switch (addr) {
    case OPCODE_BASE:
      return &(regfile->opcode);
    case STATE_BASE:
      return &(regfile->state);
    case CONFIG_BASE:
      return &(regfile->config);
    case TEST_IN_BASE:
      return &(regfile->test_in);
    case TEST_OUT_BASE:
      return &(regfile->test_out);  
    default:
      return (u32*) NULL_ADDR;
  }
}

void DUFT_reset(RF* regfile, u32* _dut_value, u32* _dut_state, int* cycle_cnt_ptr)
{
  regfile->opcode = 0;
  regfile->config = 0;
  regfile->state = 0;
  regfile->test_in = 0;
  regfile->test_out = 0;
  int i;
  for (i = 0; i < 8; i++) {
    regfile->dut_in[i] = 0;
    regfile->dut_out[i] = 0;
  }
  for (i = 0; i < 64; i++) {
    regfile->dft_out[i] = 0;
  }
  *cycle_cnt_ptr = 0;
}


u32 DUFT_ap_ctrl_chain(u32 addr, u32 wr_data, u32 rd_wr)
{   
  static RF _rf = {.opcode = NONE,.state = IDLE};
  static int cycle_cnt = 0;
  static u32 _dut_value[1] = {0};
  static u32 _dut_state[1] = {0};
  u32* ptr;
  u32 ret_value = 0;
  ptr = addr_map(&_rf,addr);
  cycle_cnt += PRE_IO_LATENCY;
  int check = 0;
  int op = 0;
  if(rd_wr == READ) {    
    do {
      ret_value = *ptr;
      op = 0;
      //fprintf(stderr,"\t\t\t[%d](%d)(%s) -> ",_rf.state,cycle_cnt,check?"o":" ");      
      check = rf_update(&_rf,&cycle_cnt,_dut_state,_dut_value,check,op);
      //fprintf(stderr,"[%d](%d)(%s)\n",_rf.state,cycle_cnt,check?"o":" "); 
    } while (check);
    //fprintf(stderr,"\t\t\t--------\n");
  }    
  else if (rd_wr == WRITE) {
    *ptr = wr_data;
    if (addr == OPCODE_BASE) op = 1;
    
    do {
      //fprintf(stderr,"\t\t\t[%d](%d)(%s) -> ",_rf.state,cycle_cnt,check?"o":" ");       
      check = rf_update(&_rf,&cycle_cnt,_dut_state,_dut_value,check,op);
      //fprintf(stderr,"[%d](%d)(%s)\n",_rf.state,cycle_cnt,check?"o":" ");
    } while (check);
    //fprintf(stderr,"\t\t\t--------\n");
  }
  
  else DUFT_reset(&_rf,_dut_value,_dut_state,&cycle_cnt);  
  
  return ret_value; 
}