// fast_read.c 

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
/**/
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

#define IMG_DATA 0xFF200170
#define READY_FOR_DATA 0xFF200160
#define DATA_QUEUE_RESET 0xFF200150

#define SIZING_DONE 0xFF2001C0
#define SIZING_START 0xFF2001D0
#define SIZING_LEFT 0xFF2001B0
#define SIZING_RIGHT 0xFF2001A0
#define SIZING_TOP 0xFF200190
#define SIZING_BOT 0xFF200180





// Computing ROI and Separate Images
int w, x, y, v, lt, lb, rt, rb;


// Timing Variables
unsigned long int reading_start = 0;
unsigned long int reading_end = 0;
unsigned long int region_start = 0;
unsigned long int region_end = 0;
unsigned long int separate_start = 0;
unsigned long int separate_end = 0;
unsigned long int recognizer_start = 0;
unsigned long int recognizer_end = 0;
unsigned long int total_start = 0;
unsigned long int total_end = 0;
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
    volatile int * (sdram_reset) = (int *)VGA_DATA1; //Output
    volatile int * (vga_reset) = (int *)VGA_DATA2; //Output
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

    volatile int *img_data = (int *)IMG_DATA;
    volatile int *ready_for_data = (int *)READY_FOR_DATA;
    volatile int *data_reset = (int *)DATA_QUEUE_RESET;

    volatile int *sizing_done = (int *)SIZING_DONE;
    volatile int *sizing_start = (int *)SIZING_START;
    volatile int *roi_left = (int *)SIZING_LEFT;
    volatile int *roi_right = (int *)SIZING_RIGHT;
    volatile int *roi_top = (int *)SIZING_TOP;
    volatile int *roi_bot = (int *)SIZING_BOT;



    *nn_write_enable = 0;
    *nn_bootup = 0;
    *nn_read_enable = 0;
    *nn_access = 0;
    *cam_start = 0;
    *ready_for_data = 1;
    *data_reset = 1;


    int pre_split = 0;
    int M = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int L = 0;
    int l = 0;
    int snapshot = 0;

    *nn_bootup = 0;
    *nn_access = 0;
    *sdram_reset = 0;
    *sdram_reset = 1;
    *vga_reset = 1;
    *clock_select = 0;
    *ready_for_data = 1;


    int write_data = 0;
    int written = 0;
    int height = HEIGHT, width = WIDTH;
    int printed_rows = 0;




    int** image = (int**)malloc(HEIGHT*sizeof(int*));
    for (i = 0; i < HEIGHT; i++)
    {
        image[i] = (int*)malloc(sizeof(int)*WIDTH);
    }

    while (1){
        *cam_start = 1;
        totalCycles = 0;
        printed_rows = 0;

        height = HEIGHT;
        width = WIDTH;

        printf("Press enter to start\n");

        fflush(stdin);
        getchar();
        fflush(stdin);



        // delay before capture
        for (i = 0; i < 3000; i++)
        {
        }


        initCounters();
        total_start = getCycles();
        reading_start = getCycles();

        *cam_start = 0; // pause camera
        *clock_select = 1;  // choose custom clock from hps
        *sdram_reset = 0; // reset sdram
        *sdram_reset = 1;
        *sdram_read = 1;  // set read request to high
        *data_reset = 0;
        *data_reset = 1;
        for (l = 0; l < 2; l = l + 1)
        {

        }

        // throwing away the first row and offsets
        for (j = 0; j < 6; j = j + 1)
        {
            *ready_for_data = 1;
            *ready_for_data = 0;
            *ready_for_data = 1;
            *ready_for_data = 0;
            for (l = 0; l < 2; l = l + 1)
            {
                // delay enough time for the module to grab 32 bits
            }
            pre_split = *img_data;
        }

        // Reading in image data
        for (j = 0; j < HEIGHT; j = j + 1)
        {
            for (k = 0; k < WIDTH / 32; k = k + 1)
            {
                *ready_for_data = 1;
                *ready_for_data = 0;
                *ready_for_data = 1;
                *ready_for_data = 0;
                for (l = 0; l < 2; l = l + 1)
                {
                    // delay enough time for the module to grab the 32 bits
                }
                pre_split = *img_data;
                for (l = 0; l < 32; l = l + 1)
                {
                    image[j][k * 32 + l] = (pre_split >> l) & 0x01;
                }

            }
        }
        //*data_reset = 0;
        //*data_reset = 1;


        *sdram_read = 0;

        *sdram_reset = 0;
        *vga_reset = 0;
        *sdram_reset = 1;
        *vga_reset = 1;

        *cam_start = 1;


        *clock_select = 0;
        //printf("Done\n");

        //
        reading_end = getCycles();


        /*
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


        region_start = getCycles();
        region2(&width, &height, image);
        region_end = getCycles();

        /*printf("ROI = %d  x %d\n", height, width);
        printf("\n\n");
        for (i = 0; i < height; i++)
        {
            for (k = 0; k < width; k++)
            {
                printf("%d\t", image[i][k]);
            }
            printf("\n");
        } /* */
        //printf("Region Found\n\n");


        digit_separate2(height, width, image);

        total_end = getCycles();

        printf("Done\n");
        printf("Reading image = %d\n", reading_end - reading_start);
        printf("region = %d\n", region_end - region_start);
        printf("separate = %d\n", separate_end - separate_start);
        printf("recognizer = %d\n", recognizer_end - recognizer_start);




        totalCycles = total_end - total_start;

        printf("Total Cycles: \t%d\n", totalCycles);
        /*
        for (i = 0; (i < height); i++)
        {
        for (k = 0; k < width; k++)
        {
        printf("%d\t", image[i][k]);
        }
        printf("\n");
        //printed_rows = printed_rows + 1;
        }/**/

    }

    return 0;
}



// ======================================================================================
// ======================================================================================



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
    //printf("left edge = %d\n", xLeft);

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
    //printf("right edge = %d\n", xRight);

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
    //printf("top edge = %d\n", yTop);

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

    //printf("bottom = %d\n", yBot);
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
    //printf("left = %d\n", xLeft);

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
    //printf("right = %d\n", xRight);

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
    //printf("top = %d\n", yTop);

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

    //printf("bot = %d\n", yBot);

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


    /*printf("resized image:\n");
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
    /**/
    return recognizer(vect);
}






int recognizer(int data[784])
{

    long int Vb1[200] = { 0 }, Vb2[200] = { 0 }, Vb3[10] = { 0 }; // array[row][col]
    int M = 0;
    int i, j;
    long double sum = 0;


    //
    // Vb1 = finalW1L1*data;
    for (i = 0; i < 200; i++)
    {

        for (j = 0; j < 784; j++)
        {
            if (data[j])
                Vb1[i] = Vb1[i] + finalW1L1[i][j];
        } // Matrix Multiplication

        Vb1[i] = Vb1[i] + finalB1L1[i]; // add biases
        Vb1[i] = 1 / (1 + exp(-Vb1[i])); // sigmoid
    } // Product into new Matrix


    //
    //
    //Vb1 = finalW1L2*Vb1;
    for (i = 0; i < 200; i++)
    {

        for (j = 0; j < 200; j++)
        {
            if (Vb1[j] != 0)
                Vb2[i] = Vb2[i] + finalW1L2[i][j] * Vb1[j];
        } // Matrix Multiplication
        Vb2[i] = Vb2[i] + finalB1L2[i];
        Vb2[i] = 1 / (1 + exp(-Vb2[i]));
    } // Product into old Matrix

    //
    //

    //Vb1 = finalSoftmaxTheta*Vb1;          finalSoftmaxTheta[10][200]
    for (i = 0; i < 10; i++)
    {

        for (j = 0; j < 200; j++)
        {
            if (Vb2[j] != 0)
                Vb3[i] = Vb3[i] + finalSoftmaxTheta[i][j] * Vb2[j];
        } //

    } //
    //
    //


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

    // Check for Zero
    //if(M == 10)
    //  M = 0;
    //end
    if (M == 10)
    {
        M = 0;
    } // Check for zero


    //output = M;
    /**/
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
                    if (!bad)
                    {
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
    printf("Guess = ");
    recognizer_start = getCycles();
    for (c = 0; c < digits_detected; c++)
    {
        guess[c] = resize2(digit_size[c], digit_size[c], digit[c]);
        printf("%d", guess[c]);
    }

    printf("\n");

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
