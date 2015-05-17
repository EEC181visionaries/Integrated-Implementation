//
// Revision History :
// --------------------------------------------------------------------
//   Ver  :| Author            :| Mod. Date :| Changes Made:
//   V1.0 :| Philip Chan       :| 05/17/15  :| Added revision history
//   V 	 :| 				       :| 05/xx/15  :| 
//   V 	 :| 				       :| 05/xx/15  :| 
//   V 	 :| 				       :| 05/xx/15  :| 
//   V 	 :| 				       :| 05/xx/15  :| 
// --------------------------------------------------------------------

module ROI(		
				oSize,
				iStart,
				iDone,
				iDATA,
				iDVAL,
				iCLK,
				iRST,
				oRead_data,
				
				iRead_clock,
				iRead_reset,
				);

input 	iStart;
input 	iDone;
input		iDATA;
input		iDVAL;
input		iCLK;
input		iRST;
input 	iRead_clock;
input 	iRead_reset;
output	reg	[7:0] oSize;
output 	reg oRead_data;

reg  target_image [239:0][319:0];
reg [7:0] row_index = 0;
reg [7:0] col_index = 0;
reg finished = 0;
reg [7:0] read_row = 0;
reg [7:0] read_col = 0;
integer r;
integer c;


always @(posedge iCLK or negedge iRST)
begin
if (!iRST)
	begin
/*			for (r = 0; r < 240; r = r+1)
			begin
				for (c = 0; c < 320; c = c+1)
					target_image[r][c] <= 0;
			end*/
	end
else
begin
	target_image[20][30] <= 1;
	if (iStart)
		begin
		if (finished)
			finished <= 0;
		if (iDVAL)
			begin
			
				target_image[row_index][col_index] <= iDATA;
				target_image[200][300] <= 1;
			end // end of if val
		end // end of if start
	else
	begin
		if (iDone)
		begin
			oSize <= row_index;
			finished <= 1;
		end
	end
end // end of else
end // end of always@

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

always @(posedge iRead_clock)
begin
	
	begin
		oRead_data <= target_image[read_row][read_col];
	end

end


always @(negedge iRead_clock or negedge iRead_reset)
begin
	if (!iRead_reset)
	begin
		read_row <= 0;
		read_col <= 0;
	end
	else
	begin
		if (iDVAL)
		begin
			read_col <= read_col + 1'b1;
		end
		if (read_col > 318)
		begin
			col_index <= 0;
			row_index <= row_index + 1'b1;
		end
		if (row_index > 318)
			row_index <= 0;
	end
end

endmodule