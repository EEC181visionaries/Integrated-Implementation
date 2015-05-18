// ROITest.c

// single_digit_reader_v2.c - Reads in a photo and prints out data

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
/*
#include "finalB1L1.c"
#include "finalB1L2.c"
#include "finalSoftmaxTheta.c"
#include "finalW1L1.c"
#include "finalW1L2.c"
*/
// Definitions
#define WIDTH 640
#define HEIGHT 480
#define BW_LEVEL  130
#define BLACK 0
#define WHITE 1
#define MAX_DIGITS 5

// Registers
#define READ_IN_ADDR  0xFF200090
#define READ_OUT_ADDR   0xFF200080
#define VGA_DATA1 0xFF200070
#define VGA_DATA2 0xFF200060
#define SDRAM_DATA1 0xFF200050
#define SDRAM_DATA2 0xFF200040
#define VGA_CLK_IN  0xFF200030
#define CAM_START 0xFF200020
#define SOURCE_SELECT 0xFF200010
#define CONTROLLING_CLK 0xFF200000
#define TEST 0xFF200100
#define DDR3_ADDR 0x00100000
#define ROI_READ_IN 0xFF2000A0
#define ROI_CLK_OUT 0xFF2000C0
#define ROI_RESET_OUT 0xFF2000B0
#define ROI_START_OUT 0xFF2000D0

// Computing ROI and Separate Images
int w, x, y, v, lt, lb, rt, rb;
int roi[HEIGHT][WIDTH];
int digit[HEIGHT][WIDTH];


// Timing Variables
// Resources:
//   http://www.tutorialspoint.com/c_standard_library/c_function_clock.htm
clock_t regionStart, regionEnd, regionClocks, resizeStart, resizeEnd, resizeClocks;

/*
	FOR TIMING
	==================
	regionStart = clock()
	regionEnd = clock()
	regionClocks = regionEnd - regionStart // Returns clock ticks
	regionClocks = (double)(regionEnd - regionStart)/CLOCKS_PER_SEC; // Returns time in sec?
	resizeStart = clock()
	resizeEnd = clock()
	resizeClocks = resizeEnd - resizeStart
	
	Do this for every function and main

*/

// List of functions
void region(void);
void region2(int* width,int* height,int **mat);
int resize(int height, int width, int** digit);
int resize2(int height, int width, int** img);
int recognizer(int data[784]);
void digit_separate2(int num_row, int num_col, int **roi);

int main(void)
{
  volatile int * (cam_start) = (int *) CAM_START; // Output
  volatile int * (sdram_data1) = (int *) SDRAM_DATA1; //Input
  volatile int * (sdram_read) = (int *) READ_OUT_ADDR;  //Output
  volatile int * (read_good) = (int *) VGA_CLK_IN;  //Input
  volatile int * (vga_data1) = (int *) VGA_DATA1; //Output
  volatile int * (vga_data2) = (int *) VGA_DATA2; //Output
  volatile int *(clock_select) = (int *) SOURCE_SELECT;
  volatile int *(clock_gen) = (int *) CONTROLLING_CLK;
  volatile int *(roi_clock_out) = (int*)ROI_CLK_OUT;
  volatile int *(roi_reset_out) = (int*)ROI_RESET_OUT;
  volatile int *(roi_read_data) = (int*)ROI_READ_IN;
  volatile int *(roi_start) = (int*) ROI_START_OUT;

  int M;
  int i = 0;
  int j = 0;
  int k = 0;
  int L = 0;
  int snapshot = 0;
  *vga_data1 = 1;
  *vga_data2 = 1;
  *clock_select = 0;
  int write_data = 0;
  int written = 0;
  int height = HEIGHT, width = WIDTH;
  int image[240][320] = { 0 };
  *roi_clock_out = 0;
  *roi_reset_out = 1;


  *cam_start = 1;

  while(1)
  {
    printf("Press enter to start\n");

    fflush(stdin);
    getchar();
    fflush(stdin);



    // delay before capture
    for (i = 0; i < 30000; i++)
    {
    } // delay before capture

    *roi_reset_out = 0;
    *roi_reset_out = 1;
    *roi_start = 1;

    for (int r = 0; r < 240; r++)
    {

        for (int c = 0; c < 320; c++)
        {
            image[r][c] = *(roi_read_data);
            *roi_clock_out = 1;
            *roi_clock_out = 0;
            
        } // columns

    } // rows

    *roi_start = 0;

    for (int r = 0; r < 240; r++)
    {

        for (int c = 0; c < 320; c++)
        {
            printf("%i\t", image[r][c]);
        } // columns

        printf("\n");
    } // rows

  } // while(1)

  return 0;
}














void region2(int* width,int* height,int **mat)
{
	int cols = (*width);
	int rows = (*height);
	int xLeft, xRight, yTop, yBot;
	int r = HEIGHT/2;
	int c = 0;
	int prev_hits = 0;
	int hits = 0;
	printf("Initialized\n");

  // LROI Left Edge = xLeft
	for (c = 0; c < cols; c = c + 25)
	{
		prev_hits = hits;
		if (mat[r][c] == WHITE)
		{
			xLeft = c;
			hits++;
		}
		if (prev_hits == hits && hits) // if no new hits, and 
			hits = 0;
		if (hits == 3)
		{
			hits = 0;
			prev_hits = 0;
			break;
		}
	} // for (c = 0;...)
	printf("left edge = %d\n", xLeft);

  // LROI Right Edge = xRight
  for (c = cols; c > 0; c = c - 25)
  {
    prev_hits = hits;
    if (mat[r][c] == WHITE)
    {
      xRight = c;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 3)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (c = cols;...)
  printf("right edge = %d\n", xRight);

  // LROI Top Edge = yTop
  c = WIDTH/2;
  for (r = 0; r < rows; r = r + 25)
  {
    prev_hits = hits;
    if (mat[r][c] == WHITE)
    {
      yTop = r;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 3)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (r = 0;...)
  printf("top edge = %d\n", yTop);

  // LROI Bottom Edge = yBot
  for (r = rows-1; r >= 0; r = r - 25)
  {
    prev_hits = hits;
    if (mat[r][c] == WHITE)
    {
      yBot = r;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 3)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (r = rows;...)


  printf("bottom = %d\n", yBot);
  // ROI Left Edge = xLeft





  r = (yBot+yTop)/2;
  int tempxEdge = (xLeft + xRight)/2;
  for (c = xLeft; c < xRight; c = c + 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hits == 0)
        xLeft = c;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (c = xLeft;...)
  printf("left = %d\n", xLeft);

  // ROI Right Edge = xRight
  for (c = xRight-1; c >= xLeft; c = c - 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hits == 0)
        xRight = c;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (c = xRight;...)
  printf("right = %d\n", xRight);

  // ROI Top Edge = yTop
  c = tempxEdge;
  for (r = yTop; r < yBot; r = r + 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hits == 0)
        yTop = r;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (r = yTop;...)
  printf("top = %d\n", yTop);

  // ROI Bottom Edge = yBot
  for (r = yBot-1; r >= yTop; r = r - 5)
  {
    prev_hits = hits;
    if (mat[r][c] == 0)
    {
      if (hits == 0)
        yBot = r;
      hits++;
    }
    if (prev_hits == hits && hits) // if no new hits, and 
      hits = 0;
    if (hits == 2)
    {
      hits = 0;
      prev_hits = 0;
      break;
    }
  } // for (r = yBot;...)
  printf("bot = %d\n", yBot);

  // Move region of interest to (0,0) of existing array mat[r][c]
  (*width) = xRight - xLeft;
  (*height) = yBot - yTop;
  printf("sizing\n");
  for (r = 0; r < *height; r++)
  {
    for (c = 0; c < *width; c++)
    {
      mat[r][c] = mat[yTop + r][xLeft + c];
    } // for (c = xLeft;...)
  } // for (r = yTop;...)

  
} // region



int resize(int height, int width, int** digit){

	int digit_final[28][28];
	int digit_vector[784];
	int i,j;
	int x_pixels = (width + 27)/28;
	int y_pixels = (height + 27)/28;
	int k, l;
	double average;
	double square = x_pixels * y_pixels;
	int digit_x = 0, digit_y = 0;//These are the pixels of the scaled down digit		

	for(i = 0; i < 28; i++){
		for(j = 0; j < 28; j++)
			digit_final[i][j] = 0;
	}
	
	for(i = 0; i < (height - (height%28)); i = i + y_pixels){
		digit_x = 0;
		for(j = 0; j < (width - (width%28)); j = j + x_pixels){
			average = 0;
			for(k = 0; k < y_pixels && ((i+k) < height); k++){
				for(l = 0; l < x_pixels && ((j+l) < width); l++){
					average += digit[i+k][j+l];
				}
			}
			average = average / square;

			if(average >= 0.5){
				digit_final[digit_y][digit_x] = 1;
			}
			else{
				digit_final[digit_y][digit_x] = 0;
			}
			digit_x++;
}

		digit_y++;
	}
  k = 0;
  for(i = 0; i < 28; i++)
  {
    for(j = 0; j < 28; j++)
    {
      digit_vector[k] = digit_final[j][i];
      k++;
    }
  }

  l = recognizer(digit_vector);
  return l;
}

int resize2(int height, int width, int** img){
	int scaled_img[28][28];
	int vector[784];

	//
	//col and row specify squares. p examines pixels. s is used for the scaled img. v is vector index.
	//	

	int col,row, p_col, p_row, s_col = 0, s_row = 0, v_index;	

	//Scales divide by 28 rounded up.	
	int col_scale = (width + 27)/28;	
	int row_scale = (height + 27)/28;

	//avg is used to calculate the average white density of a square, given by size square.
	double avg;
	double square = col_scale * row_scale;


	//
	// Initialize the image to be all black
	//
	int i, j;
	for(i = 0; i < 28; i++){

		for(j = 0; j < 28; j++)
			scaled_img[i][j] = BLACK;

	}
	printf("initialized black box\n");
	//
	//Examine the image, magnifying rectangle by rectangle down to 28 = (size - size%28)/scale
	//where size is the row or column and the scale is the calculated scale rounded up.
	//	
	
	for(row = 0; row < (height - (height%28)); row += row_scale){

		s_col = 0;

		for(col = 0; col < (width - (width%28)); col += col_scale)
		{

			avg = 0;
			
			//
			//Calculate the average of a square given starting coordinates
			//

			for(p_row = 0; p_row < row_scale && ((row+p_row) < height); p_row++)
			{
				for(p_col = 0; p_col < col_scale && ((col+p_col) < width); p_col++)
				{
					avg += img[row+p_row][col+p_col];
				}
			}

			avg = avg / square;

			if(avg >= 0.5)
			{
				scaled_img[s_row][s_col] = WHITE;
			}
			else
			{
				scaled_img[s_row][s_col] = BLACK;
			}
			s_col++;
		}

		s_row++;
	}
	printf("resized\n");
	//
	//Convert the scaled image to a vector for recognizer to use
	//

  	v_index = 0;
  	for(s_col = 0; s_col < 28; s_col++)
	{
    	for(s_row = 0; s_row < 28; s_row++)
		{
      		vector[v_index] = scaled_img[s_row][s_col];
      		v_index++;
		}
	}

	int k = 0;
	for (i = 0; i < 28; i++)
	{
		for (k = 0; k < 28; k++)
		{
			printf("%d\t",scaled_img[i][k]);
		}
		printf("\n");
	}

  return recognizer(vector);
}


int recognizer(int data[784])
{
 /* long double Vb1[200], Vb2[200], Vb3[10]; // array[row][col]
  int M = 0;
  int i,j;
  long double sum = 0;
  
  
  // Vb1 = finalW1L1*data;
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 784; j++)
    {
      sum = sum + finalW1L1[i][j] * data[j];
    } // Matrix Multiplication
    Vb1[i] = sum;
    sum = 0;
  } // Product into new Matrix
  
  
  //Vb1 = Vb1 + finalB1L1;
  for (i = 0; i < 200; i ++)
  {
    Vb1[i] = Vb1[i] + finalB1L1[i];
  } // Matrix Addition
  
  
  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb1[i] = 1/(1+exp(-Vb1[i]));
  } // Sigmoid
  
  
  //Vb1 = finalW1L2*Vb1;
  for (i = 0; i < 200; i++)
  {
    for (j = 0; j < 200; j++)
    {
      sum = sum + finalW1L2[i][j] * Vb1[j];
    } // Matrix Multiplication
    Vb2[i] = sum;
    sum = 0;
  } // Product into old Matrix
  
  
  //Vb1 = Vb1 + finalB1L2;
  for (i = 0; i < 200; i ++)
  {
    Vb2[i] = Vb2[i] + finalB1L2[i];
  } // Matrix Addition


  //Vb1 = sigmf(Vb1,[1 0]);
  for (i = 0; i < 200; i++)
  {
    Vb2[i] = 1/(1+exp(-Vb2[i]));
  } // Sigmoid
  
  
  //Vb1 = finalSoftmaxTheta*Vb1;          finalSoftmaxTheta[10][200]
  for (i = 0; i < 10; i++)
  {
    for (j = 0; j < 200; j++)
    {
      sum = sum + finalSoftmaxTheta[i][j]*Vb2[j];
    } //
    Vb3[i] = sum;
    sum = 0;
  } // 
  
 
  //M = find(Vb1==max(Vb1));
  double max = 0;
  for (i = 0; i < 10; i++)
  {
    if (max < Vb3[i])
    {
      max = Vb3[i];
      M = i + 1;
    }
  } // Finding Max Value
  
  
  // Check for Zero
  //if(M == 10)
  //  M = 0;
  //end
  if (M == 10)
  {
    M = 0;
  } // Check for zero

  
  //output = M;
    return M;
    */
    return 1;
}

void digit_separate2(int num_row, int num_col, int **roi)
{

	int mid_row = num_row/2;	
	int c = 0;
	
	int ***digit;

	int right_edge = 0;
	int bad = 0;
	int col_checker = 0;
	int first = 1;
	int digit_num = 0;
	int row = 0;
	int col = 0;

	int last_pixel = 0;
	int pixel = 0;
	int mid_col = 0;
	int last_mid = 0;
	int mid = 0;

	int digit_size[5] = {0};
	int digit_top = 0;
	int digit_bot = 0;
	int digit_left = 0;
	int digit_right = 0;
	int digit_height = 0;
	int digit_width = 0;
	int padding = 0;
	int horz_padding = 0;

	digit = (int ***) malloc(MAX_DIGITS*sizeof(int **));
	printf("Allocated int***\n");
	for (c = 0; c < num_col; c = c+4)
	{
		if ( (roi[mid_row][c] == WHITE) || (c+4 >= num_col) ) // hit white or end of roi
		{
			if (first) // skip the first hit of white
			{
				first = 0;
				pixel = WHITE;
			}
			else
			{
				if(last_pixel == BLACK) // check if start of digit found
				{
					mid_col = (c - right_edge)/2 + right_edge;
					if (roi[mid_row][c] == WHITE) // skip this section if we've hit the end of the ROI
					{
						// start at 4 to size_y-4 to give some buffer for tilted ROI's
						for (col_checker = 4; col_checker < num_col-4; col_checker++)
						{
							if ( roi[col_checker][mid_col] == WHITE)
							{
								bad = 1;
								break;
							}
						}
					} // if (roi[r][c] == WHITE)
					if (!bad)
					{
						if (digit_num < MAX_DIGITS)
						{
							// check for top
							for (row = 4; row < num_row-4; row++)
							{
								for (col = last_mid; col < mid; col++)
								{
									if (roi[row][col] == WHITE)
									{
										digit_top = row;
										// break out of for loops
										col = mid;
										row = num_row;
									}
								}
							}
							// check for bot
							for (row = num_row-4 - 1; row >= 4; row--)
							{
								for (col = last_mid; col <  mid; col++)
								{
									digit_bot = row;
									
									col = mid;
									row = -1;
								}
							}
							printf("digit_top = %d\n digit_bot = %d\n",digit_top,digit_bot);

							digit_height = digit_bot - digit_top;
							padding = digit_height/5;

							digit_size[digit_num] = digit_height + padding + padding;

							printf("digit_height = %d\n padding = %d\n",digit_height,padding);
							printf("digit_size[digit_num] = %d\n",digit_size[digit_num]);

							// check for left
							for (col = last_mid; col < mid; col++)
							{
								for (row = 4; row < num_row-4; row++)
								{
									if (roi[row][col] == WHITE)
									{
										digit_left = col;
										row = num_row;
										col = mid;
									}
								}
							}
							// check for right
							for (col = mid-1; col >= last_mid; col--)
							{
								for (row = 4; row < num_row-4; row++)
								{
									if (roi[row][col] == WHITE)
									{
										digit_right = col;
										row = num_row;
										col = last_mid-1;
									}
								}
							}
							digit_width = digit_right - digit_left;
							horz_padding = (digit_size[digit_num] - digit_width)/2;


							int i = 0;
							int j = 0;

							// allocate space for digits
							digit[digit_num] = (int **) malloc(digit_size[digit_num] * sizeof(int*));
							for (i = 0; i < digit_size[digit_num]; i++)
								digit[digit_num][i] = (int *)malloc(digit_size[digit_num] * sizeof(int));

							// create black box
							for (i = 0; i < digit_size[digit_num]; i++)
							{
								for (j = 0; j < digit_size[digit_num]; j++)
								{
									digit[digit_num][i][j] = BLACK;
								}
							}

							printf("done creating black box\n");
							
							// write digit to middle of black box
							for (i = padding; i < padding + digit_height; i++)
							{
								for (j = horz_padding; j < horz_padding + digit_width; j++)
								{
									digit[digit_num][i][j] = roi[digit_top + i - padding][digit_left + j - horz_padding];
								}
							}
							
							
							
							// print digit if checking
							printf("digit number: %d\n", digit_num);


							for (i = 0; i < digit_size[digit_num]; i++)
							{
								for (j = 0; j < digit_size[digit_num]; j++)
								{
									printf("%d\t", digit[digit_num][i][j]);
								}
								printf("\n");
							}
							printf("\n\n\n");
							
							// resize digit
							
							// pass digit through NN and receive integer
							// integer = function()
							
							// print single digit
							
						} // if (digit_num < MAX_DIGITS
						
						digit_num++;
					} // if (!bad)
				}
			} // else (first)
			last_pixel = WHITE;
		}
		else	// hit black
		{
			if (last_pixel == WHITE)
			{
				right_edge = c;
			}
			last_pixel = BLACK;
			pixel = BLACK;
		}
		bad = 0;
	}
} // digit_separate2

