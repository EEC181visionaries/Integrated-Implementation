
//   Ver  :| Author            :| Mod. Date :| Changes Made:
//   V1.0 :| Jessica MA        :| 05/24/15  :| Find Left and Right Bound
//   V1.0 :| Philip Chan       :| 05/26/15  :| Find top and bottom Bound
// --------------------------------------------------------------------

module ROI( 
        oDone,
		  oTopBound,
		  oBotBound,
        oLeftBound,
        oRightBound,
        iStart,
        iDATA,
        iDVAL,
        iCLK,
        iRST,
        iFVAL,
        oTestLEDS
        );



output oDone;
output [8:0] oTopBound;
output [8:0] oBotBound;
output [8:0] oLeftBound;
output [8:0] oRightBound;
input   iStart;
input   iDATA;
input   iDVAL;
input   iCLK;
input   iRST;
input   iFVAL;
output oTestLEDS;



// Finding height of ROI
reg [8:0] current_consecutive_white = 0;
reg [8:0] largest_consecutive_white = 0;
reg [8:0] prev_consecutive_white = 0;
reg [9:0] num_invalid = 0;
reg [1:0] rows_state = 2'b00;
reg [8:0] top_bound = 0;
reg [8:0] bot_bound = 239;
reg [8:0] height_counter = 0;
reg started = 0;
reg recording = 0;
reg stored_height = 0;
reg black_found = 0;

reg consecutive_stored = 0; // flag

// Index for Summing Image
reg [8:0] row_index = 0;
reg [8:0] col_index = 0;
reg       finished = 0;
integer   r;

// Calculating Left and Right Bounds (left and right columns)
reg [7:0] sum[319:0];
reg [7:0] prev_sum = 0;
reg [7:0] leftBound = 0;
reg [7:0] rightBound = 0;
reg       calcStart = 0;
reg       done = 0;
reg       ready = 1;



assign oTopBound = top_bound;
assign oBotBound = bot_bound;
assign oLeftBound = leftBound;
assign oRightBound = rightBound;
assign oDone = done;




always @(posedge iCLK or negedge iRST)
begin
	if (!iRST)
	begin
        done <= 0;
        started <= 0;
        rows_state <= 2'b00;
        recording <= 0;
        stored_height <= 0;
        consecutive_stored <= 0;
        num_invalid <= 0;
        black_found <= 0;
        height_counter <= 0;
        largest_consecutive_white<= 0;
        current_consecutive_white<= 0;
        prev_consecutive_white <= 0;
	end
	else
	begin
		if (iStart)
		begin
			started <= 1;
			done <= 0;
		end
		else
		begin
			if (!iFVAL)
			begin
			
				if (started)
				begin
					started <= 0;
					recording <= 1;
					rows_state <= 2'b00;
                    black_found <= 0;
                    height_counter <= 0;
                    largest_consecutive_white<= 0;
                    current_consecutive_white<= 0;
                    prev_consecutive_white <= 0;
                    stored_height <= 0;
                    consecutive_stored <= 0;
                    top_bound <= 0;
                    bot_bound <= 239;
                    num_invalid <= 0;
				end
				
			end
			else
			begin
				if (recording)
				begin
					if (!done)
					begin
					
						if (iDVAL) // if valid data
						begin
                        
							num_invalid <= 0;
							consecutive_stored <= 0; // flag used for stored max consec whites
							stored_height <= 0; // flag used to help store height counter
						
							if (iDATA) // if white
							begin
								black_found <= 0;
								current_consecutive_white <= current_consecutive_white + 1'b1;
							
									// keep track of the largest consecutive white on the row, but dont count it if the white space is tiny or spotty
								if ( (current_consecutive_white >= largest_consecutive_white) && (current_consecutive_white > 8) )
									largest_consecutive_white <= current_consecutive_white + 1'b1;
							
							end
							else // not white
							begin
								if (black_found) // noise protection against single 
									current_consecutive_white <= 0;
								else
									black_found <= 1;
							end
					
						end
						else // not valid data
						begin
					
							num_invalid <= num_invalid + 1'b1;
						
						
							if (num_invalid > 320) // new row
							begin
								if (!stored_height)
								begin	
									
									height_counter <= height_counter + 1'b1;
									stored_height <= 1;
									prev_consecutive_white <= largest_consecutive_white;
									largest_consecutive_white <= 0;
									if (height_counter > 238)
                                    begin
										done <= 1;
                                        recording <= 0;
									end
								end
								
							end
							else
							begin
								if (num_invalid > 4)
								begin
							
									if (!consecutive_stored)
									begin
										consecutive_stored <= 1;
									
									
										case (rows_state)
										2'b00:
											begin
												if ( largest_consecutive_white < (prev_consecutive_white[8:2] * 3) )
												begin
													top_bound <= height_counter;
													rows_state <= 2'b01;
												end
											end
									
										2'b01:
											begin
												if ( largest_consecutive_white > (prev_consecutive_white[8:1] * 3) )
												begin
													rows_state <= 2'b10;
													bot_bound <= height_counter;
													recording <= 0;
													done <= 1;
												end
											end
									
										2'b10:
											begin
												done <= 1;
                                                recording <= 0;
											end
									
										2'b11:
											begin
												rows_state <= 2'b00;
											end
									
									
										endcase
									
									end // ! consecutive_stored
								
								end // > 3
							end // ! > 320
				
				
						end // !DVAL
					end // !done
				end // recording
			end // FVAL
		end // !start
	end // RST

end // end of always block









/*



//
// Calculate Sum
//
always @(posedge iCLK or negedge iRST)
begin

  if (!iRST)
  begin // if reset
    sum <= 0;
    prev_sum <= 0;
    leftBound <= 0;
    rightBound <= 0;
    calcStart <= 0;
    done <= 0;
    ready <= 1;
    row_index <= 0;
    col_index <= 0;
    finished <= 0;
  end // if reset

  else
  begin // if not reset
    
    if (iStart)
    begin

      if (iDVAL)
      begin
        sum[col_index] <= iDATA + sum[col_index];
      end // if valid

    end // if start
    else if (!iStart)
    begin // if not start

      if ((sum[0] != 0) && (done == 0))
      begin
        calcStart <= 1;
        finished <= 1;
      end

    end // if not start

  end // not reset

end // at clk or rst


//
// Calculate Right and Right Bounds
//
always @(posedge calcStart)
begin
  prev_sum <= sum[0];

  for (r = 1; r < 319; r = r + 1)
  begin

    if ((sum[r] < (.75 * prev_sum)) && (right_bound == 0))
    begin
      left_bound <= r;
    end

    else if ((sum[r] > (1.50 * prev_sum)) && (left_bound != 0))
    begin
      right_bound <= r;
    end

    prev_sum <= sum[r];
  end

  done <= 1;
  calcStart <= 0;
end // Calculate Start and End Column


//
// Set index for matrix
//
always @(negedge iCLK)// or negedge iRST)
begin

  if (iStart)
  begin
    if ((ready == 0) && (finished == 1))
    begin
      sum <= 0;
      prev_sum <= 0;
      leftBound <= 0;
      rightBound <= 0;
      calcStart <= 0;
      done <= 0;
      ready <= 1 ;
      finished <= 0
    end

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

  if (finished) // reset registers for new round;
  begin
    row_index <= 0;
    col_index <= 0;
  end

end

//
// Calculating Left and Right Boudns
//
/*
always @(finished)
begin

  if (finished == 1)
  begin
    prev_sum = sum[0];

    for (i = 0; i < 320; i++)
    begin

      if (left_bound == 0)
      begin

        if ((sum[i] < 120) && (prev_sum < 120))
        begin
          left_bound = i - 1;
        end // setting left bound

      end // if no left bound

      else if ((left_bound != 0) && (right_bound == 0))
      begin

        if ((sum[i] > 120) && (prev_sum < 120))
        begin
          right_bound = i - 1;
        end // setting right bound

      end // if no right bound

    end // go though all the sums and find left and right bounds

  end // if finished == 1

end // if finished
*/


endmodule






