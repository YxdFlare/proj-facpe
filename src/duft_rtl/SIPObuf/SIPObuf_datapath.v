`timescale 1ns / 100ps
`include "./mem.v"
`include "./ScanReg.v"
`include "./counters.v"

module SIPO_buf_256B_datapath(
  input sin,
  output [31:0] pout,
  input clk,
  input reset,
  
  input addrclr,
  input sftregclr,
  input sften,
  input cnten,
  input mem_wen,
  input mem_ren

);
  wire [31:0] mem_din;
  wire [5:0] addr;
  wire [31:0] cnt_value;
  assign addr = cnt_value[5:0]; 

// memory unit
// 64 lines X 32bit/line
mem_256B memunit (
  .din(mem_din),
  .dout(pout),
  .addr(addr),
  .wen(mem_wen),
  .ren(mem_ren),
  .clk(clk),
  .reset(reset)
);
  
// input register
ScanReg32 inputreg(
	.d(32'd0),
	.q(mem_din),
	.sin(sin),
	.sout(),
	.sen(sften),
	.clk(clk),
	.clr(sftregclr),
	.ce(1'b1)
  );

// address counter
arbitrary_counter32 addrcounter (
  .cnt_min(32'd0),
  .cnt_max(32'd63),
  .cnt_value(cnt_value),
  .clr(addrclr),
  .inc_dec(1'b1),
  .cnten(cnten),
  .clk(clk)
);


endmodule // SIPO_buf_256B