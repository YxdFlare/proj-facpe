`timescale 1ns/100ps 
module ScanReg (
	input d,
	input sin,
	output q,
	input sen,
	input clk,
	input clr,
	input ce
	);
	
	reg q_REG;
	always @(posedge clk)
		if (clr == 1'b1)
			q_REG <= 1'd0;
		else if (ce == 1'b1 && sen == 1'b0)
			q_REG <= d;
		else if (ce == 1'b1 && sen == 1'b1)
			q_REG <= sin;
		else
			q_REG <= q_REG;
	assign q = q_REG;
	
endmodule

module ScanReg8 (
	input [7:0] d,
	output[7:0] q,
	input sin,
	output sout,
	input sen,
	input clk,
	input clr,
	input ce
    );
	 
	reg [7:0] q_REG;
	always @(posedge clk) 
		if (clr == 1'b1)
			q_REG <= 8'd0;
		else if (ce == 1'b1 && sen == 1'b0)
			q_REG <= d;
		else if (ce == 1'b1 && sen == 1'b1)
			q_REG <= {sin,q_REG[7:1]};
		else
			q_REG <= q_REG;
	assign q = q_REG;
	assign sout = q_REG[0];
endmodule

module ScanReg32 (
	input [31:0] d,
	output[31:0] q,
	input sin,
	output sout,
	input sen,
	input clk,
	input clr,
	input ce
    );
	 
	reg [31:0] q_REG;
	always @(posedge clk) 
		if (clr == 1'b1)
			q_REG <= 32'd0;
		else if (ce == 1'b1 && sen == 1'b0)
			q_REG <= d;
		else if (ce == 1'b1 && sen == 1'b1)
			q_REG <= {sin,q_REG[31:1]};
		else
			q_REG <= q_REG;
	assign q = q_REG;
	assign sout = q_REG[0];
endmodule

module ScanReg40 (
	input [39:0] d,
	output[39:0] q,
	input sin,
	output sout,
	input sen,
	input clk,
	input clr,
	input ce
    );
	 
	reg [39:0] q_REG;
	always @(posedge clk) 
		if (clr == 1'b1)
			q_REG <= 40'd0;
		else if (ce == 1'b1 && sen == 1'b0)
			q_REG <= d;
		else if (ce == 1'b1 && sen == 1'b1)
			q_REG <= {sin,q_REG[39:1]};
		else
			q_REG <= q_REG;
	assign q = q_REG;
	assign sout = q_REG[0];
endmodule
