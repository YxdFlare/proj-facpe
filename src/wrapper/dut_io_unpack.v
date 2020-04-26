`timescale 1ns / 100ps
`include "./reg_and_regfile.v"

module dut_io_unpack
#(
  parameter dut_input_width = 256,
  parameter dut_output_width = 256
)(
  // control signals
  input [31:0] dut_output_vec_addr,
  input [31:0] dut_input_vec_addr,
  input input_vec_en,
  input input_vec_mode,
  input output_vec_en,
  input output_vec_mode,

  // input interface
  input [31:0] dut_input_vec_from_axi,
  output [dut_input_width-1:0] dut_input_vec_to_dut,

  // output interface
  input [dut_output_width-1:0] dut_output_vec_from_dut,
  output [31:0] dut_output_vec_to_axi,

  // system signal
  input clk,
  input reset  
);
  localparam dut_input_words = (dut_input_width % 32 == 0) ? (dut_input_width / 32 ) : (dut_input_width / 32 + 1);  
  localparam dut_output_words = (dut_output_width % 32 == 0) ? (dut_output_width / 32 ) : (dut_output_width / 32 + 1);
  //localparam dut_input_lastword = dut_input_width - (dut_input_words-1)*32;
  //localparam dut_output_lastword = dut_output_width - (dut_output_words-1)*32;

  wire [32*dut_input_words-1:0] input_word_lanes_untrimmed;
  reg [32*dut_output_words-1:0] output_word_lanes_puffered;

  assign dut_input_vec_to_dut = input_word_lanes_untrimmed[dut_input_width-1:0];
  always @* output_word_lanes_puffered = {(32*dut_output_words-dut_output_width)*1'b0,dut_output_vec_from_dut};

  buf_word_expand #(dut_input_words) input_vec_unpack
  (
    .word_stream(dut_input_vec_from_axi),
    .word_lanes(input_word_lanes_untrimmed),
    .addr(dut_input_vec_addr),
    .en(input_vec_en),
    .mode(input_vec_mode),
    .clk(clk),
    .reset(reset)
  );

  buf_word_contract #(dut_output_words) output_vec_pack
  (
    .word_stream(dut_output_vec_to_axi),
    .word_lanes(output_word_lanes_puffered),
    .addr(dut_output_vec_addr),
    .en(output_vec_en),
    .mode(output_vec_mode),
    .clk(clk),
    .reset(reset)
  );

endmodule // dut_io_unpack