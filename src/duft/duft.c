#include "common.h"
#include "DUFT_ap_ctrl_chain.h"
#include "wrapper_constants.h"

//-------------------------------------------------------------------------------
// Top level
//-------------------------------------------------------------------------------

int DUFT(u32 addr, u32 wr_data, u32 rd_wr)
{
  return DUFT_ap_ctrl_chain(addr, wr_data , rd_wr);
}