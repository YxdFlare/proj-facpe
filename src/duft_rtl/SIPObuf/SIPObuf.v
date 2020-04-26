`timescale 1ns / 100ps
`include "./SIPObuf_ctrl.v"
`include "./SIPObuf_datapath.v"

module SIPO_buf_256B(
  input clk,
  input reset,
  
  input sin,
  output [31:0] pout,

  input val_op,
  input op,
  output scaning,
  output op_ack,
  output op_commit
  
);
// Instantiate the control
SIPO_buf_256B_ctrl ctrl (
    .clk(clk), 
    .reset(reset), 
    .val_op(val_op), 
    .op(op), 
    .op_ack(op_ack), 
    .op_commit(op_commit),
    .scaning(scaning), 
    .addrclr(addrclr), 
    .sftregclr(sftregclr), 
    .sften(sften), 
    .cnten(cnten), 
    .mem_wen(mem_wen), 
    .mem_ren(mem_ren)
    );
    
// Instantiate the datapath
SIPO_buf_256B_datapath datapath (
    .sin(sin), 
    .pout(pout), 
    .clk(clk), 
    .reset(reset), 
    .addrclr(addrclr), 
    .sftregclr(sftregclr), 
    .sften(sften), 
    .cnten(cnten), 
    .mem_wen(mem_wen), 
    .mem_ren(mem_ren)
    );


endmodule // SIPO_buf_256B