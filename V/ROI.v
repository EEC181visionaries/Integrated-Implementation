// --------------------------------------------------------------------
// Copyright (c) 2007 by Terasic Technologies Inc. 
// --------------------------------------------------------------------
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
//
// --------------------------------------------------------------------
//           
//                     Terasic Technologies Inc
//                     356 Fu-Shin E. Rd Sec. 1. JhuBei City,
//                     HsinChu County, Taiwan
//                     302
//
//                     web: http://www.terasic.com/
//                     email: support@terasic.com
//
// --------------------------------------------------------------------
//
// Major Functions:	RAW2BW
//
// --------------------------------------------------------------------
//
// Revision History :
// --------------------------------------------------------------------
//   Ver  :| Author            :| Mod. Date :| Changes Made:
//   V1.0 :| Johnny FAN        :| 07/07/09  :| Initial Revision
//   V2.0 :| Jessica MA	       :| 05/04/15  :| RGB to BW
// --------------------------------------------------------------------

module ROI(		
				oSize,
				iStart,
				iDone,
				iDATA,
				iDVAL,
				iCLK,
				iRST
				);

input 		iStart;
input 		iDone;
input		iDATA;
input		iDVAL;
input		iCLK;
input		iRST;
output	reg	[7:0] oSize;

// SDRAM Storage of Image
reg  target_image [239:0][319:0];

// Index for Storing Image
reg [7:0] row_index = 0;
reg [7:0] col_index = 0;
reg finished = 0;
integer r;
integer c;

// Calculating Start and End Columns
reg [7:0] sum[319:0] = 0;
reg [7:0] prev_sum = 0;
reg [7:0] start_col = 0;
reg [7:0] end_col = 0;
reg 		calcStart = 0;
reg 		calcDone = 0;
reg 		sumEnable = 0;


always @(posedge iCLK or negedge iRST)
begin

	if (!iRST)
	begin // if reset
/*			for (r = 0; r < 240; r = r+1)
			begin
				for (c = 0; c < 320; c = c+1)
					target_image[r][c] <= 0;
			end*/
	end // if reset

	else
	begin // if not reset
		
		if (iStart)
		begin

			if (finished)
				finished <= 0;

			if (iDVAL)
			begin
				target_image[row_index][col_index] <= iDATA;
				sum[col_index] <= iDATA;
			end // if valid

		end // if start

		else if (!iStart)
		begin // if not start

			if ((sum[0] != 0) && (calcDone == 0))
			begin
				calcStart = 1;
			end

			if (iDone)
			begin
				oSize <= row_index;
				finished <= 1;
			end // if done

		end // if not start

	end // not reset

end // at clk or rst


// Calculate Start and End Columns
always @(calcStart)
begin

	for (r = 0; r < 319; r = r + 1)
	begin

		if ((sum[r] < (.75 * prev_sum)) && (end_col == 0))
		begin
			start_col = r;
		end

		else if ((sum[r] > (1.50 * prev_sum)) && (start_col != 0))
		begin
			end_col = r;
		end

		prev_sum = sum[r];
	end

end // Calculate Start and End Column


// Set index for matrix
always @(negedge iCLK)// or negedge iRST)
begin

	if (iStart)
	begin

		if (iDVAL)
		begin
			col_index <= col_index + 1'b1;
		end

		if (col_index > 319)
		begin
			col_index <= 0;
			row_index <= row_index + 1'b1;
		end

	end

	if (finished)	// reset registers for new round;
	begin
		row_index <= 0;
		col_index <= 0;
	end

end

endmodule







