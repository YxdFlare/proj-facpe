`timescale 1ns / 100ps
module BSRSW_RF32 // single tri-read single write 32-bit register file with biased address
#(
  parameter base_addr = 32'd0,
  parameter reg_nbr = 32
)(
  input [31:0] rdaddr,
  input [31:0] wraddr,
  output [31:0] rddata,
  input [31:0] wrdata,
  input wen,
  input ren,
  input clk,
  input reset
);
  localparam addr_width = (reg_nbr == 1) ? 1 : $clog2(reg_nbr);
  reg [31:0] regfile [reg_nbr-1:0];

  // write address decoder
  reg [addr_width-1:0] locwraddr;
  reg locwen;
  always @(*) begin
    if (wraddr - base_addr > reg_nbr - 1 || wraddr < base_addr) begin
      locwraddr = 0;
      locwen = 1'b0;
    end
    else begin
      locwraddr = wraddr[addr_width-1:0] - base_addr[addr_width-1:0];
      locwen = wen;
    end
  end

  // read address decoder
  reg [addr_width-1:0] locrdaddr;
  reg locren;
  always @(*) begin
    if (rdaddr - base_addr > reg_nbr - 1 || rdaddr < base_addr) begin
      locrdaddr =  0;
      locren = 1'b0;
    end
    else begin
      locrdaddr = rdaddr[addr_width-1:0] - base_addr[addr_width-1:0];
      locren = ren;
    end
  end

  // tri-state read0
  assign rddata = locren ? regfile[locrdaddr] : 32'dz;


  // write
  always @(posedge clk ) begin
    if (locwen)
      regfile[locwraddr] = wrdata;
  end

endmodule // addressed_RF32



module DRSW_RF32 //double tri-read single write 32-bit register file
#(
  parameter reg_nbr = 32
)(
  input [31:0] wraddr,
  input [31:0] wrdata,
  input [31:0] rdaddr0,
  input [31:0] rdaddr1,
  output [31:0] rddata0,
  output [31:0] rddata1,
  input wen,
  input ren0,
  input ren1,
  input clk,
  input reset
);
  localparam addr_width = $clog2(reg_nbr);
  reg [31:0] regfile [addr_width-1:0];

  // write address decoder
  reg [addr_width-1:0] locwraddr;
  reg locwen;
  always @(*) begin
    if (wraddr > reg_nbr - 1) begin
      locwraddr = 0;
      locwen = 1'b0;
    end
    else begin
      locwraddr = wraddr[addr_width-1:0];
      locwen = wen;
    end
  end

  // read address0 decoder
  reg [addr_width-1:0] locrdaddr0;
  reg locren0;
  always @(*) begin
    if (rdaddr0 > reg_nbr - 1) begin
      locrdaddr0 = 0;
      locren0 = 1'b0;
    end
    else begin
      locrdaddr0 = rdaddr0[addr_width-1:0];
      locren0 = wen;
    end
  end

  // read address1 decoder
  reg [addr_width-1:0] locrdaddr1;
  reg locren1;
  always @(*) begin
    if (rdaddr1 > reg_nbr - 1) begin
      locrdaddr1 = 0;
      locren1 = 1'b0;
    end
    else begin
      locrdaddr1 = rdaddr1[addr_width-1:0];
      locren1 = wen;
    end
  end

  // tri-state read0
  assign rddata0 = locren0 ? regfile[locrdaddr0] : 32'dz;
  // tri-state read1
  assign rddata1 = locren1 ? regfile[locrdaddr1] : 32'dz;

  // write
  always @(posedge clk ) begin
    if (locwen)
      regfile[locwraddr] = wrdata;
  end

endmodule // DRSW_RF32 

module buf_word_expand // combiantional read
#(
  parameter expansion = 8
)(
  input [31:0] word_stream,
  output [expansion*32-1:0] word_lanes,
  input [31:0] addr,
  input en,
  input mode,
  input clk,
  input reset
);
  localparam SIN = 1'b1;
  localparam POUT = 1'b0;

  reg [2:0] locaddr;
  // address decoder
  always @(*) begin
    if (addr > expansion-1)
      locaddr = 3'd0;
    else
      locaddr = addr[2:0];
  end
  
  // flattened data output
  reg [31:0] buffer [expansion-1:0];
  genvar i;
  for (i = 0;i < expansion; i = i + 1) begin
    assign word_lanes[32*i+31:32*i] = reset ? 32'd0 : buffer[i];
  end

  // read-in data when enabled and mode is SIN(1)
  always @(posedge clk )  begin
    if (en && mode == SIN)
      buffer[locaddr] = word_stream;
  end
  

endmodule // buf_word_expand

module buf_word_contract // combiantional read
#(
  parameter contract = 8
)(
  output reg [31:0] word_stream,
  input [contract*32-1:0] word_lanes,
  input [31:0] addr,
  input en,
  input mode,
  input clk,
  input reset
);
  localparam PIN = 1'b1;
  localparam SOUT = 1'b0;

  reg [2:0] locaddr;
  // address decoder
  always @(*) begin
    if (addr > contract-1)
      locaddr = 3'd0;
    else
      locaddr = addr[2:0];
  end
  
  // unpcked data input
  reg [31:0] buffer [contract-1:0];
  genvar i;
  for (i = 0;i < contract; i = i + 1) begin
    always @(posedge clk) buffer[i] = word_lanes[32*i+31:32*i];
  end

  // dump-out data when enabled and mode is SOUT(0)
  always @(*)  begin
    if (reset)
      word_stream = 32'd0;
    else if (en && mode == SOUT)
      word_stream = buffer[locaddr];
  end
  

endmodule // buf_word_contract