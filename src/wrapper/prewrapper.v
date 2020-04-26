`timescale 1ns / 100ps
//----------------------------------------------------
// top-level
//----------------------------------------------------
`include "./prewrapper_ctrl.v"
`include "./prewrapper_datapath.v"
module axi_prewrapper 
#(
  parameter p_sc_nbr = 16,
  parameter dut_input_width = 256,
  parameter dut_output_width = 256
)(
  // system signal
  input clk,
  input reset,
  
  // AXI side interface
  input [31:0] axi_rd_addr,
  input [31:0] axi_wr_addr,
  output [31:0] axi_rd_msg,
  input [31:0] axi_wr_msg,

  // design under test (dut) interface
  output [dut_input_width-1:0] dut_input_vec,
  input [dut_output_width-1:0] dut_output_vec,

  input dut_op_ack,
  output dut_val_op,
  input dut_op_commit,
  output dut_commit_ack,
  output dut_sen,
  output dut_rst,

  // design for test (dft) interface, max 16 scan chains
  input [32*p_sc_nbr-1:0] dft_output_data,

  input [p_sc_nbr-1:0] dft_op_ack,
  output [p_sc_nbr-1:0] dft_val_op,
  input [p_sc_nbr-1:0] dft_op_commit,
  input [p_sc_nbr-1:0] dft_output_strobe,
  output [p_sc_nbr-1:0] dft_commit_ack
);
  wire [p_sc_nbr+5-1:0] rf_ren;  
  wire [p_sc_nbr+5-1:0] rf_wen;  
  
  wire [31:0] dut_input_vec_rdaddr;
  wire [31:0] dut_output_vec_wraddr;
  wire dut_input_vec_en;
  wire dut_input_vec_mode;
  wire dut_output_vec_en;
  wire dut_output_vec_mode;

  wire [32*p_sc_nbr-1:0] dft_output_data_wraddr;

  wire [31:0] ctrl_state;
  wire [31:0] ctrl_opcode;
  wire [31:0] ctrl_config;

  axi_prewrapper_datapath #(
    p_sc_nbr,
    dut_input_width,
    dut_output_width
  ) datapath (
    .axi_rd_addr(axi_rd_addr),
    .axi_wr_addr(axi_wr_addr),
    .axi_rd_msg(axi_rd_msg),
    .axi_wr_msg(axi_wr_msg),

    .dut_input_vec(dut_input_vec),
    .dut_output_vec(dut_output_vec),

    .dft_output_data(dft_output_data),

    .rf_ren(rf_ren),
    .rf_wen(rf_wen),
    .dut_input_vec_rdaddr(dut_input_vec_rdaddr),
    .dut_output_vec_wraddr(dut_output_vec_wraddr),
    .dut_input_vec_en(dut_input_vec_en),
    .dut_input_vec_mode(dut_input_vec_mode),
    .dut_output_vec_en(dut_output_vec_en),
    .dut_output_vec_mode(dut_output_vec_mode),

    .dft_output_data_wraddr(dft_output_data_wraddr),

    .ctrl_state(ctrl_state),
    .ctrl_opcode(ctrl_opcode),
    .ctrl_config(ctrl_config),

    .clk(clk),
    .reset(reset)
  );

  axi_prewrapper_ctrl #(
    p_sc_nbr,
    dut_input_width,
    dut_output_width
  ) ctrl (
    .dut_val_op(dut_val_op),
    .dut_op_ack(dut_op_ack),
    .dut_op_commit(dut_op_commit),
    .dut_commit_ack(dut_commit_ack),
    .dut_sen(dut_sen),
    .dut_rst(dut_rst),

    .dft_val_op(dft_val_op),
    .dft_op_ack(dft_op_ack),
    .dft_op_commit(dft_op_commit),
    .dft_output_strobe(dft_output_strobe),
    .dft_commit_ack(dft_commit_ack),

    .rf_ren(rf_ren),
    .rf_wen(rf_wen),
    .dut_input_vec_rdaddr(dut_input_vec_rdaddr),
    .dut_output_vec_wraddr(dut_output_vec_wraddr),
    .dut_input_vec_en(dut_input_vec_en),
    .dut_input_vec_mode(dut_input_vec_mode),
    .dut_output_vec_en(dut_output_vec_en),
    .dut_output_vec_mode(dut_output_vec_mode),

    .dft_output_data_wraddr(dft_output_data_wraddr),

    .ctrl_state(ctrl_state),
    .ctrl_opcode(ctrl_opcode),
    .ctrl_config(ctrl_config),

    .clk(clk),
    .reset(reset)
  );
    
endmodule // axi_prewrapper



