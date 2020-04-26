`timescale 1ns / 100ps
module arbitrary_counter32(
  input [31:0] cnt_max,
  input [31:0] cnt_min,
  output [31:0] cnt_value,
  input clr,
  input inc_dec,
  input cnten,
  input clk
);
  
  reg [31:0] cnt;

  always @(posedge clk)
    if (inc_dec && clr)
      cnt <= cnt_min;
    else if ((!inc_dec) && clr)
      cnt <= cnt_max;
    else if (inc_dec && (cnt == cnt_max))
      cnt <= cnt_min;
    else if ((!inc_dec) && (cnt == cnt_min))
      cnt <= cnt_max;
    else if (inc_dec && cnten)
      cnt <= cnt + 1;
    else if ((!inc_dec) && cnten)
      cnt <= cnt - 1;
    else 
      cnt <= cnt;

    assign cnt_value = cnt;
endmodule // arbitrary_counter32

module simple_counter #(
parameter p_nbits = 32,
parameter max = 0
)(
  output reg [p_nbits-1:0] value,
  output ov,
  input clr,
  input cnten,
  input clk,
  input reset
);
  always @(posedge clk ) begin
    if (clr || reset)
      value <= 0;
    else if (value == max)
      value <= 0;
    else if (cnten)
      value <= value + 1;
  end
  assign ov  = (value == max) ? 1'b1 : 1'b0;
endmodule