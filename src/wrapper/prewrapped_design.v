`timescale 1ns / 100ps
`include "./prewrapper.v"
`include "./duft_top.v"

module prewrapped(
  input clk,
  input reset,

  input [31:0] axi_rd_addr,
  input [31:0] axi_wr_addr,
  output [31:0] axi_rd_msg,
  input [31:0] axi_wr_msg
);
  wire [31:0] data_in;
  wire [31:0] data_out;
  wire [31:0] dft_out;
//----------------------------------------------------
// pre-wrapper
//----------------------------------------------------
  axi_prewrapper #(
    .p_sc_nbr(1),
    .dut_input_width(32),
    .dut_output_width(32)
  ) prewrapper (
    // system signal
    .clk(clk),
    .reset(reset),
    
    // AXI side interface
    .axi_rd_addr(axi_rd_addr),
    .axi_wr_addr(axi_wr_addr),
    .axi_rd_msg(axi_rd_msg),
    .axi_wr_msg(axi_wr_msg),

    // design under test (dut) interface
    .dut_input_vec(data_in),
    .dut_output_vec(data_out),

    .dut_op_ack(dut_op_ack),
    .dut_val_op(dut_val_op),
    .dut_op_commit(dut_op_commit),
    .dut_commit_ack(dut_commit_ack),
    .dut_sen(ex_sen),
    .dut_rst(dut_rst),

    // design for test (dft) interface, max 16 scan chains
    .dft_output_data(dft_out),

    .dft_op_ack(dft_op_ack),
    .dft_val_op(dft_val_op),
    .dft_op_commit(dft_op_commit),
    .dft_output_strobe(dft_out_strobe),
    .dft_commit_ack(dft_commit_ack)
  );
  
  //----------------------------------------------------
  // duft with 1 dft channel
  //----------------------------------------------------

  dft_top #(
    .chain_len(32),
    .dump_nbr(27'd1)
  ) duft (
    .data_in(data_in),
    .data_out(data_out),

    .dut_val_op(dut_val_op),
    .dut_op_ack(dut_op_ack),
    .dut_op_commit(dut_op_commit),
    .dut_commit_ack(dut_commit_ack),

    .dft_out(dft_out),

    .dft_val_op(dft_val_op),
    .dft_op_ack(dft_op_ack),
    .dft_op_commit(dft_op_commit),
    .dft_out_strobe(dft_out_strobe),
    .dft_commit_ack(dft_commit_ack),
    .ex_sen(ex_sen),

    .clk(clk),
    .reset(reset || dut_rst)
  );


endmodule // 