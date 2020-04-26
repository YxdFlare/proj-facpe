`timescale 1ns / 100ps
`include "./prewrapped_design.v"

module DUFT_ap_ctrl_chain(
  input [31:0]      addr,
  input [31:0]      wr_data,
  input             rd_wr, // 1 for read, 0 for write

  output reg [31:0] ap_return,
  input             ap_rst,
  input             ap_start,
  input             ap_continue,
  input             ap_ce,
  output reg        ap_idle,
  output reg        ap_ready,
  output reg        ap_done,

  input             ap_clk
);
//------------------------------------------------------------------
// datapath
//------------------------------------------------------------------

wire [31:0] core_return;
wire [31:0] core_rd_addr;
wire [31:0] core_wr_addr;
reg [31:0]  core_addr;
reg [31:0]  core_wr_data;
reg         core_rd_wr;
reg         return_0;
reg         reg_en_in;
reg         reg_en_out;

// input register
always @(posedge ap_clk) begin
  if(ap_rst) begin
    core_addr <= 32'hFFFFFFFF;
    core_wr_data <= 0;
    core_rd_wr <= 0;
  end
  else if(reg_en_in) begin
    core_addr <= addr;
    core_wr_data <= wr_data;
    core_rd_wr <= rd_wr;
  end
end


// output register
always @(posedge ap_clk) begin
  if(ap_rst) begin
    ap_return <= 32'd0;
  end
  else if(reg_en_out) begin
    ap_return <= return_0 ? 32'd0 : core_return;
  end
end

// DUFT core
prewrapped core_DUFT (
  .clk(ap_clk),
  .reset(ap_rst),
  .axi_rd_addr(core_rd_addr),
  .axi_wr_addr(core_wr_addr),
  .axi_rd_msg(core_return),
  .axi_wr_msg(core_wr_data)
);

//------------------------------------------------------------------
// control logic
//------------------------------------------------------------------

// read or write need 1 cycle
reg rden;
reg wren;
assign core_rd_addr = (rden && ap_ce) ? core_addr : 32'hFFFFFFFF;
assign core_wr_addr = (wren && ap_ce) ? core_addr : 32'hFFFFFFFF;

localparam y = 1'b1;
localparam n = 1'b0;

// state machine
// state def
localparam IDLE     = 3'd0;
localparam ACK      = 3'd1;
localparam RD_ING   = 3'd2;
localparam RD_DONE  = 3'd3;
localparam WR_ING   = 3'd4;
localparam WR_DONE  = 3'd5;

// next state logic
reg [2:0] current_state;
reg [2:0] next_state;
always @(posedge ap_clk) begin
  if(ap_rst)
    current_state <= IDLE;
  else
    current_state <= next_state;
end

always @(*) begin
  case(current_state)
    IDLE:
      if(ap_start)
        next_state = ACK;
      else
        next_state = IDLE;
    ACK:
      if(rd_wr)
        next_state = RD_ING;
      else
        next_state = WR_ING;
    RD_ING:
      next_state = RD_DONE;
    RD_DONE:
      if(!ap_continue)
        next_state = RD_DONE;
      // else if(ap_start)
      //   next_state = ACK;
      else
        next_state = IDLE;
    WR_ING:
      next_state = WR_DONE;
    WR_DONE:
      if(!ap_continue)
        next_state = WR_DONE;
      // else if(ap_start)
      //   next_state = ACK;
      else
        next_state = IDLE;
    default:
      next_state = IDLE;
  endcase
end

// output logic
task cbout;
  input c_ap_idle;
  input c_ap_ready;
  input c_ap_done;
  input c_rden;
  input c_wren;
  input c_reg_en_in;
  input c_reg_en_out;
  input c_return_0;
  begin
    ap_idle = c_ap_idle;
    ap_ready = c_ap_ready;
    ap_done = c_ap_done;
    rden = c_rden;
    wren = c_wren;
    reg_en_in = c_reg_en_in;
    reg_en_out = c_reg_en_out;
    return_0 = c_return_0;
  end
endtask

always @(*) begin
  case (current_state)
    //                     ap    ap   ap    rd   wr  reg  reg  ret
    //                     idle  rdy  done  en   en  in   out  0
  IDLE: if(ap_rst)    cbout(n,   n,   n,    n,   n,  n,   n,   n);
  else  if(ap_start)  cbout(n,   n,   n,    n,   n,  n,   n,   n);
  else                cbout(y,   n,   n,    n,   n,  n,   n,   n);
  ACK:                cbout(n,   n,   n,    n,   n,  y,   n,   n);
  RD_ING:             cbout(n,   n,   n,    y,   n,  n,   y,   n);
  RD_DONE:            cbout(n,   y,   y,    n,   n,  n,   n,   n);
  WR_ING:             cbout(n,   n,   n,    n,   y,  n,   y,   y);
  WR_DONE:            cbout(n,   y,   y,    n,   n,  n,   n,   n);
  default:            cbout(n,   n,   n,    n,   n,  n,   n,   n);
  endcase
end

endmodule // DUFT_ap_ctrl_hs