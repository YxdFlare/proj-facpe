`timescale 1ns /1ps
`include "../wrapper/ap_ctrl_hs_duft.v"
module DUFT_ap_ctrl_chain_tb;

  //--------------------------------------------------
  // netlisting
  //--------------------------------------------------

  // inputs
  reg [31:0] addr;
  reg [31:0] wr_data;
  reg        rd_wr;
  reg        clk;
  reg        ap_rst;
  reg        ap_start;
  reg        ap_continue;
  reg        ap_ce;

  // outputs
  wire [31:0] ap_return;
  wire        ap_idle;
  wire        ap_ready;
  wire        ap_done;

  // instantiate unit under test
  DUFT_ap_ctrl_chain UUT(
    .addr(addr),
    .wr_data(wr_data),
    .rd_wr(rd_wr),

    .ap_return(ap_return),
    .ap_rst(ap_rst),
    .ap_start(ap_start),
    .ap_continue(ap_continue),
    .ap_ce(ap_ce),
    .ap_idle(ap_idle),
    .ap_ready(ap_ready),
    .ap_done(ap_done),

    .clk(clk)
  );
  //--------------------------------------------------------------
  // user-defined parameters
  //--------------------------------------------------------------

  // address mapping
  localparam OPCODE_BASE  = 32'h00000000;
  localparam STATE_BASE   = 32'h00000001;
  localparam CONFIG_BASE  = 32'h00000002;
  localparam DUT_IN_BASE  = 32'h00000010;
  localparam DUT_OUT_BASE = 32'h00000018;
  localparam DFT_OUT_BASE = 32'h00000020;
  localparam TEST_IN_BASE = 32'hFF000000;
  localparam TEST_OUT_BASE= 32'hFF000001;
  localparam INVALID_ADDR = 32'hFFFFFFFF;

  // system parameters
  localparam clk_halfperiod = 1;
  localparam y = 1'b1;
  localparam n = 1'b0;
  localparam read = 1;
  localparam write = 0;
  localparam random_item_nbr = 100;
  localparam dump_nbr = 1;
  localparam cycle_nbr = 9;
  localparam max_latency = 10;

  // opcode codebook
  localparam NONE  = 32'd0;
  localparam INPUT = 32'd1;
  localparam RUN   = 32'd2;
  localparam ENDR  = 32'd3;
  localparam TEST  = 32'd4;
  localparam NEXT  = 32'd5;
  localparam ENDT  = 32'd6;

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
  localparam INVALID_STATE  = 32'hFFFFFFFF;

  //--------------------------------------------------------------
  // user-defined variables
  //--------------------------------------------------------------
  wire reading_state;
  assign reading_state = rd_wr && (addr == STATE_BASE);

  reg [3:0] DUFT_state;
  always @*  DUFT_state = reading_state ? ap_return[3:0] : DUFT_state;

  reg dft_val_op, dft_op_ack, dft_op_commit, dft_commit_ack;
  reg dut_val_op, dut_op_ack, dut_op_commit, dut_commit_ack; 
  always @* 
  {dft_val_op,dft_op_ack,dft_op_commit,dft_commit_ack,dut_val_op,dut_op_ack,dut_op_commit,dut_commit_ack} 
    = reading_state ? ap_return[11:4] : 
    {dft_val_op,dft_op_ack,dft_op_commit,dft_commit_ack,dut_val_op,dut_op_ack,dut_op_commit,dut_commit_ack};

  integer timer1;
  integer timer2;
  localparam TIMEOVER = 200;

  reg [31:0] data_buf;
  reg [31:0] dat_in;
  reg [31:0] expected;
  reg [31:0] dft_buf [dump_nbr-1:0];
  reg [31:0] expected_list [max_latency-1:0][dump_nbr-1:0];
  reg [31:0] captured_list [max_latency-1:0][dump_nbr-1:0];
  reg case_passed;
  reg case_passing;
  integer i,j;

  //--------------------------------------------------------------
  //  User-defined Tasks
  //--------------------------------------------------------------

  // tick cycles
  task tick;
  input [31:0] Ncycle;
    #(2*Ncycle*clk_halfperiod);
  endtask

  task tick_timeover1;
    input [4096:0] msg;
    begin
    if (timer1 < TIMEOVER)
      timer1 = timer1 + 1;
    else begin
      $display("[ERROR]:%0s",msg);
      $stop;
    end
    tick(1);
    end
  endtask

  task tick_timeover2;
    input [4096:0] msg;
    begin
    if (timer2 < TIMEOVER)
      timer2 = timer2 + 1;
    else begin
      $display("[ERROR]:%0s",msg);
      $stop;
    end
    tick(1);
    end
  endtask

  task array_test_eq;
    input [31:0] arr1 [];
    input integer len1;
    input [31:0] arr2 [];
    input integer len2;
    output eq;
    integer i;
    begin
      if(len1 > 1024 || len2 > 1024)
        eq = 0;
      else if (len1 != len2)
        eq = 0;
      else begin
        eq = 1;
        for (i = 0;i < len1 ; i = i + 1) 
          eq = eq && (arr1[i] == arr2[i]);
      end
    end
  endtask

  task start_case;
    input [4095:1] name;
    begin
      $display("----------------------------------------------------------------------------------------------");
      $display("STARTING TEST CASE : %0s",name);
      ap_continue = 1'b1;
      ap_ce = 1'b1;
    end
  endtask

  task end_case;
    input [4095:1] name;
    input passed;
    begin
      if (passed)
        $display("ENDED TEST CASE : %0s : PASSED",name);
      else
        $display("ENDED TEST CASE : %0s : FAILED",name);
      $display("----------------------------------------------------------------------------------------------");
      ap_continue = 1'b0;
      ap_ce = 1'b0;
    end
  endtask

  task DUFT_read;
    input [31:0] rd_addr;
    output [31:0] rd_data;
    begin
      addr = rd_addr;
      rd_wr = read;
      timer1 = 0;
      while(!ap_idle) tick_timeover1("ap_idle inactive!");
      ap_start = y;
      timer1 = 0;
      while(!ap_done) tick_timeover1("ap_done inactive!");
      rd_data = ap_return;
      ap_start = n;
      addr = INVALID_ADDR;
    end
  endtask

  task DUFT_write;
    input [31:0] wr_addr;
    input [31:0] data;
    begin
      addr = wr_addr;
      wr_data = data;
      rd_wr = write;
      timer1 = 0;
      while(!ap_idle) tick_timeover1("ap_idle inactive!");
      ap_start = y;
      timer1 = 0;
      while(!ap_done) tick_timeover1("ap_done inactive!");
      ap_start = n;
      addr = INVALID_ADDR;
    end
  endtask

  task initialize;
  begin
    $display("Initializing System......");
    ap_rst = y;
    ap_start = n;
    addr = INVALID_ADDR;
    wr_data = 0;
    rd_wr = read;
    tick(20);
    #clk_halfperiod;
    ap_rst = n;
    $display("Done.");
  end
  endtask

  task send_op;
    input [31:0] op;
    input [31:0] op_ret;
    reg [31:0] status;
    begin
      DUFT_write(OPCODE_BASE,op);
      DUFT_write(OPCODE_BASE,NONE);
      status = INVALID_STATE;
      timer2 = 0;
      while (status[3:0] != op_ret) begin
        DUFT_read(STATE_BASE,status);
        tick_timeover2("UUT Not Responding!");
      end
    end
  endtask

  task dut_run;
    input [31:0] dut_datain;
    output [31:0] dut_return;
    begin
      DUFT_write(DUT_IN_BASE,dut_datain);
      send_op(INPUT,INPUT_RDY);
      send_op(RUN,OUTPUT_VAL);
      send_op(ENDR,IDLE);
      DUFT_read(DUT_OUT_BASE,dut_return);
      tick(1);
    end
  endtask

  task dft_rd;
    input integer dump_nbr;
    input integer cycle_cnt;
    integer i;
    begin
      for (i = 0;i < dump_nbr ; i = i + 1) begin
        DUFT_read(DFT_OUT_BASE + i,dft_buf[i]);
      end
      captured_list[cycle_cnt] = dft_buf;
    end
  endtask

  task dft_run;
    input [31:0] dut_datain;
    input [31:0] expected_list [max_latency-1:0][dump_nbr-1:0];
    output passed;
    integer i;
    reg passing;
    reg [31:0] status;
    begin
      i = 0;
      passed = 1;
      DUFT_write(DUT_IN_BASE,dut_datain);
      send_op(INPUT,INPUT_RDY);
      send_op(TEST,SCAN_RD);
      dft_rd(dump_nbr,i);
      array_test_eq(expected_list[i],dump_nbr,dft_buf,dump_nbr,passing);
      passed = passed & passing;
      i = i + 1;
      DUFT_read(STATE_BASE,status);
      while (dut_op_commit != 1 || DUFT_state != SCAN_RD) begin
        send_op(NEXT,SCAN_RD);
        dft_rd(dump_nbr,i);
        array_test_eq(expected_list[i],dump_nbr,dft_buf,dump_nbr,passing);
        passed = passed & passing;
        i = i + 1;
        DUFT_read(STATE_BASE,status);
      end
      send_op(ENDT,IDLE);
      tick(1);
    end
  endtask


  //--------------------------------------------------------------
  //  Test source
  //--------------------------------------------------------------

  // clock generate
  initial clk = 1'b0;
  always #clk_halfperiod clk = ~clk;

  
  initial begin
    initialize;
    $display("Start Testing.");
    start_case("Basic Directed IO Test");
      dat_in = 32'h7216;
      expected = 32'h7216;
      DUFT_write(TEST_IN_BASE,dat_in);
      DUFT_read(TEST_OUT_BASE,data_buf);
      $display("\tInput data: %h, Gathered data %h, Expected data %h",dat_in,data_buf,expected);
      case_passed = (expected == data_buf);
    end_case("Basic Directed IO Test",case_passed);

    start_case("Basic Directed DUT Test");
      case_passed = 1;
      dat_in = 32'h7216;
      expected = 32'h721E; //+8
      dut_run(dat_in,data_buf);
      $display("\tInput data: %h, Gathered data %h, Expected data %h",dat_in,data_buf,expected);
      case_passed = case_passed && (expected == data_buf);

      dat_in = 32'h0722;
      expected = 32'h072A; //+8
      dut_run(dat_in,data_buf);
      $display("\tInput data: %h, Gathered data %h, Expected data %h",dat_in,data_buf,expected);
      case_passed = case_passed && (expected == data_buf);
    end_case("Basic Directed DUT Test",case_passed);

    start_case("Basic Directed DFT Test");
      case_passed = 1;
      case_passing = 1;
      dat_in = 32'h7216;
      for (i = 0; i < cycle_nbr; i = i + 1) begin
        for (j = 0; j < dump_nbr; j = j + 1) begin
          expected_list[i][j] = dat_in + i;
        end
      end      
      dft_run(dat_in,expected_list,case_passing);
      case_passed = case_passed && case_passing;
    end_case("Basic Directed DFT Test",case_passed);

    start_case("Basic Random DUT Test");
      case_passed = 1;
      for (i = 0; i < random_item_nbr ; i = i + 1) begin
        dat_in = $random;
        expected = dat_in + 8; //+8
        dut_run(dat_in,data_buf);
        if(expected != data_buf) 
          $display("\tMISMATCH!: Gathered data %h, Expected data %h",data_buf,expected);
        case_passed = case_passed && (expected == data_buf);
      end
    end_case("Basic Random DUT Test",case_passed);


    
    tick(1); $stop;
  end
 
endmodule // DUFT_ap_ctrl_hs_tb