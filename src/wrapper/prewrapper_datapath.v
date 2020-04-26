`timescale 1ns / 100ps
//----------------------------------------------------
// datapath
//----------------------------------------------------
`include "./reg_and_regfile.v"
`include "./dut_io_unpack.v"
module axi_prewrapper_datapath
#(
  parameter p_sc_nbr = 16,
  parameter dut_input_width = 256,
  parameter dut_output_width = 256
)(
  // AXI side interface
  input [31:0] axi_rd_addr,
  input [31:0] axi_wr_addr,
  output [31:0] axi_rd_msg,
  input [31:0] axi_wr_msg,

  // design under test (dut) interface
  output [dut_input_width-1:0] dut_input_vec,
  input [dut_output_width-1:0] dut_output_vec,

  // design for test (dft) interface, max 16 scan chains
  input [32*p_sc_nbr-1:0] dft_output_data,
  
  // control unit signals         
  input [p_sc_nbr+5-1:0] rf_ren,  
  input [p_sc_nbr+5-1:0] rf_wen,  
  
  input [31:0] dut_input_vec_rdaddr,
  input [31:0] dut_output_vec_wraddr,
  input dut_input_vec_en,
  input dut_input_vec_mode,
  input dut_output_vec_en,
  input dut_output_vec_mode,

  input [32*p_sc_nbr-1:0] dft_output_data_wraddr,

  input [31:0] ctrl_state,
  output [31:0] ctrl_opcode,
  output [31:0] ctrl_config,

  // system signals
  input clk,
  input reset
);
  //  name  ||   dft_output_data_reg  |  dut_output_vec_reg | dut_input_vec_reg  |   config reg    |    state reg    |   opcode reg    ||
  //  index ||? <-----p_sc_nbr-----> 5|          4          |          3         |        2        |         1       |         0       ||
  // writer ||     write from dft     |    write from dut   |  write from axi    | write from axi  | write from ctrl | write from axi  ||
  // reader ||       read by axi      |      read by axi    |    read by dut     |   read by ctrl  |   read by axi   |   read by ctrl  ||
  //  base  ||       0x00000020       |     0x00000018      |      0x00000010    |   0x00000002    |   0x00000001    |   0x00000000    ||
  
  // address mapping
  localparam OPCODE_BASE  = 32'h00000000;
  localparam STATE_BASE   = 32'h00000001;
  localparam CONFIG_BASE  = 32'h00000002;
  localparam DUT_IN_BASE  = 32'h00000010;
  localparam DUT_OUT_BASE = 32'h00000018;
  localparam DFT_OUT_BASE = 32'h00000020;
  localparam TEST_IN_BASE = 32'hFF000000;
  localparam TEST_OUT_BASE= 32'hFF000001;

  //--------------------------------------------------------
  // interface registers
  //--------------------------------------------------------

  // test register
  wire [31:0] data_loop;
  BSRSW_RF32 #(TEST_IN_BASE,1) test_in_reg
  (
  .rdaddr(TEST_IN_BASE),
  .wraddr(axi_wr_addr),
  .rddata(data_loop),
  .wrdata(axi_wr_msg),
  .wen(1'b1),
  .ren(1'b1),
  .clk(clk),
  .reset(reset)
  );

  BSRSW_RF32 #(TEST_OUT_BASE,1) test_out_reg
  (
  .rdaddr(axi_rd_addr),
  .wraddr(TEST_OUT_BASE),
  .rddata(axi_rd_msg),
  .wrdata(data_loop),
  .wen(1'b1),
  .ren(1'b1),
  .clk(clk),
  .reset(reset)
  );

  // opcode register (AXI -> reg -> ctrl)
  BSRSW_RF32 #(OPCODE_BASE,1) opcode_reg
  (
  .rdaddr(OPCODE_BASE),
  .wraddr(axi_wr_addr),
  .rddata(ctrl_opcode),
  .wrdata(axi_wr_msg),
  .wen(rf_wen[0]),
  .ren(rf_ren[0]),
  .clk(clk),
  .reset(reset)
  );

  // state register (ctrl -> reg -> AXI)
  BSRSW_RF32 #(STATE_BASE,1) state_reg
  (
  .rdaddr(axi_rd_addr),
  .wraddr(STATE_BASE),
  .rddata(axi_rd_msg),
  .wrdata(ctrl_state),
  .wen(rf_wen[1]),
  .ren(rf_ren[1]),
  .clk(clk),
  .reset(reset)
  );

  // config register (AXI -> reg -> ctrl)
  BSRSW_RF32 #(CONFIG_BASE,1) config_reg
  (
  .rdaddr(CONFIG_BASE),
  .wraddr(axi_wr_addr),
  .rddata(ctrl_config),
  .wrdata(axi_wr_msg),
  .wen(rf_wen[2]),
  .ren(rf_ren[2]),
  .clk(clk),
  .reset(reset)
  );

  wire [31:0] dut_inpv_packed;
  wire [31:0] dut_outv_packed;
  // dut input vector register (AXI -> reg -> dut)
  BSRSW_RF32 #(DUT_IN_BASE,8) dut_input_vec_reg
  (
  .rdaddr(dut_input_vec_rdaddr + DUT_IN_BASE),
  .wraddr(axi_wr_addr),
  .rddata(dut_inpv_packed),
  .wrdata(axi_wr_msg),
  .wen(rf_wen[3]),
  .ren(rf_ren[3]),
  .clk(clk),
  .reset(reset)
  );

  // dut output vector register (dut -> reg -> AXI)
  BSRSW_RF32 #(DUT_OUT_BASE,8) dut_output_vec_reg
  (
  .rdaddr(axi_rd_addr),
  .wraddr(dut_output_vec_wraddr + DUT_OUT_BASE),
  .rddata(axi_rd_msg),
  .wrdata(dut_outv_packed),
  .wen(rf_wen[4]),
  .ren(rf_ren[4]),
  .clk(clk),
  .reset(reset)
  );

  // dft output register (dft -> reg -> AXI)
  genvar i;
  generate
  for (i = 0;i < p_sc_nbr;i = i + 1) begin
    BSRSW_RF32 #(DFT_OUT_BASE + 64*i,64) dft_output_reg
    (
    .rdaddr(axi_rd_addr),
    .wraddr(dft_output_data_wraddr[32*i+31:32*i] + DFT_OUT_BASE),
    .rddata(axi_rd_msg),
    .wrdata(dft_output_data[32*i+31:32*i]),
    .wen(rf_wen[5+i]),
    .ren(rf_ren[5+i]),
    .clk(clk),
    .reset(reset)
  );
  end
  endgenerate

  //--------------------------------------------------------
  // dut input/output vector unpack
  //--------------------------------------------------------

  dut_io_unpack #(dut_input_width,dut_output_width) io_unpack
  (
  // control signals
  .dut_output_vec_addr(dut_output_vec_wraddr),
  .dut_input_vec_addr(dut_input_vec_rdaddr),

  // input interface
  .dut_input_vec_from_axi(dut_inpv_packed),
  .dut_input_vec_to_dut(dut_input_vec),

  // output interface
  .dut_output_vec_from_dut(dut_output_vec),
  .dut_output_vec_to_axi(dut_outv_packed),

  // control signal
  .input_vec_en(dut_input_vec_en),
  .input_vec_mode(dut_input_vec_mode),
  .output_vec_en(dut_output_vec_en),
  .output_vec_mode(dut_output_vec_mode),

  // system signal
  .clk(clk),
  .reset(reset)  
  );

endmodule // axi_prewarpper_datapath