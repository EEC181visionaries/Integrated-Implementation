//
// Revision History :
// --------------------------------------------------------------------
//   Ver  :| Author            :| Mod. Date :| Changes Made:
//   V1.0 :| Philip Chan       :| 05/17/15  :| Created
//   V 	 :| 				       :| 05/xx/15  :| 
//   V 	 :| 				       :| 05/xx/15  :| 
//   V 	 :| 				       :| 05/xx/15  :| 
//   V 	 :| 				       :| 05/xx/15  :| 
// --------------------------------------------------------------------

module ROI_rows(
				iPixel,
				iPix_val,
				iClk,
				iRST
				);

input		iCLK;
input		iRST;

always @(posedge iCLK or negedge iRST)
begin
	if (!iRST)
	begin
		
	end // end of iRST
	else // not resetting
	begin
		
	end // end of ELSE
end

endmodule