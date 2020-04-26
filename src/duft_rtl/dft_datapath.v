`timescale 1ns / 100ps
`include "./SIPObuf.v"
`include "./ScanReg.v"

module dft_datapath(
  input clk,
  input reset,
  input dft_sin,
  output [31:0] dft_out,
  

  input sc_sen,
  input buf_op,
  input buf_val_op,
  input buf_sin_sel,
  output buf_op_commit,
  output buf_scaning,
  output buf_op_ack
  
);
wire [31:0] buf_pout;
wire buf_sin;
assign buf_sin = buf_sin_sel ? dft_sin : 1'b0;
SIPO_buf_256B sipo_buf (
    .clk(clk), 
    .reset(reset), 
    .sin(buf_sin), 
    .pout(buf_pout), 
    .val_op(buf_val_op),
    .scaning(buf_scaning),
    .op(buf_op), 
    .op_ack(buf_op_ack), 
    .op_commit(buf_op_commit)
    );

ScanReg32 outputReg (
  .d(buf_pout),
	.q(dft_out),
	.sin(1'b0),
	.sout(),
	.sen(1'b0),
	.clk(clk),
	.clr(reset),
	.ce(buf_op_commit)
);


endmodule // dft
