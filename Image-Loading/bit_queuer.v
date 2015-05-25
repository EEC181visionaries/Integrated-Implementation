
module BitQueuer(	
				oData,
				oRD_CLK,
				oRD_RST,
				oCounter,
				oRead_req,
				iData,
				iCLK,
				iRST,
				iHPS_CLK,
				iHPS_RST,
				oflag
				);

output [31:0] oData;
output oRD_CLK;
output oRD_RST;
output [6:0] oCounter;
output oflag;
output oRead_req;
input iData;
input iCLK;
input iRST;
input iHPS_CLK;
input iHPS_RST;

reg [31:0] out_data = 0;
reg read_clk;
reg read_reset;
reg [6:0] counter = 0;
reg wait_for_HPS = 1;
reg counter_flag = 0;
reg request_read = 1;

assign oData = out_data;
assign oRD_CLK = read_clk;
assign oRD_RST = read_reset;
assign oCounter = counter;
assign oflag = counter_flag;
assign oRead_req = request_read;


always @(posedge iCLK or negedge iRST)
begin

	if (!iRST)
	begin
		//out_data <= 0;
		counter <= 0;
		read_clk <= 0;
		wait_for_HPS <= 0;
		read_reset <= 0;
		counter_flag <= 0;
	end
	else
	begin
		read_reset <= 1;
		if (iHPS_CLK)
		begin
			wait_for_HPS <= 0;
			counter <= 0;
		end
		else
		begin
			if (!wait_for_HPS)
			begin
				read_clk <= read_clk + 1'b1;
				if (counter[6:1] > 30)	// if currently 31, set to 0
				begin
					wait_for_HPS <= 1;
				end
				else
				begin
					counter <= counter + 1'b1;
					counter_flag <= 1;
				end
			end
		end
	end


end

always @(negedge iCLK)
begin
	out_data[counter[6:1]] <= iData; // Data will be read in every 2 clock cycles to make time for the read clk
end
				
	

endmodule
