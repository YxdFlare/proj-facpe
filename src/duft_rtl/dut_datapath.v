`timescale 1ns / 100ps
`include "./ScanReg.v"

module IAS_datapath (
  input [31:0] data_in,
  output [31:0] data_out,
  input clk,
  input reset,

  input scan_ce,
  input reg_en,
  input reg_sel,

  input sin,
  output sout,
  input sen
  
);
  wire [31:0] data_in_fb;
  wire [31:0] data_in_reg;
  
  assign ce = sen ? scan_ce : reg_en;

  ScanReg32 inputreg (
    .d(data_in_reg),
    .q(data_out),
    .sin(sin),
    .sout(sout),
    .sen(sen),
    .clk(clk),
    .clr(reset),
    .ce(ce)
  );

  assign data_in_fb = data_out + 1;
  assign data_in_reg = reg_sel ? data_in : data_in_fb;

  
endmodule // 