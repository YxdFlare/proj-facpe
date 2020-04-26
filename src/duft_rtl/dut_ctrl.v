`timescale 1ns / 100ps

module IAS_ctrl (
  input clk,
  input reset,
  input val_op,
  output reg op_commit,
  output reg op_ack,
  input commit_ack,
  
  output reg reg_en,
  output reg reg_sel,
  input sen
);

  // state def
  localparam IDLE = 2'd0;
  localparam START = 2'd1;
  localparam CALC = 2'd2;
  localparam FINISH = 2'd3;
  
  // state reg
  reg [1:0] current_state;
  reg [1:0] next_state;

  // counter
  reg [2:0] calc_cnt;
  reg calc_rdy;
  reg calc_en;
  reg calc_rst;

  always @(posedge clk ) begin
    if (calc_rst)
      calc_cnt <= 3'd0;
    else if (calc_en)
      if (calc_cnt == 3'd7)
        calc_cnt <= 3'd0;
      else
        calc_cnt <= calc_cnt + 1;
  end

  always @(*) begin
    if (calc_cnt == 3'd7)
      calc_rdy = 1'b1;
    else
      calc_rdy = 1'b0;
  end

  // state transition
  always @(posedge clk)
    if (reset)
      current_state <= IDLE;
    else if (sen)
      current_state <= current_state;
    else
      current_state <= next_state;

  // next state logic
  always @*
    case (current_state)
      IDLE:
        if (val_op)
          next_state = START;
        else
          next_state = IDLE;
      START:
        next_state = CALC;
      CALC:
        if (calc_rdy)
          next_state = FINISH;
        else
          next_state = CALC;        
      FINISH:
        if (commit_ack)
          next_state = IDLE;
        else
          next_state = FINISH;
      default:
        next_state = IDLE;
    endcase

  // output logic
  localparam in = 1'b1;
  localparam clc = 1'b0;
  localparam y = 1'b1;
  localparam n = 1'b0;


  task cbout;
  input C_op_ack;
  input C_op_commit;
  input C_calc_en;
  input C_calc_rst;
  input C_reg_en;
  input C_reg_sel;
  begin
    op_ack = C_op_ack;
    op_commit = C_op_commit;
    calc_en = C_calc_en;
    calc_rst = C_calc_rst;
    reg_en = C_reg_en;
    reg_sel = C_reg_sel;
  end
  endtask
    
  always @*
    case (current_state)
                  //    op    op    calc  calc  reg   reg     
                  //    ack   cmt   en    rst   en    sel
      IDLE:       cbout(n,    n,    n,    y,    n,    in);
      START:      cbout(y,    n,    n,    n,    y,    in);
      CALC:       
      if(sen)
                  cbout(n,    n,    n,    n,    n,    clc);
      else
                  cbout(n,    n,    y,    n,    y,    clc);
      FINISH:     cbout(n,    y,    n,    y,    n,    in);
      default:    cbout(n,    n,    n,    y,    n,    in);
    endcase

endmodule // IAS_ctrl 