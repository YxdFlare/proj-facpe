`timescale 1ns / 100ps

module mem_256B(
  input [31:0] din,
  output [31:0] dout,
  input [5:0] addr,
  input wen,
  input ren,
  input clk,
  input reset
);

  // memory unit
  reg [31:0] mem [63:0];

  // sequential rd/wr
  reg [31:0] dout_REG;
  assign dout = dout_REG;

  always @(posedge clk)
    if (reset)
      dout_REG <= 32'd0;
    else if (ren)
      dout_REG <= mem[addr];
    else
      dout_REG <= 32'd0;
  
  always @(posedge clk)
    if (wen)
      mem[addr] <= din;

endmodule // mem_256B (64 * 4B)