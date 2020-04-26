`timescale 1ns / 100ps
module dft_ctrl(
  input clk,
  input reset,

  input val_op,
  output op_ack,
  output op_commit,
  input commit_ack,
  output reg dft_out_strobe,
  input [31:0] chain_len,
  input [26:0] dump_nbr,

  output sc_sen,
  output reg sc_ce,
  output buf_op,
  output buf_sin_sel,
  output buf_val_op,
  output buf_reset,
  input buf_op_commit,
  input buf_scaning,
  input buf_op_ack
);

// state def
localparam IDLE = 4'd0;
localparam SCAN_PREP = 4'd1;
localparam SCAN = 4'd2;
localparam DUMP_PREP = 4'd3;
localparam DUMP = 4'd4;
localparam FINISH = 4'd5;
localparam SCAN_BUF_RST = 4'd6;
localparam DUMP_BUF_RST = 4'd7;
localparam DUMP_BUF_RST_2 = 4'd8;

reg op_ack_REG, op_commit_REG, sc_sen_REG, buf_reset_REG, buf_op_REG, buf_val_op_REG, buf_sin_sel_REG, sc_clr, dump_cnten, sc_cnten;
reg dft_out_strobe_preg;

// scan counter (count for chain length)
wire [31:0] sc_cnt_value;
arbitrary_counter32 scan_cnt (
    .cnt_max(chain_len), 
    .cnt_min(32'd0), 
    .cnt_value(sc_cnt_value), 
    .clr(sc_clr), 
    .inc_dec(1'b1), 
    .cnten(sc_cnten), 
    .clk(clk)
    );
wire sc_cntov;
assign sc_cntov = (sc_cnt_value == chain_len) ? 1'b1 : 1'b0;

// dump counter (count for chain_len/32 + 1)
wire [31:0] dump_cnt_value;
reg dump_clr;
arbitrary_counter32 dump_cnt (
    .cnt_max({5'd0,dump_nbr}), 
    .cnt_min(32'd0), 
    .cnt_value(dump_cnt_value), 
    .clr(dump_clr), 
    .inc_dec(1'b1), 
    .cnten(dump_cnten), 
    .clk(clk)
    );
wire dump_cntov;
assign dump_cntov = (dump_cnt_value == dump_nbr) ? 1'b1 : 1'b0;

// state registers
reg [3:0] current_state;
reg [3:0] next_state;

// state transition
always @(posedge clk ) begin
  if (reset)
    current_state <= IDLE;
  else 
    current_state <= next_state;
end

// next state logic
always @(*) begin
  case (current_state)
    IDLE:
      if (val_op)
        next_state = SCAN_BUF_RST;
      else 
        next_state = IDLE;
    SCAN_BUF_RST:
      next_state = SCAN_PREP;
    SCAN_PREP:
      if (sc_cntov)
        next_state = DUMP_PREP;
      else if (!buf_op_ack)
        next_state = SCAN_PREP;
      else
        next_state = SCAN;
    SCAN:
      if (sc_cntov && buf_op_commit)
        next_state = DUMP_BUF_RST;
      else if (sc_cntov)
        next_state = DUMP_BUF_RST_2;
      else if (!buf_op_commit)
        next_state = SCAN;
      else
        next_state = SCAN_PREP;
    DUMP_BUF_RST:
      next_state = DUMP_PREP;
    DUMP_BUF_RST_2:
      if (buf_op_commit)
        next_state = DUMP_PREP;
      else 
        next_state = DUMP_BUF_RST;
    DUMP_PREP:
      if (dump_cntov)
        next_state = FINISH;
      else if (!buf_op_ack)
        next_state = DUMP_PREP;
      else
        next_state = DUMP;
    DUMP:
      if (!buf_op_commit)
        next_state = DUMP;
      else 
        next_state = DUMP_PREP;
    FINISH:
      if (commit_ack)
        next_state = IDLE;
      else  
        next_state = FINISH;
    default:
      next_state = IDLE;
  endcase
end

// output logic


assign op_ack = op_ack_REG;
assign op_commit = op_commit_REG;
assign sc_sen = sc_sen_REG;
assign buf_reset = buf_reset_REG;
assign buf_val_op = buf_val_op_REG;
assign buf_sin_sel = buf_sin_sel_REG;
assign buf_op = buf_op_REG;

localparam y = 1'b1;
localparam n = 1'b0;
localparam wr = 1'b0;
localparam rd = 1'b1;
localparam dut = 1'b1;
localparam zero = 1'b0;

task cbout(op_ack_cb, op_commit_cb, dft_out_strobe_cb, sc_sen_cb, buf_reset_cb, buf_op_cb, buf_val_op_cb, buf_sin_sel_cb, dump_clr_cb, sc_clr_cb, dump_cnten_cb, sc_ce_cb, sc_cnten_cb);
begin
  op_ack_REG = op_ack_cb;
  op_commit_REG = op_commit_cb;
  sc_sen_REG = sc_sen_cb;
  buf_reset_REG = buf_reset_cb;
  buf_val_op_REG = buf_val_op_cb;
  buf_op_REG = buf_op_cb;
  buf_sin_sel_REG = buf_sin_sel_cb;
  dump_clr = dump_clr_cb;
  dump_cnten = dump_cnten_cb;
  sc_clr = sc_clr_cb;
  sc_cnten = sc_cnten_cb;
  sc_ce = buf_scaning;
  dft_out_strobe_preg = dft_out_strobe_cb;
end
endtask

always @(*) begin
  case (current_state)
                                        //    op   op     out     sc   buf  buf   buf    buf     dump sc   dump  sc sc
                                        //    ack  commit strobe  sen  rst  op    val_op sinsel  clr  clr  +1    ce +1
    IDLE:                               cbout(n,   n,     n,      n,   y,   rd,   n,     zero,   y,   y,   n,    n, n);
    SCAN_BUF_RST:                       cbout(y,   n,     n,      y,   y,   wr,   n,     zero,   n,   n,   n,    n, n);
    SCAN_PREP:  if (sc_cntov)           cbout(n,   n,     n,      y,   n,   wr,   n,     zero,   n,   n,   n,    n, n);
                else                    cbout(n,   n,     n,      y,   n,   wr,   y,     zero,   n,   n,   n,    n, n);
    SCAN:       if (sc_cntov)           cbout(n,   n,     n,      y,   n,   wr,   n,     zero,   n,   n,   n,    n, n); // completed a full-cycle, disengage scan chain, disable sen
                else if (buf_op_commit) cbout(n,   n,     n,      y,   n,   wr,   n,     dut,    n,   n,   n,    n, n); // completed a 32-bit scan, not full-cycle yet
                else                    cbout(n,   n,     n,      y,   n,   wr,   n,     dut,    n,   n,   n,    n, y); // scaning
    DUMP_BUF_RST:                       cbout(n,   n,     n,      y,   y,   rd,   n,     zero,   y,   n,   n,    n, n);
    DUMP_BUF_RST_2: if (buf_op_commit)  cbout(n,   n,     n,      y,   y,   rd,   n,     zero,   y,   n,   n,    n, n);
                else                    cbout(n,   n,     n,      y,   n,   wr,   n,     zero,   n,   n,   n,    n, n);
    DUMP_PREP:  if (dump_cntov)         cbout(n,   n,     n,      y,   n,   rd,   n,     zero,   n,   n,   n,    n, n);
                else                    cbout(n,   n,     n,      y,   n,   rd,   y,     zero,   n,   n,   n,    n, n);
    DUMP:       if (buf_op_commit)      cbout(n,   n,     y,      y,   n,   rd,   n,     zero,   n,   n,   y,    n, n); 
                else                    cbout(n,   n,     n,      y,   n,   rd,   n,     zero,   n,   n,   n,    n, n);
    FINISH:                             cbout(n,   y,     n,      y,   n,   rd,   n,     zero,   n,   n,   n,    n, n);
    default:                            cbout(n,   n,     n,      n,   n,   rd,   n,     zero,   y,   y,   n,    n, n);
  endcase
end

always @(posedge clk ) begin
  dft_out_strobe <= dft_out_strobe_preg;
end

endmodule // dft_ctrl