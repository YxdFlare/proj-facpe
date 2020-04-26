`timescale 1ns /1ps

module dummy_ap_ctrl_chain_tb;

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
  dummy_ap_ctrl_chain UUT(
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
  localparam TEST_BASE  = 32'h00000000;
  localparam INVALID_ADDR = 32'hFFFFFFFF;

  // system parameters
  localparam clk_halfperiod = 1;
  localparam y = 1'b1;
  localparam n = 1'b0;
  localparam read = 1;
  localparam write = 0;
  localparam cycle_nbr = 9;
  localparam max_latency = 10;

  //--------------------------------------------------------------
  // user-defined variables
  //--------------------------------------------------------------

  integer timer1;
  integer timer2;
  localparam TIMEOVER = 200;

  reg [31:0] data_buf;
  reg [31:0] dat_in;
  reg [31:0] expected;
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
      DUFT_write(TEST_BASE,dat_in);
      DUFT_read(TEST_BASE,data_buf);
      $display("\tInput data: %h, Gathered data %h, Expected data %h",dat_in,data_buf,expected);
      case_passed = (expected == data_buf);
    end_case("Basic Directed IO Test",case_passed);
        
    tick(1); $stop;
  end
 
endmodule // DUFT_ap_ctrl_hs_tb