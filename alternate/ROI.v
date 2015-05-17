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
				iNewFrame,
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
input 	iNewFrame;	// might not need
output	reg	[7:0] oSize;
output 	reg oRead_data;

reg  target_image [239:0][319:0];
reg [7:0] row_index = 0;
reg [7:0] col_index = 0;
reg finished = 0;
reg [7:0] read_row = 0;
reg [7:0] read_col = 0;
reg [9:0] invalid_count = 0;
reg started;
reg new_frame;
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
		if (iDVAL)
		begin
			new_frame <= 0;
			invalid_count <= 0;
			if (started)
			begin
				target_image[row_index][col_index] <= iDATA;
				if (row_index > 238)
				begin
					started <= 0;
					row_index <= 0;
					col_index <= 0;
				end
				else
				begin
					row_index <= row_index + 1'b1;
					if (col_index > 318)
						col_index <= 0;
					else
						col_index <= col_index + 1'b1;
				end
			end
			else	// if havent started recording yet
			begin
				if (iStart && new_frame)
				begin
					started <= 1;
					target_image[0][0] <= iDATA;
					row_index <= row_index + 1'b1;
					col_index <= col_index + 1'b1;
				end
			end // end of else (havent started recording)
		end
		else // invalid pixel
		begin
			invalid_count <= invalid_count + 1'b1;
			if (invalid_count > 400) 
				new_frame <= 1;
		end
	end // end of else
end // end of always@



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
		read_col <= read_col + 1'b1;
		
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