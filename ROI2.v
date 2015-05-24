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
// Major Functions: RAW2BW
//
// --------------------------------------------------------------------
//
// Revision History :
// --------------------------------------------------------------------
//   Ver  :| Author            :| Mod. Date :| Changes Made:
//   V1.0 :| Johnny FAN        :| 07/07/09  :| Initial Revision
//   V2.0 :| Jessica MA        :| 05/04/15  :| RGB to BW
//   V3.0 :| Jessica MA        :| 05/24/15  :| Find Left and Right Bound
// --------------------------------------------------------------------

module ROI( 
        oDone,
        oLeftBound,
        oRightBound,
        iStart,
        iDATA,
        iDVAL,
        iCLK,
        iRST
        );

input   iStart;
input   iDATA;
input   iDVAL;
input   iCLK;
input   iRST;
output  wire oDone;
output  wire [7:0] oLeftBound;
output  wire [7:0] oRightBound;

// Index for Summing Image
reg [7:0] row_index = 0;
reg [7:0] col_index = 0;
reg       finished = 0;
integer   r;

// Calculating Left and Right Bounds (left and right columns)
reg [7:0] sum[319:0] = 0;
reg [7:0] prev_sum = 0;
reg [7:0] leftBound = 0;
reg [7:0] rightBound = 0;
reg       calcStart = 0;
reg       done = 0;
reg       ready = 1;

assign oLeftBound = leftBound;
assign oRightBound = rightBound;
assign oDone = done;


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







