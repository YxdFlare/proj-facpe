`timescale 1ns / 100ps
//----------------------------------------------------
// control unit
//----------------------------------------------------
`include "./counters.v"
module axi_prewrapper_ctrl
#(
  parameter p_sc_nbr = 16,
  parameter dut_input_width = 256,
  parameter dut_output_width = 256
)(
  // design under test (dut) interface
  output reg dut_val_op,
  input dut_op_ack,
  input dut_op_commit,
  output reg dut_commit_ack,
  output reg dut_sen,
  output reg dut_rst,

  // design for test (dft) interface
  output reg [p_sc_nbr-1:0] dft_val_op,
  input [p_sc_nbr-1:0] dft_op_ack,
  input [p_sc_nbr-1:0] dft_op_commit,
  input [p_sc_nbr-1:0] dft_output_strobe,
  output reg [p_sc_nbr-1:0] dft_commit_ack,

  // control-datapath signals
  output reg [p_sc_nbr+5-1:0] rf_ren,
  output reg [p_sc_nbr+5-1:0] rf_wen, 
  output [31:0] dut_input_vec_rdaddr,
  output [31:0] dut_output_vec_wraddr,
  output reg dut_input_vec_en,
  output reg dut_input_vec_mode,
  output reg dut_output_vec_en,
  output reg dut_output_vec_mode,

  output [32*p_sc_nbr-1:0] dft_output_data_wraddr,

  output [31:0] ctrl_state,
  input [31:0] ctrl_opcode,
  input [31:0] ctrl_config,

  // system signals
  input clk,
  input reset
);
  localparam y = 1'b1;
  localparam n = 1'b0;

  // state def
  localparam IDLE           = 4'd0;
  localparam INPUT_FLATTEN  = 4'd1;
  localparam INPUT_DUT      = 4'd2;
  localparam INPUT_RDY      = 4'd3;
  localparam OUTPUT_WAIT    = 4'd4;
  localparam OUTPUT_VAL     = 4'd5;
  localparam OUTPUT_PACK    = 4'd6;
  localparam SCAN_PREP      = 4'd7;
  localparam SCAN           = 4'd8;
  localparam SCAN_RD        = 4'd9;
  localparam TICK           = 4'd10;
  // state reg
  reg [3:0] current_state;
  reg [3:0] next_state;
  always @(posedge clk ) begin
      if (reset)
        current_state <= IDLE;
      else
        current_state <= next_state;
    end

  // state output fields
  assign ctrl_state = {
    20'd0,
    dft_val_op,dft_op_ack,dft_op_commit,dft_commit_ack,
    dut_val_op,dut_op_ack,dut_op_commit,dut_commit_ack,
    current_state
    };
  
  // opcode codebook
  localparam NONE  = 32'd0;
  localparam INPUT = 32'd1;
  localparam RUN   = 32'd2;
  localparam ENDR  = 32'd3;
  localparam TEST  = 32'd4;
  localparam NEXT  = 32'd5;
  localparam ENDT  = 32'd6;

  // next state transition signals
  wire flatten_rdy;
  reg flatten_en;
  reg flatten_clr;
  wire pack_rdy;
  reg pack_en;
  reg pack_clr;
   
  
  // input faltten counter
  localparam dut_input_words = (dut_input_width % 32 == 0) ? (dut_input_width / 32 ) : (dut_input_width / 32 + 1); 
  localparam faltten_cnt_max = dut_input_words - 1;
  simple_counter #(32,faltten_cnt_max) fltcnt (
    .value(dut_input_vec_rdaddr),
    .ov(flatten_rdy),
    .clr(flatten_clr),
    .cnten(flatten_en),
    .clk(clk),
    .reset(reset)
  );

  // output packing counter
  localparam dut_output_words = (dut_output_width % 32 == 0) ? (dut_output_width / 32 ) : (dut_output_width / 32 + 1);
  localparam packing_cnt_max = dut_output_words - 1;
  simple_counter #(32,packing_cnt_max) pckcnt (
    .value(dut_output_vec_wraddr),
    .ov(pack_rdy),
    .clr(pack_clr),
    .cnten(pack_en),
    .clk(clk),
    .reset(reset)
  );

  // dft out register waddr counter 
  reg [p_sc_nbr-1:0] dft_cnt_en;
  reg [p_sc_nbr-1:0] dft_cnt_clr;
  genvar i;
  for (i = 0;i < p_sc_nbr;i = i + 1) begin
    simple_counter #(32,32'hffffffff) dftoutcnt (
      .value(dft_output_data_wraddr[32*i+31:32*i]),
      .ov(),
      .clr(dft_cnt_clr[i]),
      .cnten(dft_cnt_en[i]),
      .clk(clk),
      .reset(reset)
    );
  end

  // next state logic
  always @(*) begin
    case (current_state)
      IDLE: 
        if (ctrl_opcode == NONE)
          next_state = IDLE;
        else if (ctrl_opcode == INPUT)
          next_state = INPUT_FLATTEN;
        else
          next_state = IDLE;

      INPUT_FLATTEN: 
        if (flatten_rdy)
          next_state = INPUT_DUT;
        else
          next_state = INPUT_FLATTEN;

      INPUT_DUT:
        if (dut_op_ack)
          next_state = INPUT_RDY;
        else
          next_state = INPUT_DUT;
      
      INPUT_RDY:
        if (ctrl_opcode == NONE)
          next_state = INPUT_RDY;
        else if (ctrl_opcode == RUN)
          next_state = OUTPUT_WAIT;
        else if (ctrl_opcode == TEST)
          next_state = SCAN_PREP;
        else
          next_state = INPUT_RDY;
      
      OUTPUT_WAIT:
        if (dut_op_commit)
          next_state = OUTPUT_VAL;
        else
          next_state = OUTPUT_WAIT;
      
      OUTPUT_VAL:
        if (ctrl_opcode == ENDR)
          next_state = OUTPUT_PACK;
        else
          next_state = OUTPUT_VAL;
      
      OUTPUT_PACK:
        if (pack_rdy)
          next_state = IDLE;
        else
          next_state = OUTPUT_PACK;

      SCAN_PREP:
        if (&dft_op_ack)
          next_state = SCAN;
        else
          next_state = SCAN_PREP;
      
      SCAN:
        if (&dft_op_commit)
          next_state = SCAN_RD;
        else
          next_state = SCAN;
      
      SCAN_RD:
        if (ctrl_opcode == ENDT)
          next_state = IDLE;
        else if (ctrl_opcode == NEXT)
          next_state = TICK;
        else
          next_state = SCAN_RD;
      
      TICK:
        next_state = SCAN_PREP;
      
      default: next_state = IDLE;
    endcase
  end

  // output logic
  localparam RST        = 13;
  localparam NOP        = 0;
  localparam VAL_OP     = 1;
  localparam CM_ACK     = 2;
  localparam RD         = 3;
  localparam WR         = 4;
  localparam CLR        = 5;
  localparam PLUS_1     = 6; 
  localparam HALT       = 7;
  localparam WR_PLUS_1  = 8; 
  localparam F_SIN      = 9;
  localparam F_POUT     = 10;
  localparam P_PIN      = 11;
  localparam P_SOUT     = 12;

  localparam SIN = 1'b1;
  localparam POUT = 1'b0;
  localparam PIN = 1'b1;
  localparam SOUT = 1'b0;

  localparam a_opcode = 0;
  localparam a_status = 1;
  localparam a_config = 2;
  localparam a_dutin  = 3;
  localparam a_dutout = 4;

  task cbout;
  input integer c_dut;
  input integer c_dft;
  input integer c_opcode;
  input integer c_status;
  input integer c_config;
  input integer c_dutin;
  input integer c_dutout;
  input integer c_flatten;
  input integer c_dut_inf;
  input integer c_pack;
  input integer c_dut_outp;
  input integer c_dftout;
  integer j;
    begin
      // dut transactions
      case (c_dut)
        VAL_OP: begin dut_val_op = 1'b1; dut_commit_ack = 1'b0; dut_sen = 1'b0; dut_rst = 1'b0; end
        CM_ACK: begin dut_val_op = 1'b0; dut_commit_ack = 1'b1; dut_sen = 1'b0; dut_rst = 1'b0; end
        HALT:   begin dut_val_op = 1'b0; dut_commit_ack = 1'b0; dut_sen = 1'b1; dut_rst = 1'b0; end
        RST:    begin dut_val_op = 1'b0; dut_commit_ack = 1'b0; dut_sen = 1'b0; dut_rst = 1'b0; end
        default:begin dut_val_op = 1'b0; dut_commit_ack = 1'b0; dut_sen = 1'b0; dut_rst = 1'b0; end
      endcase

      // dft transactions
      case (c_dft)
        VAL_OP: begin dft_val_op = {p_sc_nbr{1'b1}}; dft_commit_ack = {p_sc_nbr{1'b0}}; end
        CM_ACK: begin dft_val_op = {p_sc_nbr{1'b0}}; dft_commit_ack = {p_sc_nbr{1'b1}}; end
        default:begin dft_val_op = {p_sc_nbr{1'b0}}; dft_commit_ack = {p_sc_nbr{1'b0}}; end
      endcase

      // register transactions
      rf_ren[a_opcode] = (c_opcode == RD) ? 1'b1 : p_sc_nbr*1'b0;  rf_wen[a_opcode] = 1'b1;
      rf_ren[a_status] = 1'b1;                            rf_wen[a_status] = 1'b1;
      rf_ren[a_config] = 1'b0;                            rf_wen[a_config] = 1'b0;
      rf_ren[a_dutin]  = (c_dutin == RD) ? 1'b1 : 1'b0;   rf_wen[a_dutin]  = 1'b1;
      rf_ren[a_dutout] = 1'b1;                            rf_wen[a_dutout] = (c_dutout == WR) ? 1'b1 : 1'b0;
      rf_ren[p_sc_nbr+5-1:5] = {p_sc_nbr{1'b1}};           
 
      for (j = 0;j < p_sc_nbr; j = j + 1) begin
        rf_wen[j+5] = (c_dftout == WR_PLUS_1) ? dft_output_strobe[j] : 1'b0;
      end

      // address counter transactions
      case (c_flatten)
        CLR:    begin flatten_clr = 1'b1; flatten_en = 1'b0; end
        PLUS_1: begin flatten_clr = 1'b0; flatten_en = 1'b1; end
        default:begin flatten_clr = 1'b1; flatten_en = 1'b0; end
      endcase

      case (c_pack)
        CLR:    begin pack_clr = 1'b1; pack_en = 1'b0; end
        PLUS_1: begin pack_clr = 1'b0; pack_en = 1'b1; end
        default:begin pack_clr = 1'b1; pack_en = 1'b0; end
      endcase


      for (j = 0;j < p_sc_nbr; j = j + 1) begin
        case (c_dftout)
          CLR:       begin dft_cnt_clr[j] = 1'b1; dft_cnt_en[j] = 1'b0; end
          WR_PLUS_1: begin dft_cnt_clr[j] = 1'b0; dft_cnt_en[j] = 1'b1; end
          default:   begin dft_cnt_clr[j] = 1'b0; dft_cnt_en[j] = 1'b0; end
        endcase
      end

      // dut io unpack transactions
      case (c_dut_inf)
        F_SIN:   begin dut_input_vec_en = 1'b1; dut_input_vec_mode = SIN;  end
        F_POUT:  begin dut_input_vec_en = 1'b1; dut_input_vec_mode = POUT; end
        default: begin dut_input_vec_en = 1'b0; dut_input_vec_mode = SIN;  end
      endcase

      case (c_dut_outp)
        P_PIN:   begin dut_output_vec_en = 1'b1; dut_output_vec_mode = PIN;  end
        P_SOUT:  begin dut_output_vec_en = 1'b1; dut_output_vec_mode = SOUT; end
        default: begin dut_output_vec_en = 1'b0; dut_output_vec_mode = PIN;  end
      endcase
    end
  endtask

  // mealy output conditions
  wire end_test;
  assign end_test = (ctrl_opcode == ENDT);

  always @(*) begin
    case (current_state)
      //                      dut     dft      opcode  status  config  dutin  dutout  flatten  inf    pack    outp   dftout      
      IDLE:             cbout(RST,    NOP,     RD,     WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,   CLR);        

      INPUT_FLATTEN:    cbout(NOP,    NOP,     NOP,    WR,     NOP,    RD,    NOP,    PLUS_1,  F_SIN, CLR,    NOP,   CLR);  

      INPUT_DUT:        cbout(VAL_OP, NOP,     NOP,    WR,     NOP,    NOP,   NOP,    CLR,     F_POUT,CLR,    NOP,   CLR);  

      INPUT_RDY:        cbout(HALT,   NOP,     RD,     WR,     NOP,    NOP,   NOP,    CLR,     F_POUT,CLR,    NOP,   CLR);  
      
      OUTPUT_WAIT:      cbout(NOP,    NOP,     NOP,    WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,   CLR);  

      OUTPUT_VAL:       cbout(NOP,    NOP,     RD,     WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    P_PIN, CLR);  

      OUTPUT_PACK:      cbout(CM_ACK, NOP,     NOP,    WR,     NOP,    NOP,   WR,     CLR,     NOP,   PLUS_1, P_SOUT,CLR);  

      SCAN_PREP:        cbout(HALT,   VAL_OP,  NOP,    WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,   CLR);  
      
      SCAN:             
        if(dft_output_strobe)   
                        cbout(NOP,    NOP,     NOP,    WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,  WR_PLUS_1);
        else
                        cbout(NOP,    NOP,     NOP,    WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,  NOP);  
      
      SCAN_RD:          
        if(end_test)
                        cbout(CM_ACK, CM_ACK,  RD,     WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,  NOP);
        else
                        cbout(HALT,   CM_ACK,  RD,     WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,  NOP);  
      
      TICK:             cbout(NOP,    NOP,     NOP,    WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,  CLR);  
     
      default:          cbout(NOP,    NOP,     NOP,    WR,     NOP,    NOP,   NOP,    CLR,     NOP,   CLR,    NOP,  CLR);  
    endcase
  end
endmodule // axi_prewrapper_ctrl