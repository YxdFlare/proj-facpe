#include "DUT.h"
#include "wrapper_constants.h"
#include "wrapper_datastruct.h"

extern RF _rf;
extern u32 _dut_value[1];
extern u32 _dut_state[1];
extern int cycle_cnt;

// state definiton
FSM_state FSM_IDLE           = {.CS = IDLE,         .LAT = 0};
FSM_state FSM_INPUT_FLATTEN  = {.CS = INPUT_FLATTEN,.LAT = DUT_ILEN/32 + (DUT_ILEN%32 ? 1 : 0)}; 
FSM_state FSM_INPUT_DUT      = {.CS = INPUT_DUT,    .LAT = DUT_IACK};
FSM_state FSM_INPUT_RDY      = {.CS = INPUT_RDY,    .LAT = 0};
FSM_state FSM_OUTPUT_WAIT    = {.CS = OUTPUT_WAIT,  .LAT = MAX_LATENCY};
FSM_state FSM_OUTPUT_VAL     = {.CS = OUTPUT_VAL,   .LAT = 0};
FSM_state FSM_OUTPUT_PACK    = {.CS = OUTPUT_PACK,  .LAT = DUT_OLEN/32 + (DUT_OLEN%32 ? 1 : 0)};
FSM_state FSM_SCAN_PREP      = {.CS = SCAN_PREP,    .LAT = DFT_IACK};
FSM_state FSM_SCAN           = {.CS = SCAN,         .LAT = DUMP_NBR*33};
FSM_state FSM_SCAN_RD        = {.CS = SCAN_RD,      .LAT = 0};
FSM_state FSM_TICK           = {.CS = TICK,         .LAT = 1};

void dft_scan(RF* regfile, int dump_nbr)
{
  for (int i = 0; i < dump_nbr; i++)
    regfile->dft_out_ptr[i] = _dut_value[i];
}

void rf_update(RF* regfile, int* cycle_cnt)
{
  switch ((regfile->state) & WRAPPER_FSM_CS) {
  
  // ***********TEMPLATE*************
  /*
  case CURRENT_STATE:
    if (*cycle_cnt < FSM_CURRENT_STATE.LAT)  // do not trigger state transition
      *cycle_cnt += IO_LATENCY;
    else {                                   // trigger state transition
      *cycle_cnt -= FSM_CURRENT_STATE.LAT;
      operations;
      next_state_logic;
    }
    break;
  */

  case IDLE:
    if (*cycle_cnt < FSM_IDLE.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_IDLE.LAT; 
      if(regfile->opcode == INPUT)      
        regfile->state = INPUT_FLATTEN;
    }    
    break;

  case INPUT_FLATTEN:
    if (*cycle_cnt < FSM_INPUT_FLATTEN.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_INPUT_FLATTEN.LAT; 
      regfile->state = INPUT_DUT;
    }    
    break;
  
  case INPUT_DUT:
    if (*cycle_cnt < FSM_INPUT_DUT.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_INPUT_DUT.LAT;
      _dut_value[0] = regfile->dut_in_ptr[0]; 
      _dut_state[0] = 0;
      regfile->state = INPUT_RDY;
    }    
    break;

  case INPUT_RDY:
    if (*cycle_cnt < FSM_INPUT_RDY.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_INPUT_RDY.LAT;
      if(regfile->opcode == RUN) 
        regfile->state = OUTPUT_WAIT;
      else if (regfile->opcode == TEST) {
        regfile->state = SCAN_PREP;
      }
    }      
    break;
  
  case OUTPUT_WAIT:
    if (*cycle_cnt < FSM_OUTPUT_WAIT.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_OUTPUT_WAIT.LAT;
      dut_run(_dut_value);
      regfile->state = OUTPUT_VAL;
    }      
    break;
  
  case OUTPUT_VAL:
    if (*cycle_cnt < FSM_OUTPUT_VAL.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_OUTPUT_VAL.LAT;
      regfile->state = OUTPUT_PACK;
    }      
    break;
  
  case OUTPUT_PACK:
    if (*cycle_cnt < FSM_OUTPUT_PACK.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_OUTPUT_PACK.LAT;
      regfile->dut_out_ptr[0] = _dut_value[0];
      regfile->state = IDLE;
    }      
    break;
  
  case SCAN_PREP:
    if (*cycle_cnt < FSM_SCAN_PREP.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_SCAN_PREP.LAT;
      regfile->state = SCAN;
    }      
    break;
  
  case SCAN:
    if (*cycle_cnt < FSM_SCAN.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_SCAN.LAT;
      dft_scan(regfile,DUMP_NBR);
      regfile->state = SCAN_RD;
    }      
    break;
  
  case SCAN_RD:
    if(*cycle_cnt < FSM_SCAN_RD.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_SCAN_RD.LAT;
      if(regfile->opcode == ENDT) 
        regfile->state = IDLE;
      else if(regfile->opcode == NEXT) 
        regfile->state = TICK; 
      }
    break;

  case TICK:
    if(*cycle_cnt < FSM_TICK.LAT)
      (*cycle_cnt) +=  IO_LATENCY;
    else {
      *cycle_cnt -= FSM_TICK.LAT;
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
}

u32* addr_map(RF* regfile, u32 addr)
{
  // locating register files
  if (DUT_IN_BASE <= addr && addr < DFT_OUT_BASE + 64) {
    if(DUT_IN_BASE <= addr && addr < DUT_IN_BASE + 8)
      return regfile->dut_in_ptr + addr - DUT_IN_BASE;
    else if (DUT_OUT_BASE <= addr && addr < DUT_OUT_BASE + 8)
      return regfile->dut_out_ptr + addr - DUT_OUT_BASE;
    else
      return regfile->dft_out_ptr + addr - DFT_OUT_BASE;
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

u32 DUFT_ap_ctrl_chain(u32 addr, u32 wr_data, u32 rd_wr)
{
  u32* ptr;
  ptr = addr_map(&_rf,addr);
  if(rd_wr) {
    rf_update(&_rf,&cycle_cnt);
    return *ptr;
  }    
  else {
    *ptr = wr_data;
    rf_update(&_rf,&cycle_cnt);
    return 0;
  }   
}