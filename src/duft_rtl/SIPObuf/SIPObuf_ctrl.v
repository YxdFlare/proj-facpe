`timescale 1ns / 100ps

module SIPO_buf_256B_ctrl(
  input clk,
  input reset,

  input val_op,
  input op,
  output op_ack,
  output op_commit,
  
  output addrclr,
  output sftregclr,
  output scaning,
  output sften,
  output cnten,
  output mem_wen,
  output mem_ren
);
    
  reg op_ack_REG;
  reg op_commit_REG;
  
  reg addrclr_REG;
  reg sftregclr_REG;
  reg sften_REG;
  reg cnten_REG;
  reg mem_wen_REG;
  reg mem_ren_REG;
  
  // ctrl signal def
  localparam wr = 1'b0;
  localparam rd = 1'b1;

  // state def
  localparam IDLE = 3'd0;
  localparam SEIN = 3'd1;
  localparam MEMW = 3'd2;
  localparam MEMR = 3'd3;
  localparam MEMRR = 3'd4;

  // state register
  reg [2:0] current_state;
  reg [2:0] next_state;

  // state transition
  always @(posedge clk)
    if (reset)
      current_state <= IDLE;
    else
      current_state <= next_state;

  // shift bit counter
  reg [5:0] shiftcnt;
  reg shiftcntov;
  reg shiftcntclr;
  always @(posedge clk)
    if (shiftcntclr)
      shiftcnt <= 6'd0;
    else if (shiftcnt == 6'd32) 
      shiftcnt <= 6'd0;
    else
      shiftcnt <= shiftcnt + 1;
  always @* 
    if (shiftcnt == 6'd32)
      shiftcntov = 1'b1;
    else 
      shiftcntov = 1'b0;

  // next state logic
  always @*
    case (current_state)
      IDLE: 
        if (val_op && op == wr)
          next_state  = SEIN;
        else if (val_op && op == rd)
          next_state = MEMR;
        else
          next_state = IDLE;
      SEIN:
        if (!shiftcntov)
          next_state = SEIN;
        else
          next_state = MEMW;
      MEMW:
        next_state = IDLE;
      MEMR:
        next_state = MEMRR;
      MEMRR:
        next_state = IDLE;
      default :
        next_state = IDLE;
    endcase

  //output logic
  assign scaning = (current_state == SEIN) ? 1'b1 : 1'b0;

  assign op_ack = op_ack_REG;
  assign op_commit = op_commit_REG;
  assign addrclr = addrclr_REG;
  assign sftregclr = sftregclr_REG;
  assign sften = sften_REG;
  assign cnten = cnten_REG;
  assign mem_wen = mem_wen_REG;
  assign mem_ren = mem_ren_REG;

  localparam y = 1'b1;
  localparam n = 1'b0;

  task cbout(op_ack_cb,op_commit_cb,shiftcntclr_cb,addrclr_cb,sftregclr_cb,sften_cb,cnten_cb,mem_ren_cb,mem_wen_cb);
    begin
		 op_ack_REG = op_ack_cb;
		 op_commit_REG = op_commit_cb;
		 addrclr_REG = addrclr_cb;
		 sftregclr_REG = sftregclr_cb;
		 sften_REG = sften_cb;
		 cnten_REG = cnten_cb;
		 mem_wen_REG = mem_wen_cb;
		 mem_ren_REG = mem_ren_cb;
		 shiftcntclr = shiftcntclr_cb;
	 end
  endtask

  always @*
    case (current_state)
                            //  op    op   shift  addr  shift  shift  count mem mem
                            //  ack commit cntclr clr   clr    en     en    ren wen
      IDLE:   if (reset)  cbout(n,  n,     y,     y,    y,     n,     n,    n,  n);
        else              cbout(n,  n,     y,     n,    y,     n,     n,    n,  n);
      SEIN:   if (val_op) cbout(y,  n,     n,     n,    n,     y,     n,    n,  n);
        else              cbout(n,  n,     n,     n,    n,     y,     n,    n,  n);
      MEMW:   if (reset)  cbout(n,  y,     n,     y,    y,     n,     y,    n,  y);
        else              cbout(n,  y,     n,     n,    n,     n,     y,    n,  y);
      MEMR:   if (reset)  cbout(y,  n,     n,     y,    y,     n,     y,    y,  n);
        else              cbout(y,  n,     n,     n,    n,     n,     y,    y,  n);
      MEMRR:              cbout(n,  y,     n,     n,    n,     n,     n,    y,  n);
      default:            cbout(n,  n,     y,     y,    y,     n,     n,    n,  n);
	endcase

endmodule // SIPObuf_ctrl