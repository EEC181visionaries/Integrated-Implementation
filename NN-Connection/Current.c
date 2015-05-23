// SDRv3compatible.c - region2, resize2, and recognizer complied and working
//                     separater2 not working

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#include "finalB1L1.c"
#include "finalB1L2.c"
#include "finalSoftmaxTheta.c"
#include "finalW1L1.c"
#include "finalW1L2.c"

// Definitions
#define WIDTH 320
#define HEIGHT 240
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

#define NN_WRITE_DATA_1 0xFF200140
#define NN_WRITE_DATA_2 0xFF200120
#define NN_WRITE_ENABLE 0xFF200130
#define NN_WRITE_CLOCK 0xFF200110
#define NN_READ_ENABLE 0xFF2000F0
#define NN_READ_CLOCK 0xFF2000E0
#define NN_BOOTUP 0xFF2000D0
#define NN_ACCESS 0xFF2000C0
#define NN_READ_DATA_1 0xFF2000B0
#define NN_READ_DATA_2 0xFF2000A0


// Computing ROI and Separate Images
int w, x, y, v, lt, lb, rt, rb;
int roi[HEIGHT][WIDTH];
int digit[HEIGHT][WIDTH];


// Timing Variables
unsigned int region_1, resize_1, rec_1, rec_2, main_1, main_2, main_3, main_4, main_5, main_6, main_7, final;
unsigned int separate_end, separate_start, region_end, resize_start, resize_end, recognizer_start, recognizer_end, normalization_start, normalization_end, detection_start, detection_end;
unsigned int LROIstart, LROIend, SROIstart, SROIend, ROImovStart, ROImovEnd;
unsigned int resizeStart, resizeEnd, resizeMovStart, resizeMovEnd;
unsigned int MMstart1, MMend1, vstart1, vend1, sigStart1, sigEnd1, MMstart2, MMend2, vstart2, vend2, sigStart2, sigEnd2, MMstart3, MMend3, maxStart, maxEnd;
unsigned int digits_separated;
unsigned long int totalCycles;

unsigned int cycle[100];

// List of Functions
//void region(void);
void region2(int* width, int* height, int **mat);
//int resize(int height, int width, int** digit);
int resize2(int height, int width, int** img);
int recognizer(int data[784]);
void digit_separate2(int num_row, int num_col, int **roi);
static inline unsigned int getCycles();
static inline void initCounters();

int main(void)
{
    volatile int * (cam_start) = (int *)CAM_START; // Output
    volatile int * (sdram_data1) = (int *)SDRAM_DATA1; //Input
    volatile int * (sdram_read) = (int *)READ_OUT_ADDR;  //Output
    volatile int * (read_good) = (int *)VGA_CLK_IN;  //Input
    volatile int * (vga_data1) = (int *)VGA_DATA1; //Output
    volatile int * (vga_data2) = (int *)VGA_DATA2; //Output
    volatile int *(clock_select) = (int *)SOURCE_SELECT;
    volatile int *(clock_gen) = (int *)CONTROLLING_CLK;


    volatile int *nn_write_data_1 = (int *)NN_WRITE_DATA_1;
    volatile int *nn_write_data_2 = (int *)NN_WRITE_DATA_2;
    volatile int *nn_write_enable = (int *)NN_WRITE_ENABLE;
    volatile int *nn_write_clock = (int *)NN_WRITE_CLOCK;
    volatile int *nn_read_enable = (int *)NN_READ_ENABLE;
    volatile int *nn_read_clock = (int *)NN_READ_CLOCK;
    volatile int *nn_bootup = (int *)NN_BOOTUP;
    volatile int *nn_access = (int *)NN_ACCESS;
    volatile int *nn_read_data_1 = (int *)NN_READ_DATA_1;
    volatile int *nn_read_data_2 = (int *)NN_READ_DATA_2;




    int16_t number1, number2;

  *nn_write_clock = 0;
  *nn_read_clock = 0;
  *nn_bootup = 1;
  *vga_data1 = 1; // reset signals to set read address
  *vga_data1 = 0;
  *vga_data1 = 1;
  *nn_write_enable = 1;
  
  int i = 0;
  


  // WRITING
  // finalW1L1
  /*
  for (int i = 0; i < 200; i++)
  {
      for (int j = 0; j < 784/4; j = j + 4)
      {
          number1 = (finalW1L1[i][j] << 8) | (finalW1L1[i][j+1] & 0xff);
          number2 = (finalW1L1[i][j+2] << 8) | (finalW1L1[i][j+3] & 0xff);
          *nn_write_data_1 = number1;
          *nn_write_data_2 = number2;
          *nn_write_clock = 1;
          *nn_write_clock = 0;
      }
  } // finalW1L1
  // finalB1L1
  for (int i = 0; i < 200/4; i = i + 4)
  {
    number1 = (finalB1L1[i] << 8) | (finalB1L1[i+1] & 0xff);
    number2 = (finalB1L1[i+2] << 8) | (finalB1L1[i+3] & 0xff);
    *nn_write_data_1 = number1;
    *nn_write_data_2 = number2;
    *nn_write_clock = 1;
    *nn_write_clock = 0;
  } // finalB1L1
  
  
    // finalW1L2
  for (int i = 0; i < 200; i++)
  {
      for (int j = 0; j < 200/4; j = j + 4)
      {
          number1 = (finalW1L2[i][j] << 8) | (finalW1L2[i][j+1] & 0xff);
          number2 = (finalW1L2[i][j+2] << 8) | (finalW1L2[i][j+3] & 0xff);
          *nn_write_data_1 = number1;
          *nn_write_data_2 = number2;
          *nn_write_clock = 1;
          *nn_write_clock = 0;
      }
  } // finalW1L2
  // finalB1L2
  for (int i = 0; i < 200/4; i = i + 4)
  {
    number1 = (finalW1L2[i] << 8) | (finalW1L2[i+1] & 0xff);
    number2 = (finalW1L2[i+2] << 8) | (finalW1L2[i+3] & 0xff);
    *nn_write_data_1 = number1;
    *nn_write_data_2 = number2;
    *nn_write_clock = 1;
    *nn_write_clock = 0;
  } // finalB1L2
    // finalSoftmaxTheta
  for (int i = 0; i < 10; i++)
  {
      for (int j = 0; j < 200/4; j = j + 4)
      {
          number1 = (finalW1L2[i][j] << 8) | (finalW1L2[i][j+1] & 0xff);
          number2 = (finalW1L2[i][j+2] << 8) | (finalW1L2[i][j+3] & 0xff);
          *nn_write_data_1 = number1;
          *nn_write_data_2 = number2;
          *nn_write_clock = 1;
          *nn_write_clock = 0;
      }
  } // finalSoftmaxTheta
  */

    for (i = 0; i < 20; i = i + 2)
    {
        number1 = 1;
        number2 = 2;
        *nn_write_data_1 = number1;
        *nn_write_data_2 = number2;
        *nn_write_clock = 1;
        *nn_write_clock = 0;
    }


    // READING
    *nn_access = 1;
    *nn_read_enable = 1;
    *vga_data1 = 0;  // reset sdram
    *vga_data1 = 1;
    int8_t testRead1, testRead2;
    for (i = 0; i < 20; i = i + 2)
    {
        number1 = *nn_read_data_1;
        number2 = *nn_read_data_2;
        *nn_read_clock = 1;
        *nn_read_clock = 0;

        printf("%d\t", number1);
        printf("%d\n", number2);

    }



    *nn_write_enable = 0;
    *nn_bootup = 0;


    
    

    *nn_read_enable = 0;
    *nn_access = 0;







    int M;
    //int i = 0;
    int j = 0;
    int k = 0;
    int L = 0;
    int snapshot = 0;
    *nn_bootup = 0;
    *nn_access = 0;
    *vga_data1 = 0;
    *vga_data1 = 1;
    *vga_data2 = 1;
    *clock_select = 0;


    int write_data = 0;
    int written = 0;
    int height = HEIGHT, width = WIDTH;

    int** black_white = (int**)malloc(HEIGHT*sizeof(int*));
    for (i = 0; i < HEIGHT; i++)
    {
        black_white[i] = (int*)malloc(sizeof(int)*WIDTH);
    }

    while (1){
        *cam_start = 1;
        totalCycles = 0;
        printf("Press enter to start\n");

        fflush(stdin);
        getchar();
        fflush(stdin);



        // delay before capture
        for (i = 0; i < 30000; i++)
        {
        }
        snapshot = 1;
        if (snapshot)
        {
            //
            //      int cycleCounter = 0;
            //      int cycleIndex = 0;
            initCounters();
            main_1 = getCycles();

            *cam_start = 0; // pause camera
            *clock_select = 1;  // choose custom clock from hps
            *vga_data1 = 0; // reset sdram
            *vga_data1 = 1;
            *sdram_read = 1;  // set read request to high

            //
            //     main_2 = getCycles();

            // clear out first horizontal row, it is all black
            for (k = 0; k < WIDTH + 3; k = k + 1)
            {
                *clock_gen = 1; // generate 4 clock cycles to move slower clock 1 cycle
                *clock_gen = 0;
                *clock_gen = 1;
                *clock_gen = 0;

            }

            //      
            //      main_3 = getCycles();

            // begin reading in data
            for (j = 0; j < HEIGHT; j = j + 1)
            {
                for (k = 0; k < WIDTH; k = k + 1)
                {
                    for (L = 0; L < 2; L = L + 1)
                    {
                        *clock_gen = 1; // generate 4 clock cycles, checking each cycle
                        if (!written)
                        {
                            if (*read_good) // take in data from verilog to read block (not sure if needed)
                            {
                                black_white[j][k] = *(sdram_data1);
                                written = 1;
                            }
                        }
                        *clock_gen = 0;
                    }
                    written = 0;
                }
                *sdram_read = 0;
                *sdram_read = 1;
                /*
                //
                if (cycleCounter == 48)
                {
                cycleCounter = 0;
                cycle[cycleIndex] = getCycles();
                cycleIndex++;
                }
                */
            }

            // 
            //      main_3 = getCycles();

            *sdram_read = 0;

            *vga_data1 = 0;
            *vga_data2 = 0;
            *vga_data1 = 1;
            *vga_data2 = 1;

            *cam_start = 1;
            *clock_select = 0;
            snapshot = 0;
            //printf("Done\n");
        }

        *(sdram_read) = 0;

        //
        main_4 = getCycles();

        height = HEIGHT;
        width = WIDTH;
        
        printf("Image = %d  x %d\n", height, width);
        printf("\n\n");
        for (i = 0; i < height; i++)
        {
        for (k = 0; k < width; k++)
        {
        printf("%d\t", black_white[i][k]);
        }
        printf("\n");
        }/**/


        //printf("Total Image = %d   %d\n",height, width);
        region_1 = getCycles();
        region2(&width, &height, black_white);
        region_end = getCycles();
        /*
        printf("ROI = %d  x %d\n",height, width);
        printf("\n\n");
        for (i = 0; i < height; i++)
        {
        for (k = 0; k < width; k++)
        {
        printf("%d\t",black_white[i][k]);
        }
        printf("\n");
        } /* */
        //printf("Region Found\n\n");


        digit_separate2(height, width, black_white);
        //separate_end = getCycles();
        rec_2 = getCycles();
        printf("Done\n");

        /*    	for (i = 0; i < 10; i++)
        printf("%d:\t %u\n",i+1,cycle[i]);
        */
        //    printf("\n1: \t%d\n2: \t%d\n3: \t%d\n4: \n5: \n6: \t%d\n7: \t%d\n", main_1, main_2, main_3, main_6, main_7);

        final = (main_4 - main_1);
        //        printf("\nTimes:\nMain: \t%d\n\n", final);
        totalCycles += final;

        final = (region_end - region_1);
        totalCycles += final;
        //      printf("Region: \t%d\n", final);
        /*	printf("\tBreakdown:\n");
        printf("\t Large ROI:\t%d\n", LROIend - LROIstart);
        printf("\t Small ROI:\t%d\n", SROIend - SROIstart);
        printf("\t Array assignmet:\t%d\n\n", ROImovEnd - ROImovStart);
        */
        final = (separate_end - region_end);
        totalCycles += separate_end - separate_start;
        //        printf("Separator: \t%d\n", separate_end - separate_start);
        /*	printf("\t single digit detection:\t%d\n",detection_end - detection_start);
        printf("\t single digit normalization:\t%d\n",normalization_end - normalization_start);
        printf("\t digits separated:\t%d\n",digits_separated);
        */

        final = (rec_1 - resize_1);
        //totalCycles += resize_end - resize_start;
        //        printf("Resize: \t%d\n", resize_end - resize_start);
        /*	printf("\tBreakdown:\n");
        printf("\t resizing:\t%d\n", resizeEnd - resizeStart);
        printf("\t Array assignment:\t%d\n\n", resizeMovEnd - resizeMovStart);
        */

        final = (rec_2 - rec_1);
        totalCycles += recognizer_end - recognizer_start;
        //        printf("Recognize: \t%d\n", recognizer_end - recognizer_start);
        /*	printf("\tBreakdown:\n");
        printf("\t First matrix mult:\t%d\n", MMend1 - MMstart1);
        printf("\t First vector addition:\t%d\n", vend1 - vstart1);
        printf("\t First sigmoid:\t%d\n", sigEnd1 -sigStart1);
        printf("\t 2nd matrix mult:\t%d\n", MMend2 - MMstart2);
        printf("\t 2nd vector addition:\t%d\n", vend2 - vstart2);
        printf("\t 2nd sigmoid:\t%d\n", sigEnd2 - sigStart2);
        printf("\t 3rd matrix mult:\t%d\n", MMend3 - MMstart3);
        printf("\t Determining digit:\t%d\n\n", maxEnd - maxStart);
        */
        printf("Total Cycles: \t%d\n", totalCycles);
    }

    return 0;
}



void region2(int* width, int* height, int **mat)
{
    int cols = (*width);
    int rows = (*height);
    int xLeft, xRight, yTop, yBot;
    int r = HEIGHT / 2;
    int c = 0;
    int prev_hits = 0;
    int hits = 0;
    //printf("Initialized\n");

    //
    //	LROIstart = getCycles();

    // LROI Left Edge = xLeft
    for (c = 0; c < cols; c = c + 4)
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
    for (c = cols; c > 0; c = c - 4)
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
    c = WIDTH / 2;
    for (r = 0; r < rows; r = r + 4)
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
    for (r = rows - 1; r >= 0; r = r - 4)
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

    //
    //  LROIend = getCycles();

    printf("bottom = %d\n", yBot);
    // ROI Left Edge = xLeft




    r = (yBot + yTop) / 2;
    int tempxEdge = (xLeft + xRight) / 2;

    //
    //  SROIstart = getCycles();

    for (c = xLeft; c < xRight; c = c + 2)
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
    for (c = xRight - 1; c >= xLeft; c = c - 2)
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
    for (r = yTop; r < yBot; r = r + 2)
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
    for (r = yBot - 1; r >= yTop; r = r - 2)
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

    //
    //  SROIend = getCycles();

    printf("bot = %d\n", yBot);

    //printf("sizing\n");
    // Move region of interest to (0,0) of existing array mat[r][c]
    (*width) = xRight - xLeft;
    (*height) = yBot - yTop;

    //
    //  ROImovStart = getCycles();

    for (r = 0; r < *height; r++)
    {
        for (c = 0; c < *width; c++)
        {
            mat[r][c] = mat[yTop + r][xLeft + c];
        } // for (c = xLeft;...)
    } // for (r = yTop;...)

    //
    //  ROImovEnd = getCycles();

} // region





// nearest neighbor
// roughly 1,973,133 cycles
int resize2(int width, int height, int **digit) {

    resize_start = getCycles();
    //    printf("width = %d, height = %d\n", width, height);
    int scaled_img[28][28];
    int vect[784];
    int r = 0;
    int c = 0;
    int v_index = 0;
    float x_ratio = width / (double)28;
    float y_ratio = height / (double)28;
    //    printf("xratio = %f, yratio = %f\n", x_ratio, y_ratio);
    //    printf("sample, r = c = 24:   x2 = %d, y2 = %d\n", (int)floor(24 * x_ratio), (int)floor(24 * y_ratio));
    float x2, y2;
    for (r = 0; r<28; r++) {
        for (c = 0; c<28; c++) {
            x2 = floor(c*x_ratio);
            y2 = floor(r*y_ratio);
            scaled_img[r][c] = digit[(int)y2][(int)x2];
        }
    }

    for (c = 0; c < 28; c++)
    {
        for (r = 0; r < 28; r++)
        {
            vect[v_index] = scaled_img[r][c];
            v_index++;
        }
    }

    resize_end = getCycles();
    /*
    printf("resized image:\n");
    int i = 0;
    int k = 0;
    for (i = 0; i < 28; i++)
    {
    for (k = 0; k < 28; k++)
    {
    printf("%d\t", scaled_img[i][k]);
    }
    printf("\n");
    }
    */
    return recognizer(vect);
}


/*
// bilinear
int resize2(int width, int height, int **digit)
{
resize_start = getCycles();

int scaled_img[28][28];
int A, B, C, D, x, y, index, pixel;
float x_ratio = ((float)(width - 1)) / 28;
float y_ratio = ((float)(height - 1)) / 28;
float x_diff, y_diff;
int r = 0;
int c = 0;
int vect[784];
int v_index = 0;

for (r = 0; r<28; r++) {
for (c = 0; c<28; c++) {
x = (int)(x_ratio * c);
y = (int)(y_ratio * r);
x_diff = (x_ratio * c) - x;
y_diff = (y_ratio * r) - y;
index = y*w + x;

A = digit[r][c] & 0xff;
B = digit[r][c+1] & 0xff;
C = digit[r+1][c] & 0xff;
D = digit[r+1][c+1] & 0xff;

pixel = (int)(
A*(1 - x_diff)*(1 - y_diff) + B*(x_diff)*(1 - y_diff) +
C*(y_diff)*(1 - x_diff) + D*(x_diff*y_diff)
);

scaled_img[r][c] = pixel;
}
}

for (c = 0; c < 28; c++)
{
for (r = 0; r < 28; r++)
{
vect[v_index] = scaled_img[r][c];
v_index++;
}
}

resize_end = getCycles();
printf("resized image:\n");
int i = 0;
int k = 0;
for (i = 0; i < 28; i++)
{
for (k = 0; k < 28; k++)
{
printf("%d\t", scaled_img[i][k]);
}
printf("\n");
}

return recognizer(vect);

}
*/


/*
// chan's algorithm
int resize2(int height, int width, int** img){
resize_start = getCycles();
int scaled_img[28][28];
int vector[784];

//
//col and row specify squares. p examines pixels. s is used for the scaled img. v is vector index.
//

int col, row, p_col, p_row, s_col = 0, s_row = 0, v_index;

//Scales divide by 28 rounded up.
int col_scale = (width + 27) / 28;
int row_scale = (height + 27) / 28;

//avg is used to calculate the average white density of a square, given by size square.
double avg;
double square = col_scale * row_scale;


//
// Initialize the image to be all black
//
int i, j;

//
resizeStart = getCycles();
for (i = 0; i < 28; i++){

for (j = 0; j < 28; j++)
scaled_img[i][j] = BLACK;

}
printf("initialized black box\n");
//
//Examine the image, magnifying rectangle by rectangle down to 28 = (size - size%28)/scale
//where size is the row or column and the scale is the calculated scale rounded up.
//

for (row = 0; row < (height - (height % 28)); row += row_scale){

s_col = 0;

for (col = 0; col < (width - (width % 28)); col += col_scale)
{

avg = 0;

//
//Calculate the average of a square given starting coordinates
//

for (p_row = 0; p_row < row_scale && ((row + p_row) < height); p_row++)
{
for (p_col = 0; p_col < col_scale && ((col + p_col) < width); p_col++)
{
avg += img[row + p_row][col + p_col];
}
}

avg = avg / square;

if (avg >= 0.5)
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
//
resizeEnd = getCycles();

printf("resized\n");
//
//Convert the scaled image to a vector for recognizer to use
//
resizeMovStart = getCycles();
v_index = 0;
for (s_col = 0; s_col < 28; s_col++)
{
for (s_row = 0; s_row < 28; s_row++)
{
vector[v_index] = scaled_img[s_row][s_col];
v_index++;
}
}

//
resizeMovEnd = getCycles();

int k = 0;
for (i = 0; i < 28; i++)
{
for (k = 0; k < 28; k++)
{
printf("%d\t",scaled_img[i][k]);
}
printf("\n");
}
resize_end = getCycles();
return recognizer(vector);
}
*/



int recognizer(int data[784])
{
    
    long int Vb1[200], Vb2[200], Vb3[10]; // array[row][col]
    int M = 0;
    int i, j;
    long double sum = 0;

    //
    MMstart1 = getCycles();
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

    //
    MMend1 = getCycles();

    //
    vstart1 = getCycles();
    //Vb1 = Vb1 + finalB1L1;
    for (i = 0; i < 200; i++)
    {
        Vb1[i] = Vb1[i] + finalB1L1[i];
    } // Matrix Addition
    //
    vend1 = getCycles();
    //
    sigStart1 = getCycles();

    //Vb1 = sigmf(Vb1,[1 0]);
    for (i = 0; i < 200; i++)
    {
        Vb1[i] = 1 / (1 + exp(-Vb1[i]));
    } // Sigmoid
    //  
    sigEnd1 = getCycles();
    //
    MMstart2 = getCycles();
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
    //  
    MMend2 = getCycles();
    //
    vstart2 = getCycles();
    //Vb1 = Vb1 + finalB1L2;
    for (i = 0; i < 200; i++)
    {
        Vb2[i] = Vb2[i] + finalB1L2[i];
    } // Matrix Addition
    // 
    vend2 = getCycles();
    //
    sigStart2 = getCycles();

    //Vb1 = sigmf(Vb1,[1 0]);
    for (i = 0; i < 200; i++)
    {
        Vb2[i] = 1 / (1 + exp(-Vb2[i]));
    } // Sigmoid
    //
    sigEnd2 = getCycles();
    //
    MMstart3 = getCycles();

    //Vb1 = finalSoftmaxTheta*Vb1;          finalSoftmaxTheta[10][200]
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 200; j++)
        {
            sum = sum + finalSoftmaxTheta[i][j] * Vb2[j];
        } //
        Vb3[i] = sum;
        sum = 0;
    } // 
    //  
    MMend3 = getCycles();
    // 
    maxStart = getCycles();


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
    //
    maxEnd = getCycles();

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
}

void digit_separate2(int num_row, int num_col, int **roi)
{
    separate_start = getCycles();
    int mid_row = num_row / 2;
    int c = 0;

    int ***digit;

    int right_edge = 0;
    int bad = 0;
    int first = 1;
    int digit_num = 0;
    int row = 0;
    int col = 0;

    int last_pixel = 0;
    int pixel = 0;
    int mid_col = 0;
    int last_mid = 0;


    int digit_size[MAX_DIGITS] = { 0 };
    int digit_top = 0;
    int digit_bot = 0;
    int digit_left = 0;
    int digit_right = 0;
    int digit_height = 0;
    int digit_width = 0;
    int padding = 0;
    int horz_padding = 0;
    int guess[MAX_DIGITS] = { 0 };
    int digits_detected = 0;

    digit = (int ***)malloc(MAX_DIGITS*sizeof(int **));
    for (c = 2; c < num_col - 2; c = c + 2)
    {
        detection_start = getCycles();
        if ((roi[mid_row][c] == WHITE) || (c + 4 >= num_col - 2)) // hit white or end of roi
        {
            if (first) // skip the first hit of white
            {
                first = 0;
                pixel = WHITE;
            }
            else
            {
                if (last_pixel == BLACK) // check if start of digit found
                {
                    mid_col = (c - right_edge) / 2 + right_edge;
                    if (roi[mid_row][c] == WHITE) // skip this section if we've hit the end of the ROI
                    {
                        // start at 4 to size_y-4 to give some buffer for tilted ROI's
                        for (row = 4; row < num_row - 4; row++)
                        {
                            if (roi[row][mid_col] == WHITE)
                            {
                                bad = 1;
                                break;
                            }
                        }
                    } // if (roi[r][c] == WHITE)
                    detection_end = getCycles();
                    if (!bad)
                    {
                        normalization_start = getCycles();
                        digits_detected++;
                        if (digit_num < MAX_DIGITS)
                        {
                            // check for top
                            for (row = 2; row < num_row - 2; row++)
                            {
                                for (col = last_mid; col < mid_col; col++)
                                {
                                    if (roi[row][col] == WHITE)
                                    {
                                        digit_top = row;
                                        // break out of for loops
                                        col = mid_col;
                                        row = num_row;
                                    }
                                }
                            }
                            // check for bot
                            for (row = num_row - 2 - 1; row >= 2; row--)
                            {
                                for (col = last_mid; col < mid_col; col++)
                                {
                                    digit_bot = row;
                                    if (roi[row][col] == WHITE)
                                    {
                                        col = mid_col;
                                        row = -1;
                                    }
                                }
                            }

                            digit_height = digit_bot - digit_top;
                            padding = digit_height / 5;

                            digit_size[digit_num] = digit_height + padding + padding;


                            // check for left
                            for (col = last_mid; col < mid_col; col++)
                            {
                                for (row = 2; row < num_row - 2; row++)
                                {
                                    if (roi[row][col] == WHITE)
                                    {
                                        digit_left = col;
                                        row = num_row;
                                        col = mid_col;
                                    }
                                }
                            }
                            // check for right
                            for (col = mid_col - 1; col >= last_mid; col--)
                            {
                                for (row = 2; row < num_row - 2; row++)
                                {
                                    if (roi[row][col] == WHITE)
                                    {
                                        digit_right = col;
                                        last_pixel = WHITE;							row = num_row;
                                        col = last_mid - 1;
                                    }
                                }
                            }
                            digit_width = digit_right - digit_left;
                            horz_padding = (digit_size[digit_num] - digit_width) / 2;


                            int i = 0;
                            int j = 0;

                            // allocate space for digits
                            digit[digit_num] = (int **)malloc(digit_size[digit_num] * sizeof(int*));
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



                            // write digit to middle of black box
                            for (i = padding; i < padding + digit_height; i++)
                            {
                                for (j = horz_padding; j < horz_padding + digit_width; j++)
                                {
                                    digit[digit_num][i][j] = roi[digit_top + i - padding][digit_left + j - horz_padding];
                                }
                            }
                            normalization_end = getCycles();

                            // ===========================
                            // print digit if checking
                            /*                            printf("digit number: %d\n", digit_num);


                            for (i = 0; i < digit_size[digit_num]; i++)
                            {
                            for (j = 0; j < digit_size[digit_num]; j++)
                            {
                            printf("%d\t", digit[digit_num][i][j]);
                            }
                            printf("\n");
                            }
                            printf("\n\n\n");/* */
                            // ==========================

                            // resize digit

                            // pass digit through NN and receive integer
                            // integer = function()

                            // print single digit

                        } // if (digit_num < MAX_DIGITS
                        last_mid = mid_col;
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
    printf("Digits found: %i\n", digits_detected);

    if (digits_detected > MAX_DIGITS)
        digits_detected = MAX_DIGITS;
    // ===========
    separate_end = getCycles();
    // ===========

    /*
    int i = 0;
    int j = 0;
    for (c = 0; c < digits_detected; c++)
    {
    printf("digit number: %d\n", c+1);
    for (i = 0; i < digit_size[c]; i++)
    {
    for (j = 0; j < digit_size[c]; j++)
    {
    printf("%d\t", digit[c][i][j]);
    }
    printf("\n");
    }
    printf("\n\n\n");
    }/**/

    recognizer_start = getCycles();
    for (c = 0; c < digits_detected; c++)
    {

        guess[c] = resize2(digit_size[c], digit_size[c], digit[c]);
        printf("Guess = %d\n", guess[c]);
    }
    recognizer_end = getCycles();
} // digit_separate2






static inline unsigned int getCycles()
{
    unsigned int cycleCount;
    // Read CCNT register
    asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(cycleCount));
    return cycleCount;
}





static inline void initCounters()
{
    // Enable user access to performance counter
    asm volatile ("MCR p15, 0, %0, C9, C14, 0\t\n" :: "r"(1));
    // Reset all counters to zero
    int MCRP15ResetAll = 23;
    asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(MCRP15ResetAll));
    // Enable all counters:
    asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));
    // Disable counter interrupts
    asm volatile ("MCR p15, 0, %0, C9, C14, 2\t\n" :: "r"(0x8000000f));
    // Clear overflows:
    asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
}
