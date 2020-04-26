`timescale 1ns / 100ps
`include "./dut_ctrl.v"
`include "./dut_datapath.v"

module incr_decr(
	input [31:0] data_in,
  output [31:0] data_out,
  input clk,
  input reset,

  input val_op,
  output op_ack,
  output op_commit,
  input commit_ack,
  
  input sen,
  input scan_ce,
  output sout,
  input sin
); 
  IAS_datapath datapath (
    .data_in(data_in), 
    .data_out(data_out), 
    .clk(clk), 
    .reset(reset), 
    .reg_en(reg_en), 
    .reg_sel(reg_sel), 
    .sin(sin), 
    .sout(sout), 
    .sen(sen),
    .scan_ce(scan_ce)
    );
 
  IAS_ctrl ctrl (
    .clk(clk), 
    .reset(reset), 
    .val_op(val_op), 
    .op_commit(op_commit),
    .op_ack(op_ack),
    .commit_ack(commit_ack),
    .reg_en(reg_en), 
    .reg_sel(reg_sel), 
    .sen(sen)
    );

endmodule
