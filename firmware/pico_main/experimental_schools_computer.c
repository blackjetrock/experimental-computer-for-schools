////////////////////////////////////////////////////////////////////////////////
//
//  PRO-101 Recreation PCB Specific Code
//
////////////////////////////////////////////////////////////////////////////////
//
// Simulates a PRO-101
//
// Runs with the original (or recreated) PRO-101 keyboard.
// Uses original (or FX201/202/PRO FX-1) VFD display
// 
//
////////////////////////////////////////////////////////////////////////////////

#include "switches.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/flash.h"
#include "hardware/structs/bus_ctrl.h"

#define DEBUG_STOP while(1) {}

// Some logic to analyse:
#include "hardware/structs/pwm.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

#include "icd/Icd.h"
#include "fx201p.h"
#include "fx201p_execution.h"
#include "pro101_recreate.h"

#include "pro_101_common.h"

// Single shot trae, don't repeatedly capture data
#define SINGLE_SHOT     0

// whether to display bus transations over USB, it can mess the CLI up
#define NO_TRACE_USB    1

// On falling edge of CE, are both cycle counters reset?
#define RESET_BOTH_CYCLES	  1

#define EMULATE_FX201P            1

#define NUM_KBD_LINES (sizeof(kbd_lines)/sizeof(int))
#define NUM_KBD_DRV 5

////////////////////////////////////////////////////////////////////////////////
//
// Command codes
//
////////////////////////////////////////////////////////////////////////////////

#define CMD_CLR      0x94          // Zero all
#define CMD_CLR_M    0x95          // Zero memories
#define CMD_CLR_P    0x96          // Zero program

#define CMD_LOAD     0x11          // Loads program and memory areas
#define CMD_LOAD_M   0x12          // Loads memory areas
#define CMD_LOAD_P   0x13          // Loads program areas

#define CMD_SAVE     0x15          // Saves program and memory areas
#define CMD_SAVE_M   0x16          // Saves memory areas
#define CMD_SAVE_P   0x17          // Saves program areas

#define CMD_CATALOG  0x47          // Shows empty slots

#define STATUS_CHAR_GOOD 0x0B
#define STATUS_CHAR_BAD  0x0D

#define GET_SPLIT_BYTE(NNN) (((*(command_ptr+(NNN+1))) & 0x0F) << 4) + (((*(command_ptr+NNN)) & 0xF0)>>4);

////////////////////////////////////////////////////////////////////////////////
//
// Test sequences
// We run these sequences of keys and check the X and Y registers and stores against
// the expected results
//
// CTOK_TEST in the tokens means test result now. Resulkts also tested at end
//
////////////////////////////////////////////////////////////////////////////////

#define SEQ_NONE -1
#define TEST_E   0.00001

int test_sequence = SEQ_NONE;
int test_sequence_all = 0;
int test_result_i = 0;

int seq_i = 0;

int held_key = 0;

typedef struct _TEST_RESULTS
{
  int test_x;
  double x;
  char * display;
} TEST_RESULTS;

typedef struct _TEST_INFO
{
  char         *desc;
  TOKEN        *seq;
  TEST_RESULTS *results;
  char         actual_results_display[DISPLAY_WIDTH+1];
  int          passed;
  int          approx_passed;
} TEST_INFO;

////////////////////////////////////////////////////////////////////////////////

// Simple calculation
TOKEN seq0[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2, TOK_PL, TOK_3, TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq0_res[] =
  {
   1, 5.0,
   NULL,
  };

// Negative result
TOKEN seq1[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2, TOK_MN, TOK_3, TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq1_res[] =
  {
   1, -1.0,
   NULL,
  };

// Longer calculation
TOKEN seq2[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2,     TOK_PL, TOK_3,
   TOK_MN,    TOK_1,
   TOK_DIV,   TOK_2,
   TOK_TIMES, TOK_3,
   TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq2_res[] =
  {
   1, 6.0,   
   NULL,
  };

// Square root

TOKEN seq3[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2,     TOK_5, TOK_SQRT,
   TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq3_res[] =
  {
   1, 5.0,   
   NULL,
  };

// Powers
TOKEN seq4[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2,     TOK_XY, TOK_5,
   TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq4_res[] =
  {
   0, 32.0,   
   "            32",
  };

// Constant function
TOKEN seq5[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2,     TOK_TIMES, TOK_TIMES, TOK_5,
   TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq5_res[] =
  {
   1, 10.0,   
   NULL,
  };

// Constant function
TOKEN seq6[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2,     TOK_TIMES, TOK_TIMES, TOK_5,
   TOK_EQ,
   // This C shouldn't be needed
   //TOK_C,
   TOK_2, TOK_0,    TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq6_res[] =
  {
   1, 40.0,   
   NULL,
  };

TOKEN seq7[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2, TOK_5, TOK_0, TOK_0,
   TOK_TIMES,
   TOK_5, TOK_0,
   TOK_PERCENT,
   TOK_NO_TOK,
  };

TEST_RESULTS seq7_res[] =
  {
   1, 1250.0,   
   NULL,
  };

TOKEN seq8[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2, TOK_5, TOK_0, TOK_0,
   TOK_TIMES,
   TOK_2, TOK_5,
   TOK_PERCENT,
   TOK_MN,
   TOK_NO_TOK,
  };

TEST_RESULTS seq8_res[] =
  {
   1, 1875.0,   
   NULL,
  };

TOKEN seq9[] =
  {
   TOK_COMP_MODE,
   TOK_AC,
   TOK_PRN,
   TOK_NO_TOK,
  };

TEST_RESULTS seq9_res[] =
  {
   0, 40.0,   
   "3810         P",
  };

TOKEN seq10[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2, TOK_5, TOK_0, TOK_0,
   TOK_TIMES,
   TOK_2, TOK_5,
   TOK_PERCENT,
   TOK_PL,
   TOK_NO_TOK,
  };

TEST_RESULTS seq10_res[] =
  {
   1, 3125.0,   
   NULL,
  };

TOKEN seq11[] =
  {
   TOK_AC,
   TOK_COMP_MODE,
   TOK_2, TOK_5,
   TOK_PL,
   TOK_1, TOK_0,
   TOK_EQ,
   TOK_DIV,
   TOK_5,
   TOK_EQ,
   TOK_NO_TOK,
  };

TEST_RESULTS seq11_res[] =
  {
   1, 7.0,   
   NULL,
  };

TOKEN seq12[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_2, TOK_3, TOK_COLON,
   TOK_ANS, TOK_1, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq12_res[] =
  {
   1, 123.0,   
   NULL,
  };

TOKEN seq_ans[] =
  {
   TOK_ANS,
   TOK_NO_TOK,
  };

TEST_RESULTS seq_ans_res[] =
  {
   0, 123.0,   
   NULL,
  };

TOKEN seq14[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_0, TOK_COLON,
   TOK_IF, TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_0, TOK_COLON, TOK_1, TOK_COLON, TOK_2, TOK_COLON, TOK_3, TOK_COLON,

   TOK_ST, TOK_1, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_6, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_2, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_7, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_3, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_8, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,
   

   TOK_ST, TOK_9, TOK_COLON,
   TOK_ANS, TOK_2, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq14_res[] =
  {
   0, 123.0,   
   " 2           7",
  };

TOKEN seq16[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_0, TOK_COLON,
   TOK_IF, TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_1, TOK_COLON, TOK_1, TOK_COLON, TOK_2, TOK_COLON, TOK_3, TOK_COLON,

   TOK_ST, TOK_1, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_6, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_2, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_7, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_3, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_8, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,
   

   TOK_ST, TOK_9, TOK_COLON,
   TOK_ANS, TOK_2, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq16_res[] =
  {
   0, 123.0,   
   " 2           6",
  };

TOKEN seq18[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_2, TOK_COLON,
   TOK_IF, TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_1, TOK_COLON, TOK_1, TOK_COLON, TOK_2, TOK_COLON, TOK_3, TOK_COLON,

   TOK_ST, TOK_1, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_6, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_2, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_7, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_3, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_8, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_9, TOK_COLON,
   TOK_ANS, TOK_2, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,

   CTOK_TEST,
   
   // Same calculation but swapped round, so result will be different

   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_2, TOK_COLON,
   TOK_IF, TOK_K, TOK_1, TOK_0, TOK_1, TOK_EQ, TOK_1, TOK_COLON, TOK_1, TOK_COLON, TOK_2, TOK_COLON, TOK_3, TOK_COLON,

   TOK_ST, TOK_1, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_6, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_2, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_7, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_3, TOK_COLON,
   TOK_2, TOK_EQ, TOK_K, TOK_8, TOK_COLON,
   TOK_GOTO, TOK_9, TOK_COLON,

   TOK_ST, TOK_9, TOK_COLON,
   TOK_ANS, TOK_2, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq18_res[] =
  {
  {
   0, 123.0,   
   " 2           8",
  },
  {
   0, 123.0,   
   " 2           6",
  },
  };

// Assignment arithmetic

TOKEN seq20[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,   

   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_COLON,
   TOK_1, TOK_EQ, TOK_1, TOK_PL, TOK_K, TOK_1, TOK_COLON,

   // Skip over another increment
   TOK_GOTO, TOK_1, TOK_COLON,

   TOK_1, TOK_EQ, TOK_1, TOK_PL, TOK_K, TOK_1, TOK_COLON,
   
   TOK_ST, TOK_1, TOK_COLON,
   TOK_ANS, TOK_1, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq20_res[] =
  {
   1, 11.0,   
   NULL,
  };

// Manual jump

TOKEN seq22[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   
   // Put 10 in store 1
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_COLON,

   // Increment it but also have the manual jump label here
   TOK_MJ,
   TOK_1, TOK_EQ, TOK_1, TOK_PL, TOK_K, TOK_1, TOK_COLON,

   // Program will stop here, we can then run a MJ to increment again
   TOK_ANS, TOK_1, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq22_res[] =
  {
   1, 11.0,   
   NULL,
  };

// exit program and do manual jump

TOKEN seq23[] =
  {
   // Exit the previous program
   TOK_ANS,
   TOK_AC,
   
   // Do a manual jump
   TOK_MJ,

   // Done
   TOK_NO_TOK,
  };

TEST_RESULTS seq23_res[] =
  {
   1, 12.0,   
   NULL,
  };

// Indirect register access

TOKEN seq25[] =
  {

   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,
   
   // Put 10 in store 1
   TOK_I, TOK_EQ, TOK_K, TOK_1, TOK_0, TOK_COLON,

   // Put 321 in store 10 indirectly
   TOK_IM, TOK_EQ, TOK_K, TOK_3, TOK_2, TOK_1, TOK_COLON,

   // Program will stop here, we can then run a MJ to increment again
   TOK_ANS, TOK_1, TOK_0, TOK_COLON, TOK_END,
   TOK_COMP_MODE,
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq25_res[] =
  {
   1, 321.0,   
   NULL,
  };

// CALL test
TOKEN seq27[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   TOK_AC,
   
   TOK_PRN, TOK_2, TOK_COLON,
   TOK_AC,
   
   // Put 10 in store I
   TOK_1, TOK_EQ, TOK_K, TOK_1, TOK_2, TOK_COLON,

   TOK_END,
   
   TOK_COMP_MODE,

   TOK_AC,
   TOK_WRITE_MODE,
   TOK_AC,
   TOK_PRN, TOK_1, TOK_COLON,

   TOK_CALL, TOK_2, TOK_COLON,
   TOK_ANS, TOK_1, TOK_COLON,
   TOK_END,
   
   TOK_COMP_MODE,

   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_NO_TOK,
  };

TEST_RESULTS seq27_res[] =
  {
   1, 12.0,   
   NULL,
  };

// CUT and 5/4

TOKEN seq29[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_3,
   TOK_CUT_F,

   // Add 1/7 rounded in different ways, adding results to the
   // memory. We then check the final result
   TOK_1, TOK_DIV, TOK_7, TOK_EQ,
   TOK_MEM_PL,

   TOK_AC,
   TOK_1, TOK_DIV, TOK_7, TOK_EQ_2,
   TOK_MEM_PL,

   TOK_AC,
   TOK_CUT_CUT,
   TOK_1, TOK_DIV, TOK_7, TOK_EQ,
   TOK_MEM_PL,

   TOK_AC,
   TOK_CUT_54,
   TOK_1, TOK_DIV, TOK_7, TOK_EQ,
   TOK_MEM_PL,

   TOK_AC,
   TOK_CUT_CUT,
   TOK_1, TOK_DIV, TOK_7, TOK_EQ_2,
   TOK_MEM_PL,

   TOK_AC,
   TOK_CUT_54,
   TOK_1, TOK_DIV, TOK_7, TOK_EQ_2,
   TOK_MEM_PL,

   // The result is in the memory
   TOK_AC,
   TOK_MEM_REC,

   TOK_NO_TOK,
  };

TEST_RESULTS seq29_res[] =
  {
   0, 12.0,   
   "  085642857142",
  };


// Examples Page 5

TOKEN seq30[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_3,
   TOK_CUT_F,

   TOK_5, TOK_3,
   TOK_PL,
   TOK_1, TOK_2, TOK_DOT, TOK_3, 
   TOK_MN,
   TOK_4, TOK_2,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq30_res[] =
  {
   0, 23.3,   
   "           233",
  };

TOKEN seq31[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_3,
   TOK_CUT_F,

   TOK_5, TOK_6,
   TOK_TIMES,
   TOK_7, TOK_DOT, TOK_8, TOK_9,  
   TOK_DIV,
   TOK_9, TOK_DOT, TOK_6, TOK_3,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq31_res[] =
  {
   0, 45.8816199376,   
   "  458816199376",
  };

TOKEN seq32[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_3,
   TOK_CUT_F,

   TOK_5, TOK_6,
   TOK_MN,
   TOK_2, TOK_3,
   TOK_TIMES,
   TOK_9, TOK_6, TOK_3,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq32_res[] =
  {
   1, 31779,   
   NULL,
  };

TOKEN seq33[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_3,
   TOK_CUT_F,

   TOK_7, TOK_8, TOK_9, TOK_4,
   TOK_DIV,
   TOK_6,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq33_res[] =
  {
   0, 31779,   
   "  131566666666",
  };

TOKEN seq34[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_CUT,

   TOK_7, TOK_8, TOK_9, TOK_4,
   TOK_DIV,
   TOK_6,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq34_res[] =
  {
   0, 31779,   
   "        131566",
  };

TOKEN seq35[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_54,

   TOK_7, TOK_8, TOK_9, TOK_4,
   TOK_DIV,
   TOK_6,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq35_res[] =
  {
   0, 31779,   
   "        131567",
  };

TOKEN seq36[] =
  {
   TOK_COMP_MODE,
   TOK_AC,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_54,

   TOK_7, TOK_8, TOK_9, TOK_4,
   TOK_DIV,
   TOK_6,
   TOK_EQ_2,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq36_res[] =
  {
   0, 31779,   
   "  131566666666",
  };

// Error condition
TOKEN seq37[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_54,

   TOK_0,
   TOK_XY,
   TOK_0,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq37_res[] =
  {
   0, 31779,   
   "             E",
  };


// Negative numbers in CUT mode 54

TOKEN seq38[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_54,

   TOK_2,
   TOK_MN,
   TOK_3,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq38_res[] =
  {
   0, 31779,   
   "            -1",
  };


// Negative numbers in CUT mode 54

TOKEN seq39[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_F,

   TOK_1, TOK_5, TOK_0,
   TOK_TIMES,
   TOK_1, TOK_DOT, TOK_0, TOK_0, TOK_0,
   TOK_EQ,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq39_res[] =
  {
   0, 31779,   
   "           150",
  };


// Example page 9

TOKEN seq40[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_F,

   TOK_1, TOK_5, TOK_0,
   TOK_TIMES,
   TOK_6, TOK_DOT, TOK_5,
   TOK_MEM_PL,

   TOK_7, TOK_0,
   TOK_TIMES,
   TOK_1, TOK_4, TOK_DOT, TOK_2, TOK_5,
   TOK_MEM_PL,
   
   TOK_4, TOK_8, TOK_0,
   TOK_TIMES,
   TOK_5, TOK_5,
   TOK_MEM_PL,

   TOK_MEM_REC,
   TOK_TIMES, TOK_6, TOK_PERCENT,
   TOK_PL,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq40_res[] =
  {
   0, 31779,   
   "       3007485",
  };

// Example page 9

TOKEN seq41[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_F,

   TOK_7, TOK_4, TOK_1,
   TOK_PL,
   TOK_2, TOK_1, TOK_0,
   TOK_PL,
   TOK_5, TOK_4, TOK_9,
   TOK_DIV,

   TOK_1, TOK_0, TOK_0,
   TOK_DIV,
   TOK_DIV,
   TOK_7, TOK_4, TOK_1,
   TOK_MEM_PL,
   TOK_2, TOK_1, TOK_0,
   TOK_MEM_PL,
   TOK_5, TOK_4, TOK_9,
   TOK_MEM_PL,
   
   TOK_MEM_REC,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq41_res[] =
  {
   0, 31779,   
   "           100",
  };

// Example page 8

TOKEN seq42[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_F,

   TOK_2, TOK_XY, TOK_XY, TOK_3, TOK_MEM_PL,
   TOK_4, TOK_MEM_PL,
   TOK_5, TOK_MEM_PL,
   TOK_6, TOK_MEM_PL,
   
   TOK_MEM_REC,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq42_res[] =
  {
   0, 31779,   
   "            86",
  };

// Example page 8

TOKEN seq43[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_MEM_CLR,
   TOK_PLACES_2,
   TOK_CUT_F,

   TOK_7, 
   TOK_MEM_PL,
   TOK_MEM_PL,
   TOK_MEM_MN,
   TOK_2, TOK_TIMES, TOK_3,
   TOK_MEM_PL,
   TOK_MEM_PL,
   
   TOK_MEM_REC,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq43_res[] =
  {
   0, 31779,   
   "            19",
  };

#if DISPLAY_WRITE_PRO101

// PRO-101 type program entry

TOKEN seq_p44[] =
  {
   TOK_AC,
   TOK_WRITE_MODE,
   
   TOK_PRN, TOK_1, TOK_COLON,
   TOK_AC,

   CTOK_TEST,
   
   TOK_1,
   CTOK_TEST,

   TOK_EQ,
   CTOK_TEST,

   TOK_K,
   CTOK_TEST,

   TOK_1,
   CTOK_TEST,

   TOK_2,
   CTOK_TEST,

   TOK_3,
   CTOK_TEST,

   TOK_4,
   CTOK_TEST,

   TOK_COLON,
   CTOK_TEST,

   TOK_IF,
   CTOK_TEST,
   
   TOK_1,
   CTOK_TEST,

   TOK_EQ,
   CTOK_TEST,

   TOK_K,
   CTOK_TEST,

   TOK_1,
   CTOK_TEST,

   TOK_2,
   CTOK_TEST,

   TOK_4,
   CTOK_TEST,

   TOK_DOT,
   CTOK_TEST,

   TOK_5,
   CTOK_TEST,

   TOK_6,
   CTOK_TEST,

   TOK_COLON,
   CTOK_TEST,

   TOK_1,
   CTOK_TEST,

   TOK_COLON,
   CTOK_TEST,

   TOK_2,
   CTOK_TEST,

   TOK_COLON,
   CTOK_TEST,

   TOK_3,
   CTOK_TEST,

   TOK_COLON,
   CTOK_TEST,

   TOK_END,
   CTOK_TEST,

   TOK_NO_TOK,
  };

TEST_RESULTS seq_p44_res[] =
  {
   {
    0, 0,   
    "P1            ",
   },
   {
    0, 0,   
    "001          1",
   },
   {
    0, 0,   
    "002       -01-",
   },
   {
    0, 0,   
    "003           ",
   },
   {
    0, 0,   
    "004          1",
   },
   {
    0, 0,   
    "005         12",
   },
   {
    0, 0,   
    "006        123",
   },
   {
    0, 0,   
    "007       1234",
   },
   {
    0, 0,   
    "008       -14-",
   },
   {
    0, 0,   
    "009       -15-",
   },
   {
    0, 0,   
    "010          1",
   },
   {
    0, 0,   
    "011       -01-",
   },
   {
    0, 0,   
    "012           ",
   },
   {
    0, 0,   
    "013          1",
   },
   {
    0, 0,   
    "014         12",
   },
   {
    0, 0,   
    "015        124",
   },
   {
    0, 0,   
    "016        124",
   },
   {
    0, 0,   
    "017       1245",
   },
   {
    0, 0,   
    "018      12456",
   },
   {
    0, 0,   
    "019       -14-",
   },
   {
    0, 0,   
    "020          1",
   },
   {
    0, 0,   
    "021       -14-",
   },
   {
    0, 0,   
    "022          2",
   },
   {
    0, 0,   
    "023       -14-",
   },
   {
    0, 0,   
    "024          3",
   },
   {
    0, 0,   
    "025       -14-",
   },
   {
    0, 0,   
    "026       -20-",
   },
   {
    0, 0,   
    "026       -20-",
   },

  };
#endif

// Example page 8

TOKEN seq45[] =
  {
   TOK_AC,   
   TOK_COMP_MODE,

   TOK_PLACES_2,
   TOK_CUT_F,
   
   TOK_MEM_CLR,
   TOK_8, TOK_5, TOK_2, 
   TOK_TIMES,
   TOK_1, TOK_4, TOK_7, TOK_MEM_PL,

   CTOK_TEST,
   
   TOK_7, TOK_8, TOK_9,
   TOK_TIMES, TOK_6, TOK_5, TOK_4, TOK_MEM_MN,
   
   TOK_MEM_REC,
   CTOK_TEST,
   
   TOK_NO_TOK,
  };

TEST_RESULTS seq45_res[] =
  {
   {
    0, 31779,   
    "        125244",
   },
   {
    0, 31779,   
    "        516006",
   },
   {
    0, 31779,   
    "       -390762",
   },
   {
    0, 31779,   
    "       -390762",
   },
  };

TEST_INFO test_seq[] =
  {
   {"Simple calculation", seq0,  seq0_res, 0, 0},
   {"Negative result",    seq1,  seq1_res, 0, 0},
   {"Longer calculation", seq2,  seq2_res, 0, 0},
   {"Square root",        seq3,  seq3_res, 0, 0},
   {"Powers",             seq4,  seq4_res, 0, 0},
   {"Constants",          seq5,  seq5_res, 0, 0},
   {"Longer constants",   seq6,  seq6_res, 0, 0},
   {"Percent",            seq7,  seq7_res, 0, 0},
   {"Percent markdown",   seq8,  seq8_res, 0, 0},
   {"Program 2",          seq9,  seq9_res, 0, 0},
   {"Percent markup",     seq10, seq10_res, 0, 0},
   {"Chain calculation",  seq11, seq11_res, 0, 0},
   {"Small Program",      seq12, seq12_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"IF",                 seq14, seq14_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"IF",                 seq16, seq16_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"IF",                 seq18, seq18_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"Goto",               seq20, seq20_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"Set up MJ",          seq22, seq22_res, 0, 0},
   {"Perform MJ",         seq23, seq23_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"Indirect store",     seq25, seq25_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"CALL",               seq27, seq27_res, 0, 0},
   {"      Exit",         seq_ans, seq_ans_res, 0, 0},  // Exit from program
   {"Rounding",           seq29, seq29_res, 0, 0},
   {"Example page 5",     seq30, seq30_res, 0, 0},
   {"Example page 5",     seq31, seq31_res, 0, 0},
   {"Example page 5",     seq32, seq32_res, 0, 0},
   {"Example page 5",     seq33, seq33_res, 0, 0},
   {"Example page 5",     seq34, seq34_res, 0, 0},
   {"Example page 5",     seq35, seq35_res, 0, 0},
   {"Example page 5",     seq36, seq36_res, 0, 0},
   {"Error Power",        seq37, seq37_res, 0, 0},
   {"Negative in cut 54", seq38, seq38_res, 0, 0},
   {"Extra decimals",     seq39, seq39_res, 0, 0},
   {"Example pag 9",      seq40, seq40_res, 0, 0},
   {"Example pag 9",      seq41, seq41_res, 0, 0},
   {"Example pag 8",      seq42, seq42_res, 0, 0},
   {"Example pag 8",      seq43, seq43_res, 0, 0},
#if DISPLAY_WRITE_PRO101
   {"Prog entry",         seq_p44, seq_p44_res, 0, 0},
#endif
   {"Prog entry",         seq45,  seq45_res, 0, 0},
  };

#define NUM_TESTS ((sizeof(test_seq))/(sizeof(TEST_INFO)))

////////////////////////////////////////////////////////////////////////////////

#define TEXT_PARAMETER_LEN 40

const uint CAPTURE_PIN_BASE = 0;
const uint CAPTURE_PIN_COUNT = 32;
const uint CAPTURE_N_SAMPLES = 100;

#define CLOCK_DIV  30.0f

const int P_VFI_D_H = 0;
const int P_VFI_D_L = 1;
const int P_VF_D_L  = 2;
const int P_VF_D_H  = 3;

const int P_MAX_LOAD  = 4;
const int P_MAX_BLANK = 5;
const int P_MAX_CLK   = 6;
const int P_MAX_DIN   = 7;

const int P_CATHODE1  = 8;
const int P_CATHODE2  = 9;

const int P_KI0  = 15;
const int P_KI1  = 16;
const int P_KI2  = 17;
const int P_KI3  = 18;
const int P_KI4  = 19;
const int P_KI5  = 20;
const int P_KI6  = 21;
const int P_KI7  = 22;

const int P_SW0  = 10;
const int P_SW1  = 11;
const int P_SW2  = 12;
const int P_SW3  = 13;
const int P_SW4  = 14;

#define READ_GPIO(NNN)          (((gpio_states = sio_hw->gpio_in) & (1 << NNN)) >> NNN)
#define CHECK_GPIO_STATE(NNN)   (((gpio_states                  ) & (1 << NNN)) >> NNN)
#define GET_GPIO_STATE(NNN)     (((gpio_states                  ) & (1 << NNN)) >> NNN)

#define ALL_HIGH 0xFFFFFFFFFFFFFFFF
#define GPIO_VALUE(XXX,NNN)     (((XXX) & (1 << NNN)) >> NNN)

#define LED_ENT   5
#define LED_ANS   4
#define LED_END   3
#define LED_MISS  0
#define LED_K     2

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Serial loop command structure

typedef void (*SERIAL_FPTR)(void);

typedef struct
{
  char key;
  char *desc;
  SERIAL_FPTR fn;
} SERIAL_COMMAND;

int keypress = 0;
int parameter = 0;
int auto_increment_parameter = 0;
int auto_increment_address   = 0;

// Used when building data in a buffer
char hex_buffer[2048];
char key_buffer[2048];
char keystroke_buffer[20];

#define PRINT_LF     0
#define PRINT_NO_LF  1

char text_parameter[TEXT_PARAMETER_LEN+1] = "";

// Shadow fields
volatile int shadow_sf_a = 0;
volatile int shadow_sf_b = 0;

volatile int shadow2_sf_a = 0;
volatile int shadow2_sf_b = 0;

int filament_drive = 1;
int led_drive = 0;

// Character set
#define CHAR_SET_LEN (sizeof(char_set)/sizeof(CHAR_MAP))

//
//     0
//   5   1
//     6
//   4   2
//     3  
//          7
//
//
//
//
CHAR_MAP char_set[] =
  {
   { 0, 0x3F},
   { 1, 0x06},
   { 2, 0x5B},
   { 3, 0x4F},
   { 4, 0x66},
   { 5, 0x6D},
   { 6, 0x7D},
   { 7, 0x07},
   { 8, 0x7F},
   { 9, 0x6F},
   {'0', 0x3F},
   {'1', 0x06},
   {'2', 0x5B},
   {'3', 0x4F},
   {'4', 0x66},
   {'5', 0x6D},
   {'6', 0x7D},
   {'7', 0x07},
   {'8', 0x7F},
   {'9', 0x6F},
   {' ', 0x00},
   {'-', 0x40},
   {'A', 0x77},
   {'B', 0x7C},
   {'C', 0x39},
   {'D', 0x5E},
   {'E', 0x79},
   {'F', 0x71},
   {'a', 0x77},
   {'b', 0x7C},
   {'c', 0x39},
   {'d', 0x5E},
   {'e', 0x79},
   {'f', 0x71},

   {'P', 0x73},
   {'R', 0x50},
   {'O', 0x3F},
   {'o', 0x5C},
   
  };
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//

int cathode_1_led_data = 0;
int cathode_2_led_data = 0;

#define NUM_BUS_T  1

int queue_overflow = 0;

volatile int data_in = 0;
volatile int data_out = 0;

volatile int       bus_addr[NUM_BUS_T];
volatile int       bus_clk_count[NUM_BUS_T];

int last_ce        = 0;
int last_ce2       = 0;
int last_clk       = 0;
uint64_t ce_cycle  = 0;
uint64_t ce2_cycle = 0;

int ce          = 0;
int ce2         = 0;
int trans_i     = 0;
int dout        = 0;
int din         = 0;
int rw          = 0;
int address     = 0;
int clk         = 0;
uint64_t doutlo = 0;
uint64_t douthi = 0;

// Tracing of reads and writes is done in this buffer
volatile int stopped = 0;

#define TRACE_BIT  7
#define NUM_TRACE_ENTRIES  100
volatile uint8_t exec_trace_address[NUM_TRACE_ENTRIES];
volatile uint8_t exec_trace_data[NUM_TRACE_ENTRIES];
volatile uint8_t exec_trace_flags[NUM_TRACE_ENTRIES];
volatile int     exec_trace_i = 0;

#define EXEC_TRACE_START_128    1
#define EXEC_TRACE_FLAG_R       1
#define EXEC_TRACE_FLAG_W       2

#define NUM_DATA_BITS 256
int bit_din[NUM_DATA_BITS];
int bit_dout[NUM_DATA_BITS];
int bit_rw[NUM_DATA_BITS];

int clk_count   = 0;

volatile int loop_count  = 0;

// Keyboard drive
int kb_drv = 1;
int kb_tick = 0;
int kb_drv_num = 0;
volatile int kbd_sense_data[NUM_KBD_DRV];
volatile int sw_sense_data[NUM_SWITCH_DRIVE];

////////////////////////////////////////////////////////////////////////////////
//
// RAM emulation data
//
// Two HD36106s are emulated, one stores the program, the other stores memories.
//

#define DO_MP_MEMORIES    1
#define DO_MP_PROGRAM     2
#define DO_MP_BOTH        3

// Data is stored for all emulated RAMs as bytes
uint8_t ram_data[RAM_SIZE];

// Where are commands looked for?
#define COMMAND_RAM_ADDR ADDRESS_OF_MEM(8)

// We have a pointer to the commands so it can be changed, if required
uint8_t *command_ptr = COMMAND_RAM_ADDR;

#define NUM_DIGITS 14
int digit_drive = 1;
int digit_num = 0;

extern char display[1000];
char new_display[1000];

icd n;
int dotp;

char display[1000];
int  display_decimal_pos = -1;

////////////////////////////////////////////////////////////////////////////////
//
// Flash data
//
// We have several flash program slots.
// Slots are in the top 1Mbyte of flash
//
// The flash has to be erased in 4096 byte aligned blocks
// Writes can be 256 byte aligned but due to erase being 4096 we use that slot size.

// Program and dat aram is stored at the same time, so it is a snapshot of the entire
// RAM. RAM plkus program is 256 bytes.

#define FLASH_PROGRAM_DATA_SIZE         256
#define FLASH_PROGRAM_SLOT_SIZE         4096
#define FLASH_PROGRAM_SLOT_AREA_SIZE    (800*1024)
#define FLASH_PROGRAM_NUM_SLOTS         (FLASH_PROGRAM_SLOT_AREA_SIZE / FLASH_PROGRAM_SLOT_SIZE)

#define FLASH_SLOT_OFFSET (1024*1024)
uint8_t *flash_slot_contents   = (uint8_t *) (XIP_BASE + FLASH_SLOT_OFFSET);

// general buffer for flash read and write
uint8_t slot_buffer[FLASH_PROGRAM_DATA_SIZE];

////////////////////////////////////////////////////////////////////////////////
//
// Slot layout:
//
// RAM_SIZE bytes      :  Program
// TEXT_PARAMETER_LEN  :  Slot label
//
////////////////////////////////////////////////////////////////////////////////
//
// Prototypes
//
////////////////////////////////////////////////////////////////////////////////

void print_keystroke_to_buffer(int byte, int no_lf);
void serial_help(void);
double mem_to_dbl(uint8_t *m);
char *mem_to_str(uint8_t *m);
void dbl_to_mem(double value, uint8_t *m);
void print_keystroke_to_buffer(int byte, int no_lf);
void  print_gpio_functions(void);
void set_serial_field(int field, int value);
void display_keystrokes_at(TOKEN *dat);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CALCULATOR_STATE state;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void set_gpio_input(int gpio_pin)
{
  gpio_init(gpio_pin);
  gpio_set_dir(gpio_pin, GPIO_IN);
  gpio_set_pulls(gpio_pin, 0, 0);
}

void set_gpio_output(int gpio_pin)
{
  gpio_init(gpio_pin);
  gpio_set_dir(gpio_pin, GPIO_OUT);
  //gpio_put(gpio_pin, 0);
}

////////////////////////////////////////////////////////////////////////////////
//

// Drives FSMs

void drive_fsms(void)
{
  while( !token_queue_empty() )
    {
      int tok = unqueue_token();
      
      do_fsm(&top_fsm, &state, tok);
    }
}

////////////////////////////////////////////////////////////////////////////////

// Read keyboard sense lines

int kbd_lines[] =
  {
   P_KI0,
   P_KI1,
   P_KI2,
   P_KI3,
   P_KI4,
   P_KI5,
   P_KI6,
   P_KI7,
  };


int kbd_sense(void)
{
  int v;
  
  for(int i=0; i<NUM_KBD_LINES; i++)
    {
      v<<=1;
      
      if( gpio_get(kbd_lines[i]) )
	{
	  v |= 1;
	}
    }
  return(v);  
}

// Determine keyboard key code
int keycode(void)
{
  int code = 0;
  
  for(int i=0; i<NUM_KBD_DRV; i++)
    {
      if( kbd_sense_data[i] != 0 )
	{
	  code = (i << NUM_KBD_LINES) + kbd_sense_data[i];
#if DUMP_KEYCODE
	  printf("\nKeycode:%04X", code);
#endif
	  return(code);
	}
    }
  
  return(0);
}

typedef struct _CODE_TOKEN_MAP_ENTRY
{
  int token;
  int code;
} CODE_TOKEN_MAP_ENTRY;

#define NUM_CODE_TOKEN_MAP (sizeof(code_token_map)/sizeof(struct _CODE_TOKEN_MAP_ENTRY))

CODE_TOKEN_MAP_ENTRY code_token_map[] =
  {
   { TOK_MAC,          0x008},
   { TOK_1X,           0xFFF},
   { TOK_GOTO,         0x410},
   { TOK_SUB,          0xFFF},
   { TOK_ST,           0x420},
   { TOK_COLON,        0x408},
   { TOK_MJ,           0x040},
   { TOK_SQRT,         0x208},
   { TOK_LOG,          0xFFF},
   { TOK_LN,           0xFFF},
   { TOK_EX,           0xFFF},
   { TOK_XY,           0x201},
   { TOK_10X,          0xFFF},
   { TOK_PLMN,         0x202},
   { TOK_K,            0x020},
   { TOK_IF,           0x440},
   { TOK_ARC,          0xFFF},
   { TOK_SIN,          0xFFF},
   { TOK_COS,          0xFFF},
   { TOK_TAN,          0xFFF},
   { TOK_EQ,           0x110},
   { TOK_IM,           0x080},
   { TOK_MR,           0x440},
   { TOK_ENT,          0x401},
   { TOK_ANS,          0x402},
   { TOK_MPL,          0xFFF},
   { TOK_TIMES,        0x180},
   { TOK_DIV,          0x101},
   { TOK_PL,           0x140},
   { TOK_MN,           0x120},
   { TOK_DOT,          0x340},
   { TOK_EXP,          0x0FF},
   { TOK_0,            0x304},
   { TOK_1,            0x308},
   { TOK_2,            0x302},
   { TOK_3,            0x301},
   { TOK_4,            0x380},
   { TOK_5,            0x310},
   { TOK_6,            0x320},
   { TOK_7,            0x104},
   { TOK_8,            0x108},
   { TOK_9,            0x102},
   { TOK_AC,           0x220},
   { TOK_C,            0x210},
   { TOK_PERCENT,      0x280},
   { TOK_START,        0xFFF},
   { TOK_COMP_MODE,    0x200},
   { TOK_WRITE_MODE,   0x201},
   { TOK_RUN_MODE,     0x202},
   { TOK_PRN,          0x002},
   { TOK_I,            0x010},
   { TOK_END,          0x001},
   { TOK_MC,           0x480},
   { TOK_EQ_2,         0x204},
   { TOK_RETURN,       0x004},
  };
  

int keycode_to_token(int code)
{
  
  for(int i=0; i<NUM_CODE_TOKEN_MAP; i++)
    {
      if( code_token_map[i].code == code)
	{
#if DUMP_KEYCODE
	  printf("\nLooking for keycode 0x%03X", code);
	  printf("\nToken found: %03X", code_token_map[i].token);
#endif
	  
	  return(code_token_map[i].token);
	}
    }

  return(0);
}

////////////////////////////////////////////////////////////////////////////////

// Read switch sense lines

int sw_lines[] =
  {
   P_SW0,
   P_SW1,
   P_SW2,
   P_SW3,
   P_SW4,
  };

#define NUM_SW_LINES (sizeof(sw_lines)/sizeof(int))

int sw_sense(void)
{
  int v = 0;
  
  for(int i=0; i<NUM_SW_LINES; i++)
    {
      v<<=1;
      
      if( gpio_get(sw_lines[i]) )
	{
	  v |= 1;
	}
    }
  return(v);  
}

////////////////////////////////////////////////////////////////////////////////

void display_keystrokes(void)
{
  display_keystrokes_at(&(ram_data[0]));
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////

void display_ram_at(uint8_t *dat)
{
  printf("\n");
  
  for(int z = 0; z<RAM_SIZE; z++)
    {
      int byte = 0;
      
      if( (z % 8) == 0)
	{
#if 0	  
	  if( (z != 0) && (z <= 128))
	    {
	      switch(z/8)
		{
		case 11:
		case 12:
		  printf("    Jump Indices");
		  break;
		  
		default:
		  if( ((*(dat-1)) & 0xF0) == 0xF0,1 )
		    {
		      printf("    M%02d %s %g", MEM_OF_ADDRESS(z), mem_to_str(dat-8), mem_to_dbl(dat-8));

#if 0		      
		      if( MEM_OF_ADDRESS(z)==4 )
			{
			  dbl_to_mem(mem_to_dbl(dat-8), dat-16);
			}
#endif
		    }
		  break;
		}
	    }
#endif
	  printf("\n%03X: ", z);
	}
      
      printf("%02X ", *(dat++));
    }
  
  printf("\n");

}

void display_int_ram_at(TOKEN *dat)
{
  printf("\n");
  
  for(int z = 0; z<RAM_SIZE; z++)
    {
      int byte = 0;
      
      if( (z % 8) == 0)
	{
	  printf("\n%03X: ", z);
	}
      
      printf("%02X ", *(dat++));
    }
  
  printf("\n");

}

////////////////////////////////////////////////////////////////////////////////

void display_keystrokes_at(TOKEN *dat)
{
  int done = 0;
  
  printf("\n");
  
  for(int z = 0; (z<RAM_SIZE) && !done; z++)
    {
      int b = *(dat++);

      switch(b)
	{
	case TOK_NONE:
	  break;
	  
	case TOK_END:
	  printf("\n%s ", label_of_token(b));
	  done = 1;
	  break;
	  
	case TOK_COLON:
	case TOK_MJ:
	case TOK_MAC:
	  printf("%s \n", label_of_token(b));
	  break;

	default:
	  printf("%s ", label_of_token(b));
	  break;
	}
    }
  
  printf("\n");

}


////////////////////////////////////////////////////////////////////////////////

void cli_display_prog_ram(void)
{
  printf("\nPR#%d\n", parameter);
  
  display_int_ram_at(&(state.program_step[parameter][0]));
  display_keystrokes_at(&(state.program_step[parameter][0]));
  
  // Display keystrokes after RAM hex data
  //display_keystrokes();

}

////////////////////////////////////////////////////////////////////////////////
//
// Flash write/erase
//
////////////////////////////////////////////////////////////////////////////////

// Erase a slot
void erase_slot(int n)
{
  flash_range_erase(FLASH_SLOT_OFFSET+n*FLASH_PROGRAM_SLOT_SIZE, FLASH_PROGRAM_SLOT_SIZE);
}

// Checksum a slot
int checksum_slot(int slot_num)
{
  int csum = 0;

  for(int i=0; i<FLASH_PROGRAM_DATA_SIZE; i++)
    {
      csum += *(flash_slot_contents+slot_num*FLASH_PROGRAM_SLOT_SIZE+i);
    }

  return(csum);
}

void do_auto_increment_parameter(void)
{
  if( auto_increment_parameter )
    {
      parameter++;
    }
}

void do_auto_increment_address(void)
{
  if( auto_increment_address )
    {
      address++;
    }
}

////////////////////////////////////////////////////////////////////////////////

void print_keystroke_to_buffer(int byte, int no_lf)
{
  char * dest = keystroke_buffer;
  *dest = '\0';
  
  switch(byte)
    {
    case 0x00:
      break;
      
    case 0x80:
    default:
      sprintf(dest, "?(%02x)", byte);
      break;
      
    case 0xC4:
      if( no_lf )
	{
	  sprintf(dest, "  ST# ");
	}
      else
	{
	  sprintf(dest, "\n  ST# ");
	}
      break;
      
    case 0xC1:
      sprintf(dest, "MAC ");
      break;
      
    case 0xC2:
      sprintf(dest, "GOTO ");
      break;
      
    case 0xC3:
      sprintf(dest, "SUB# ");
      break;
      
    case 0xB1:
      sprintf(dest, "SQRT ");
      break;
    case 0xB2:
      sprintf(dest, "log ");
      break;
    case 0xB3:
      sprintf(dest, "ln ");
      break;
    case 0xB4:
      sprintf(dest, "e^x ");
      break;
    case 0xB5:
      sprintf(dest, "x^y ");
      break;
    case 0xB6:
      sprintf(dest, "10^x ");
      break;
    case 0xA2:
      sprintf(dest, "K ");
      break;
    case 0xA1:
      sprintf(dest, "+/- ");
      break;
    case 0xA3:
      sprintf(dest, "IF	 ");
      break;
    case 0xA4:
      sprintf(dest, "arc ");
      break;
    case 0xA5:
      sprintf(dest, "sin ");
      break;
    case 0xA6:
      sprintf(dest, "cos ");
      break;
    case 0xA7:
      sprintf(dest, "tan ");
      break;
    case 0xC6:
      sprintf(dest, "MJ ");
      break;
    case 0xC5:
      sprintf(dest, " : ");
      break;
    case 0xD1:
      sprintf(dest, "IM ");
      break;
    case 0xD0:
      sprintf(dest, "= ");
      break;
    case 0xD2:
      sprintf(dest, "ENT ");
      break;
    case 0xD3:
      sprintf(dest, "ANS ");
      break;
    case 0xD4:
      sprintf(dest, "M+ ");
      break;
    case 0xD6:
      sprintf(dest, "* ");
      break;
    case 0xD5:
      sprintf(dest, "/ ");
      break;
    case 0xD7:
      sprintf(dest, "+ ");
      break;
    case 0xD8:
      sprintf(dest, "- ");
      break;
    case 0xDD:
      sprintf(dest, "EXP ");
      break;
    case 0xD9:
      sprintf(dest, ". ");
      break;
    case 0xF0:
      sprintf(dest, "0 ");
      break;
    case 0xF1:
      sprintf(dest, "1");
      break;
    case 0xF2:
      sprintf(dest, "2");
      break;
    case 0xF3:
      sprintf(dest, "3");
      break;
    case 0xF4:
      sprintf(dest, "4");
      break;
    case 0xF5:
      sprintf(dest, "5");
      break;
    case 0xF6:
      sprintf(dest, "6");
      break;
    case 0xF7:
      sprintf(dest, "7");
      break;
    case 0xF8:
      sprintf(dest, "8");
      break;
    case 0xF9:
      sprintf(dest, "9");
      break;

    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Convert memory bytes to a double value
//
////////////////////////////////////////////////////////////////////////////////

double bcd_to_dec(uint8_t *b)
{
  double result = ((*b) & 0xF);

  result += (((*b) & 0xF0) >> 4) * 10.0;
  return(result);
}

int bcd_to_int(uint8_t *b)
{
  int result = ((*b) & 0xF);
  result += (((*b) & 0xF0) >> 4) * 10.0;
  return(result);
}

void dbl_to_bcd(double v, uint8_t *m)
{
  int byte;
  double intpart;
  double fracpart = modf(v/10.0, &intpart);

  byte = intpart * 16.0;
  byte += (fracpart * 10.0);

  *m = byte;
}

//------------------------------------------------------------------------------


void dbl_to_mem(double value, uint8_t *m)
{
  int sign = 0;
  int exp_sign = 0;

  // Build up sign byte
  if( value < 0.0 )
    {
      sign = 4;
    }
  else
    {
      sign = 0;
    }

  //  printf("\nSign:%d", sign);
  
  value = fabs(value);
  
  double exponent = floor(log10(value));

  //printf("\nExponent:%g", exponent);

  // Normalise  
  value /= pow(10, exponent);
  
  if( exponent < 0.0 )
    {
      exponent = 100+exponent;
    }
  else
    {
      sign |= 0x1;
    }
  
  // get digits
  double dpair;

  dpair = floor(value);
  dbl_to_bcd(dpair, m+5);
  value = value - floor(value);
  value *=100.0;

  dpair = floor(value);
  dbl_to_bcd(dpair, m+4);
  value = value - floor(value);
  value *=100.0;

  dpair = floor(value);
  dbl_to_bcd(dpair, m+3);
  value = value - floor(value);
  value *=100.0;

  dpair = floor(value);
  dbl_to_bcd(dpair, m+2);
  value = value - floor(value);
  value *=100.0;

  dpair = floor(value);
  dbl_to_bcd(dpair, m+1);
  value = value - floor(value);
  value *=100.0;

  dpair = floor(value);
  dbl_to_bcd(dpair, m+0);
  value = value - floor(value);
  value *=100.0;

  // Write exponent and signs
  dbl_to_bcd(exponent, m+6);

  int signs = 0xF0;

  *(m+7) = sign; 
  
}

////////////////////////////////////////////////////////////////////////////////
//
// Converts a memory to a string representation
// Fixed format with exponent
//
////////////////////////////////////////////////////////////////////////////////

#define MEM_STR_LEN  40

char memstr[MEM_STR_LEN];

char *mem_to_str(uint8_t *m)
{
  char man_sgn_str[2] = " ";
  char exp_sgn_str[2] = " ";
  
  int exp = bcd_to_int((m+6));
  
  int exp_sign =  (*(m+7)) & 1;
  int man_sign = ((*(m+7)) & 4) >> 2;

  exp_sgn_str[0] = (exp_sign==0)? '-':' ';
  if( exp_sign == 0 )
    {
      exp = 100-exp;
    }
  
  man_sgn_str[0] = (man_sign==0)? ' ':'-';
  
  int r1 = bcd_to_int( (m+5));
  int r2 = bcd_to_int( (m+4));
  int r3 = bcd_to_int( (m+3));
  int r4 = bcd_to_int( (m+2));
  int r5 = bcd_to_int( (m+1));
  int r6 = bcd_to_int( (m+0));

  sprintf(memstr, "%s%02d.%02d%02d%02d%02d%02d %sE%03d",
	  man_sgn_str,
	  r1,
	  r2,
	  r3,
	  r4,
	  r5,
	  r6,
	  exp_sgn_str,
	  exp
	  );
  return(memstr);
}


double mem_to_dbl(uint8_t *m)
{
  double result = 0.0;
  double exp = bcd_to_dec((m+6));
  int exp_sign =  (*(m+7)) & 1;
  int man_sign = ((*(m+7)) & 4) >> 2;
  double exp_sgn = (exp_sign==0)? -1.0 : 1.0;
  double man_sgn = (man_sign==0)?  1.0 :-1.0;
  
  result  = bcd_to_dec( (m+5)) * 1.0;
  result += bcd_to_dec( (m+4)) * 0.01;
  result += bcd_to_dec( (m+3)) * 0.0001;
  result += bcd_to_dec( (m+2)) * 0.000001;
  result += bcd_to_dec( (m+1)) * 0.00000001;
  result += bcd_to_dec( (m+0)) * 0.0000000001;

  result *= man_sgn;
  if( exp_sign == 0 )
    {
      result *= pow(10, (100.0-exp)*-1);
    }
  else
    {
      result *= pow(10, exp);
    }
  
  return(result);
}

void print_keystroke(int byte, int no_lf)
{
  print_keystroke_to_buffer(byte, no_lf);
  printf("%s", keystroke_buffer);
}

void cli_boot_mass(void)
{
  reset_usb_boot(0,0);
}

  char marker4 = '4';
  icd z = 1.0;
  char marker1 = '1';
  icd a("5");
  char marker2 = '2';
  icd b("2");
  char marker3 = '3';

// Test the BCD library
void cli_bcd_test(void)
{
  // Do some simple calculations

  a.dump();
  
  printf("\na = %s", a.AsDisplayString().c_str());
  printf("\nb = %s", b.AsDisplayString().c_str());
  printf("\nz = %s", z.AsDisplayString().c_str());

#if 0
  a = 1.0;
  b = 0.0;

  a = a / b;
  return;
#endif
  
  b = b.Power(a);

  icd sin45("45");
  sin45 = sin45 / icd("180") * icd("PI");
  sin45 = sin45.Sine();
  
  printf("\nsin45 = %s", sin45.AsDisplayString().c_str());
  sin45 = sin45 * sin45;
  
  printf("\nsin45 = %s", sin45.AsDisplayString().c_str());

  printf("\nStart");

#if 0
  for(int i=0; i< 100; i++)
    {
      sin45 = sin45 * sin45;
      printf("\nsin45 = %s", sin45.AsDisplayString().c_str());
    }
#endif

#if 1
  a = 2.0;
  
  for(int i=0; i< 100; i++)
    {
      a = a.SquareRoot();
      printf("\na = %s", a.AsDisplayString().c_str());
    }
#endif

  printf("\na = %s", a.AsDisplayString().c_str());
  printf("\nb = %s", b.AsDisplayString().c_str());
  printf("\nz = %s", z.AsDisplayString().c_str());

  printf("\nMarker1 = '%c'", marker1);
  printf("\nMarker2 = '%c'", marker2);
  printf("\nMarker3 = '%c'", marker3);
  
  printf("\nEnd");

}

// Another digit pressed, update the parameter variable
void cli_digit(void)
{
  parameter *= 10;
  parameter += keypress-'0';
}

void cli_zero_parameter(void)
{
  parameter = 0;
}

void cli_set_address(void)
{
  address = parameter;
}

void cli_information(void)
{
  printf("\n");

  //  printf("\nNumber of reads  :%d", num_rd);
  //printf("\nNumber of writes :%d", num_wr);
  printf("\nNumber of loops: %d", loop_count);
  printf("\n");
}

void cli_start_tracing(void)
{
  stopped = 0;
  exec_trace_i = 0;

  for(int i=0; i<NUM_TRACE_ENTRIES; i++)
    {
      exec_trace_address[i] = 0;
      exec_trace_data[i]    = 0;
      exec_trace_flags[i]   = 0;
    }
}

#define NUM_SLICES  5
#define TRACE_SLICE_SIZE (NUM_TRACE_ENTRIES/NUM_SLICES)
void cli_display_trace(void)
{
  printf("\n");
  
  for(int i=0; i<TRACE_SLICE_SIZE; i++)
    {
      for(int j = 0; j<NUM_SLICES; j++)
	{
	  char flags = '.';
	  switch(exec_trace_flags[i+j*TRACE_SLICE_SIZE] )
	    {
	    case EXEC_TRACE_FLAG_R:
	      flags = 'R';
	      break;
	      
	    case EXEC_TRACE_FLAG_W:
	      flags = 'W';
	      break;
	    }
	  
	  printf("  %05d: %c %02X %02X  %c",
		 i+j*TRACE_SLICE_SIZE,
		 (i+j*TRACE_SLICE_SIZE)==exec_trace_i?'*':' ',
		 exec_trace_address[i+j*TRACE_SLICE_SIZE],
		 exec_trace_data[i+j*TRACE_SLICE_SIZE],
		 flags);
	}
      printf("\n");
    }

  printf("\n");
}

void cli_write_byte(void)
{
  ram_data[address] = parameter;

  do_auto_increment_address();
  do_auto_increment_parameter();
  
  printf("\nWritten %02X to %03X\n", parameter, address);
}

void cli_erase_program_slot(void)
{
  printf("\nErasing program slot %d...", parameter);
  erase_slot(address);
  printf("\ndone.\n");

  do_auto_increment_parameter();
}

void cli_display_program_slot(void)
{
  
  printf("\nSlot %d\n", address);

  // First dump in hex
  display_ram_at(flash_slot_contents+address*FLASH_PROGRAM_SLOT_SIZE);

  printf("\n\n");

  // Then decode keystrokes
  display_keystrokes_at(flash_slot_contents+address*FLASH_PROGRAM_SLOT_SIZE);
  //printf("\n");
}

void save_ram(int slotnum, int what_mp)
{
  // Save slot data in a buffer so we can write just memories or program and keep what was in the
  // other half
  memcpy(slot_buffer, flash_slot_contents+slotnum*FLASH_PROGRAM_SLOT_SIZE, RAM_SIZE);
  
  // Erase slot
  erase_slot(slotnum);
  
  // Write to slot buffer

  if ( what_mp & DO_MP_MEMORIES )
    {
      printf("\nWriting memory data to slot %d", slotnum);
      memcpy(&(slot_buffer[RAM_OFF_M]), &(ram_data[RAM_OFF_M]), RAM_SIZE_M);
    }

  if ( what_mp & DO_MP_PROGRAM )
    {
      printf("\nWriting program data to slot %d", slotnum);
      memcpy(&(slot_buffer[RAM_OFF_P]), &(state.program_step[parameter][0]), RAM_SIZE_P);
    }

  // Write the buffer back
  flash_range_program(FLASH_SLOT_OFFSET + (FLASH_PROGRAM_SLOT_SIZE * slotnum), (uint8_t *) &(slot_buffer[0]), RAM_SIZE);

  printf("\nData written\n");
  
}

int choose_what_mp(void)
{
  int what = 0;
  int key = 0;
  
  printf("\nMemories or Program (M/P)?");

  if( ((key = getchar_timeout_us(10000000)) != PICO_ERROR_TIMEOUT))
    {
      switch(key)
	{
	case 'm':
	case 'M':
	  what = DO_MP_MEMORIES;
	  break;

	case 'p':
	case 'P':
	  what = DO_MP_PROGRAM;
	  break;
	  
	default:
	  break;
	}
      
    }

return(what);
}


void cli_save_ram_mp(void)
{
  int do_what_mp = choose_what_mp();
  
  save_ram(address, do_what_mp);
}

void cli_save_ram(void)
{
  save_ram(address, DO_MP_BOTH);
}

// Parameter is slotnum
// Address is program

void load_ram(int slotnum, int do_which_mp)
{
  if( do_which_mp & DO_MP_MEMORIES )
    {
      printf("\nLoading memories from flash slot %03d", slotnum);
      memcpy(ram_data+RAM_OFF_M, flash_slot_contents+slotnum*FLASH_PROGRAM_SLOT_SIZE+RAM_OFF_M, RAM_SIZE_M);
    }

  if( do_which_mp & DO_MP_PROGRAM )
    {
      printf("\nLoading program from flash slot %03d", slotnum);
      memcpy(&(state.program_step[parameter][0]), flash_slot_contents+slotnum*FLASH_PROGRAM_SLOT_SIZE+RAM_OFF_P, RAM_SIZE_P);
    }
  
  printf("\n");
}

void cli_load_ram_mp(void)
{
  int do_what_mp = choose_what_mp();
  
  load_ram(parameter, do_what_mp);
}

void cli_load_ram(void)
{
  load_ram(address, DO_MP_BOTH);
}

void copy_slot_to_buffer(int slot, uint8_t *buffer)
{
  memcpy(buffer, flash_slot_contents+slot*FLASH_PROGRAM_SLOT_SIZE, FLASH_PROGRAM_DATA_SIZE);
}

////////////////////////////////////////////////////////////////////////////////
//
// Write the text parameter as a slot label to the slot
// with the number of the parameter
// We have to copy the slot program, erase the slot then write the program
// back and then write the label.
//
////////////////////////////////////////////////////////////////////////////////

uint8_t label_buffer[FLASH_PROGRAM_SLOT_SIZE];

void cli_label_slot(void)
{
  copy_slot_to_buffer(address, &(label_buffer[0]));
  erase_slot(address);

  // Add label data to buffer before we write it back
  for(int i=0; i<TEXT_PARAMETER_LEN;i++)
    {
      label_buffer[FLASH_PROGRAM_DATA_SIZE+i] = text_parameter[i];
    }

  // Write the buffer back
  flash_range_program(FLASH_SLOT_OFFSET + (FLASH_PROGRAM_SLOT_SIZE * address), (uint8_t *) &(label_buffer[0]), FLASH_PROGRAM_SLOT_SIZE);
  
}

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////

void cli_slot_program_list(void)
{
  for(int i=0; i<FLASH_PROGRAM_NUM_SLOTS; i++)
    {
      printf("\n%03d:", i);
      
      if( *((flash_slot_contents+i*FLASH_PROGRAM_SLOT_SIZE)) == 0xFF )
	{
	  printf(" Blank");
	  continue;
	}

      for(int j=0; j<TEXT_PARAMETER_LEN;j++)
	{
	  uint8_t c=*((flash_slot_contents+i*FLASH_PROGRAM_SLOT_SIZE)+FLASH_PROGRAM_DATA_SIZE+j);
	  if( isprint(c) )
	    {
	      printf("%c", c);
	    }
	}

      // Now display the keystrokes
      display_keystrokes_at(flash_slot_contents+i*FLASH_PROGRAM_SLOT_SIZE);
    
      printf("\n");
    }

  printf("\n");
}

void cli_toggle_auto_increment_parameter(void)
{
  auto_increment_parameter = !auto_increment_parameter;
}

void cli_toggle_auto_increment_address(void)
{
  auto_increment_address = !auto_increment_address;
}

// Enter a text string that can be used by other commands

void cli_text_param(void)
{
  int key;
  int done = 0;
  int i = 0;

  printf("\nType string and end with RETURN...\n");
  
  while( !done )
    {
      if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
	{
	  printf("%c", key);
	  
	  if( key == 13 )
	    {
	      done = 1;
	      continue;;
	    }

	  text_parameter[i++] = key;
	  
	  if( i >= (TEXT_PARAMETER_LEN-1) )
	    {
	      i--;
	    }
	}
    }
  
  text_parameter[i] = '\0';
}

void cli_slot_label_list(void)
{
  for(int i=0; i<FLASH_PROGRAM_NUM_SLOTS; i++)
    {
      if( *((flash_slot_contents+i*FLASH_PROGRAM_SLOT_SIZE)+FLASH_PROGRAM_DATA_SIZE+0) == 0xFF)
	{
	  // Probably erased slot, don't display
	  continue;
	}
      
      printf("\n%03d: ", i);
      for(int j=0; j<TEXT_PARAMETER_LEN;j++)
	{
	  uint8_t c=*((flash_slot_contents+i*FLASH_PROGRAM_SLOT_SIZE)+FLASH_PROGRAM_DATA_SIZE+j);
	  if( isprint(c) )
	    {
	      printf("%c", c);
	    }
	}
    }
  printf("\n");
}

void clear_ram_mp(int what)
{
  if( what & DO_MP_MEMORIES )
    {
      printf("\nClearing memories");
      memset(&(ram_data[RAM_OFF_M]), 0x00, RAM_SIZE_M);
    }

  if( what & DO_MP_PROGRAM )
    {
      printf("\nClearing program");
      memset(&(ram_data[RAM_OFF_P]), 0x00, RAM_SIZE_P);
    }
  
  printf("\n");
}

void cli_clear_ram_mp(void)
{
  int what = choose_what_mp();

  clear_ram_mp(what);
}

void cli_clear_ram(void)
{
  clear_ram_mp(DO_MP_BOTH);
}

void cli_kbd_data(void)
{
  printf("\nKeyboard");

  for(int i=0; i<NUM_SWITCH_DRIVE; i++)
    {
      printf("\nSW %d: %04X", i, sw_sense_data[i]);
    }

  printf("\n");
}

void cli_clear_memories(void)
{
  clear_ram_mp(DO_MP_MEMORIES);
}

// Turn on an LED
void cli_led_on(void)
{
  int mask = (1 << parameter);

  cathode_1_led_data = mask & 0x07;
  cathode_2_led_data = (mask & 0x38) >> 3;

#if DEBUG_LEDS  
  printf("\nCath1: %02X  cath2:%02X", cathode_1_led_data, cathode_2_led_data);
#endif
}

// Turn an LED on or off, leaving others unchanged
void set_led_state(int led, int state)
{
  int mask = 1 << led;

  cathode_1_led_data &= ~((mask & 0x07) >> 0);
  cathode_2_led_data &= ~((mask & 0x38) >> 3);
  
  if( state )
    {
      cathode_1_led_data |=  ((mask & 0x07) >> 0);
      cathode_2_led_data |=  ((mask & 0x38) >> 3);
    }

#if DEBUG_LEDS  
  printf("\nLED %d turned %s", led, state?"on":"off");
  printf("\nCath1: %02X  cath2:%02X", cathode_1_led_data, cathode_2_led_data);
#endif
}

// Get the state of an LED

int get_led_state(int led)
{
  int mask = 1 << led;
  
  int state1, state2;
  
  state1 = cathode_1_led_data &= (mask & 0x07) >> 0;
  state2 = cathode_2_led_data &= (mask & 0x38) >> 3;

  if( (state1 + state2) == 0 )
    {
      return(0);
    }

  return(1);
}

void cli_display_dump_x(void)
{
  state.x.dump();
}

void cli_display_dump(void)
{
  printf("\nInformation");
  printf("\n===========");

  printf("\nShadow A:%08X", shadow_sf_a);
  printf("\nShadow B:%08X", shadow_sf_b);

  printf("\nLength of display variable:%d", strlen(display));
  printf("\nDisplay:'%s'   dotp:%d display_decimal_pos:%d\n", display, dotp, display_decimal_pos);
  printf("\nNew display:'%s'", new_display);
  printf("\nStore Index:%d", state.store_index);
  
  print_fsm_info(&top_fsm);
  print_fsm_info(&mode_comp_fsm);
  print_fsm_info(&mode_write_fsm);
  print_fsm_info(&mode_run_fsm);

  print_state(&state);

}

void cli_execute(void)
{
  execution_start();
}

void cli_program_mode(void)
{
  queue_token(TOK_WRITE_MODE);
}

void cli_comp_mode(void)
{
  queue_token(TOK_COMP_MODE);
}

void cli_run_mode(void)
{
  queue_token(TOK_RUN_MODE);
}

void cli_run_test_sequence(void)
{
  printf("\nRunning test sequence %d", parameter);
  test_sequence_all = 0;
  test_sequence     = parameter;
  test_result_i     = 0;

  test_seq[test_sequence].passed = 1;
  test_seq[test_sequence].approx_passed = 1;

}

void cli_run_all_test_sequence(void)
{
  test_sequence_all = 1;
  test_sequence     = 0;
  test_result_i     = 0;
  test_seq[test_sequence].passed = 1;
  test_seq[test_sequence].approx_passed = 1;
}

void cli_display_test_sequence_results(void)
{
  for(int i=0; i<NUM_TESTS; i++)
    {
      printf("\nSeq %04d %-30s %s (%s)  Actual:'%s'", i, test_seq[i]. desc, test_seq[i].passed?"Passed":"Failed", test_seq[i].approx_passed?"Approx Passed":"Approx failed", test_seq[i].actual_results_display);
    }
}

////////////////////////////////////////////////////////////////////////////////

//
// Accept keys from the keyboard and display the output on the USB. This is
// a simple emulator running on the Pico. All the Core 1 stuff is still running, we just
// drive the keys from the keyboard and ensure the display is shown on the USB

// This lets you use the code without a PRO-101, you just need a Pico

// Key map
TOKEN_INFO emulator_key_map[] =
  {
   {TOK_0,          "0"},
   {TOK_1,          "1"},
   {TOK_2,          "2"},
   {TOK_3,          "3"},
   {TOK_4,          "4"},
   {TOK_5,          "5"},
   {TOK_6,          "6"},
   {TOK_7,          "7"},
   {TOK_8,          "8"},
   {TOK_9,          "9"},
   {TOK_DOT,        "."},
   {TOK_TIMES,      "*"},
   {TOK_DIV,        "/"},
   {TOK_PL,         "+"},
   {TOK_MN,         "-"},
   {TOK_PRN,        "P"},
   {TOK_COLON,      ":"},
   {TOK_PERCENT,    "%"},
   {TOK_EQ,         "="},
   {TOK_EQ_2,       "(=)"},
   {TOK_COMP_MODE,  "comp"},
   {TOK_WRITE_MODE, "write"},
   {TOK_CLEAR_MODE, "clear"},
   {TOK_AC,         "AC"},
   {TOK_C,          "C"},
   {TOK_RETURN,     "\r"},
   {TOK_K,          "k"},
   {TOK_I,          "i"},
   {TOK_IM,         "IM"},
   {TOK_MJ,         "mj"},
   {TOK_MEM_PL,     "m+"},
   {TOK_MEM_MN,     "m-"},
   {TOK_MEM_REC,    "mr"},
   {TOK_MEM_CLR,    "mc"},
   {TOK_ENT,        "ent"},
   {TOK_ANS,        "ans"},
   {TOK_END,        "end"},
   {TOK_XY,         "AN"},
   {TOK_SQRT,       "sqrt"},
   {TOK_PLMN,       "chs"},
   {TOK_CUT_F,      "cutf"},
   {TOK_CUT_CUT,    "cutcut"},
   {TOK_CUT_54,     "cut54"},
   {TOK_PLACES_0,   "places0"},
   {TOK_PLACES_1,   "places1"},
   {TOK_PLACES_2,   "places2"},
   {TOK_PLACES_3,   "places3"},
   {TOK_PLACES_4,   "places4"},
   {TOK_PLACES_6,   "places6"},
   
  };

#define NUM_EMULATOR_KEY_MAP (sizeof(emulator_key_map)/sizeof(TOKEN_INFO))

void cli_emulator(void)
{
  int done = 0;
  char last_display[DISPLAY_WIDTH+1];
  
  // Sit in a loop getting keys from the USB and outputting the display
  // when something changes
  strcpy(last_display, "");

  printf("\nUSB Emulation");
  printf("\nKeys from USB, display shown on USB, calculator keyboard will not function in this mode.");
  printf("\nESC to exit");
  printf("\n? for help on keys and what tokens they generate");
  printf("\nBackspace to clear input buffer");
  printf("\n");

#define KEY_QUEUE_LEN  10
#define LINE_LENGTH  78
  
  char key_queue[KEY_QUEUE_LEN+1];
  char tmp_queue[KEY_QUEUE_LEN+10];
  char col_data[LINE_LENGTH+1];
  char padding[LINE_LENGTH+1];
  char new_display[DISPLAY_WIDTH+5];
  int dp;
  
  char keystr[2];
  int col = 0;
  
  keystr[1] = '\0';
  
  memset(key_queue, '\0', KEY_QUEUE_LEN);
  
  while(!done)
    {
        int  key;
	
	if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
	  {
	    //printf("\nKey:%03d 0x%03X", key, key);
	    
	    switch(key)
	      {
	      case 27:
		done = 1;
		break;

	      case '?':
		printf("\nKey map");
		printf("\n=======");
		printf("\n");

#define NUM_COLS  2
#define COL_WIDTH ((LINE_LENGTH/NUM_COLS)-1)
		
		for(int i=0; i<NUM_EMULATOR_KEY_MAP; i+=NUM_COLS)
		  {
		    col = 0;
		    printf("\n");
		    
		    for(int c=0; c<NUM_COLS; c++)
		      {
			sprintf(col_data, "%s -> %s", emulator_key_map[i+c].tok_name, name_of_token(emulator_key_map[i+c].tok_value));
			memset(padding, ' ', COL_WIDTH);
			padding[COL_WIDTH-strlen(col_data)] = '\0';
			printf("%s%s", col_data, padding);
		      }
		  }
		
		printf("\n");
		break;
		
	      case 8:
		key_queue[0] = '\0';
		break;
	      
	      case '!':
		cli_boot_mass();
		break;
		       
	      default:
		// Add the key to the queue of characters
		strcpy(tmp_queue, key_queue);
		keystr[0] = key;
		strcat(tmp_queue, keystr);
		if( strlen(tmp_queue) > KEY_QUEUE_LEN )
		  {
		    strcpy(key_queue, &(tmp_queue[strlen(tmp_queue)-KEY_QUEUE_LEN]));
		  }
		else
		  {
		    strcpy(key_queue, tmp_queue);
		  }
		
		// Check to see if we have a keystroke
		for(int i=0; i<NUM_EMULATOR_KEY_MAP; i++)
		  {
		    if( strncmp(emulator_key_map[i].tok_name, key_queue, strlen(emulator_key_map[i].tok_name)) == 0 )
		      {
			queue_token(emulator_key_map[i].tok_value);

			strcpy(tmp_queue, &(key_queue[strlen(emulator_key_map[i].tok_name)]));

			strcpy(key_queue, tmp_queue);

			// All done
			break;
		      }
		  }
		break;
	      }
	  }

	// Handle keys etc.
	process_hw_inputs();
	
	// We have to drive the FSMs here as the main() loop nornmally does that
	// and this function is taking over the duties of that code while we run]
	// in the 'emulator'
	drive_fsms();

	// Add in the decimal point if it is to be displayed

	dp = display_decimal_pos;
	  
	if( (display_decimal_pos >=0) && (display_decimal_pos < DISPLAY_WIDTH) )
	  {
	    memset(new_display, '\0', DISPLAY_WIDTH+2);
	    strncpy(new_display, &(display[0]), dp+1);
	    strcat(new_display, ".");
	    strcat(new_display, &(display[dp+1]));
	    //printf("\nnd='%s'   %d", new_display, dp);
	  }
	else
	  {
	    strcpy(new_display, display);
	  }

	// If display has changed then update the display
	if( strncmp(last_display, display, DISPLAY_WIDTH) != 0 )
	  {
	    printf("\n'%s'", new_display);
	    strncpy(last_display, display, DISPLAY_WIDTH);
	    last_display[DISPLAY_WIDTH] = '\0';
	  }
    }
}

////////////////////////////////////////////////////////////////////////////////

SERIAL_COMMAND serial_cmds[] =
  {
   {
    'Q',
    "Run as Emulator",
    cli_emulator,
   },
   {
    'p',
    "Program mode",
    cli_program_mode,
   },
   {
    'c',
    "Comp mode",
    cli_comp_mode,
   },
   {
    'r',
    "Run mode",
    cli_run_mode,
   },
   {
    'h',
    "Serial command help",
    serial_help,
   },
   {
    'l',
    "Load emulation RAM from flash slot",
    cli_load_ram,
   },
   {
    'L',
    "Load memory or program from flash slot",
    cli_load_ram_mp,
   },
   {
    's',
    "Save emulation RAM to flash slot",
    cli_save_ram,
   },
   {
    'S',
    "Save memory or program to flash slot",
    cli_save_ram_mp,
   },
   {
    '?',
    "Serial command help",
    serial_help,
   },
   {
    'I',
    "Display Information",
    cli_display_dump,
   },
   {
    '=',
    "Dump X",
    cli_display_dump_x,
   },
   {
    'd',
    "Display RAM",
    cli_display_prog_ram,
   },
   {
    '+',
    "Display KBD data",
    cli_kbd_data,
   },
   {
    'T',
    "Test BCD",
    cli_bcd_test,
   },
   {
    '$',
    "Display trace",
    cli_display_trace,
   },
   {
    'z',
    "Zero parameter",
    cli_zero_parameter,
   },
   {
    'E',
    "Erase program slot",
    cli_erase_program_slot,
   },
   {
    'D',
    "Display program slot",
    cli_display_program_slot,
   },
   {
    'a',
    "Display All Programs",
    cli_slot_program_list,
   },
   {
    'A',
    "Set Address",
    cli_set_address,
   },
   {
    'W',
    "Write Byte",
    cli_write_byte,
   },
   {
    '<',
    "Display GPIO Functions",
    print_gpio_functions,
   },
   {
    '_',
    "Clear All RAM",
    cli_clear_ram,
   },
   {
    '-',
    "Clear Memory or Program RAM",
    cli_clear_ram_mp,
   },
   {
    '0',
    "*Digit",
    cli_digit,
   },
   {
    '1',
    "*Digit",
    cli_digit,
   },
   {
    '2',
    "*Digit",
    cli_digit,
   },
   {
    '3',
    "*Digit",
    cli_digit,
   },
   {
    '4',
    "*Digit",
    cli_digit,
   },
   {
    '5',
    "*Digit",
    cli_digit,
   },
   {
    '6',
    "*Digit",
    cli_digit,
   },
   {
    '7',
    "*Digit",
    cli_digit,
   },
   {
    '8',
    "*Digit",
    cli_digit,
   },
   {
    '9',
    "*Digit",
    cli_digit,
   },
   {
    'B',
    "Label Slot",
    cli_label_slot,
   },
   {
    'b',
    "List Slot Labels",
    cli_slot_label_list,
   },
   {
    't',
    "Enter Text Parameter",
    cli_text_param,
   },
   {
    '.',
    "Toggle auto increment of parameter",
    cli_toggle_auto_increment_parameter,
   },
   {
    ',',
    "Toggle auto increment of address",
    cli_toggle_auto_increment_address,
   },
   {
    '!',
    "Boot to mass storage",
    cli_boot_mass,
   },
   {
    '*',
    "Turn on LED",
    cli_led_on,
   },
   {
    '@',
    "Run test sequence",
    cli_run_test_sequence,
   },
   {
    '~',
    "Run all test sequence",
    cli_run_all_test_sequence,
   },
   {
    '#',
    "Display test sequence results",
    cli_display_test_sequence_results,
   },
  };



void serial_help(void)
{
  printf("\n");
  
  for(int i=0; i<sizeof(serial_cmds)/sizeof(SERIAL_COMMAND);i++)
    {
      if( *(serial_cmds[i].desc) != '*' )
	{
	  printf("\n%c:   %s", serial_cmds[i].key, serial_cmds[i].desc);
	}
    }
  printf("\n0-9: Enter parameter digit");
}


void prompt(void)
{
  printf("\n\n(Text Parameter:'%s'", text_parameter);
  printf("\n(Parameter (Program Num):%d (%04X) %c, Address (Slot Num):%d (%04X) %c) >",
	 parameter, parameter, auto_increment_parameter?'A':' ',
	 address,   address,   auto_increment_address?  'A':' ');
}


////////////////////////////////////////////////////////////////////////////////
//
// Serial CLI Handling
//
////////////////////////////////////////////////////////////////////////////////

int pcount = 0;
int periodic_read = 0;

void serial_loop()
{
  int  key;
  
  if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
    {
      for(int i=0; i<sizeof(serial_cmds)/sizeof(SERIAL_COMMAND);i++)
	{
	  if( serial_cmds[i].key == key )
	    {

	      keypress = key;
	      (*serial_cmds[i].fn)();
	      prompt();
	      break;
	    }
	}
    }
  else
    {
      // I have found that I need to send something if the serial USB times out
      // otherwise I get lockups on the serial communications.
      // So, if we get a timeout we send a space and backspace it. And
      // flush the stdio, but that didn't fix the problem but seems like a good idea.
      stdio_flush();
      //printf(" \b");
    }
}

////////////////////////////////////////////////////////////////////////////////

// Process commands
//
// Commands are put into M<command>
//
// Form:
// ccsnnn E 88
//
// (This is normalised to c.csnnn E 93)
//
// cc  : Command number
// s   : Status
// nnn : Numeric parameter
//

void set_status(int n)
{
  int stat_byte = *(command_ptr+4);

  stat_byte &= 0xF0;
  stat_byte |= (n & 0xF);
  
  *(command_ptr+4) = stat_byte;
}

#define CATALOG_WIDTH  30

int slot_blank_ma(int num)
{
  if( *((flash_slot_contents+num*FLASH_PROGRAM_SLOT_SIZE)) == 0xFF )
    {
      // Blank
      return(1);
    }

  // Not blank
  return(0);
}

void cmd_catalog(int num)
{
  int bitmask = 0;
  int bitnum = 2;

  uint8_t *cmd_ptr2 = command_ptr+8;
  
  for(int j=num; j<num+CATALOG_WIDTH; j++)
    {
      int i = ((CATALOG_WIDTH-1) - (j-num))+3;
      //i = j + 3 - num;
      
      bitmask = (1 << bitnum);
      
      *(cmd_ptr2+i/6) &= ~bitmask;

      printf("\n%02X %02X %d %d", bitnum, i/6, num, slot_blank_ma(j));
      
      if( slot_blank_ma(j) )
	{
	}
      else
	{
	  *(cmd_ptr2+i/6) |= bitmask;
	}

      bitnum--;

      switch(bitnum)
	{
	case 3:
	  *(cmd_ptr2+i/6) &= ~(1 << 3);
	  bitnum--;
	  break;

	case -1:
	  
	  *(cmd_ptr2+i/6) &= ~(1 << 7);
	  bitnum = 6;
	  break;
	}
    }

  *(cmd_ptr2+6) = 0x09;
  *(cmd_ptr2+7) = 0xF1;
}

void status_good(void)
{
  set_status(STATUS_CHAR_GOOD);
}

void status_bad(void)
{
    set_status(STATUS_CHAR_GOOD);
}

void process_commands(void)
{

  // Do we have a valid command?
  if( *(command_ptr+6) == 0x93 && *(command_ptr+7) == 0xF1 &&
      
    // Status is zero if we haven't executed the command
      ((*(command_ptr+4) & 0xF)==0)
      )
    {
      
      // This could be a command
      int cmd_code = GET_SPLIT_BYTE(4);
      int num      = bcd_to_dec(command_ptr+3) * 10;
      num += bcd_to_dec(command_ptr+2)/10;

      printf("\nCmd code:%02X", cmd_code);
      printf(" Num = %04X %d",num, num);

      switch( cmd_code )
	{
	case CMD_LOAD:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS) )
	    {
	      status_good();
	      load_ram(num, DO_MP_BOTH);
	    }
	  else
	    {
	      status_bad();
	    }
	  break;

	case CMD_LOAD_M:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS) )
	    {
	      status_good();
	      load_ram(num, DO_MP_MEMORIES);
	    }
	  else
	    {
	      status_bad();
	    }
	  break;
	  
	case CMD_LOAD_P:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS) )
	    {
	      status_good();
	      load_ram(num, DO_MP_PROGRAM);
	    }
	  else
	    {
	      status_bad();
	    }
	  break;

	case CMD_SAVE:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS) )
	    {
	      status_good();
	      save_ram(num, DO_MP_BOTH);
	    }
	  else
	    {
	      status_bad();
	    }
	  break;

	case CMD_SAVE_M:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS) )
	    {
	      status_good();
	      save_ram(num, DO_MP_MEMORIES);
	    }
	  else
	    {
	      status_bad();
	    }
	  break;
	  
	case CMD_SAVE_P:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS) )
	    {
	      save_ram(num, DO_MP_PROGRAM);
	      status_good();
	    }
	  else
	    {
	      status_bad();
	    }
	  break;

	case CMD_CLR:
	  status_good();
	  clear_ram_mp(DO_MP_BOTH);
	  break;

	case CMD_CLR_M:
	  status_good();
	  clear_ram_mp(DO_MP_MEMORIES);
	  break;

	case CMD_CLR_P:
	  status_good();
	  clear_ram_mp(DO_MP_PROGRAM);
	  break;

	case CMD_CATALOG:
	  if( (num >=0) && (num<FLASH_PROGRAM_NUM_SLOTS-CATALOG_WIDTH) )
	    {
	    cmd_catalog(num);
	    status_good();
	    }
	  else
	    {
	      status_bad();
	    }
	  break;
	  
	default:
	  break;
	}
    }
}


////////////////////////////////////////////////////////////////////////////////

void  print_gpio_functions(void)
{
  for(int i=0; i<32; i++)
    {
      printf("\n%02d: %d", i, gpio_get_function(i));
    }
  printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
//
//
// Drive a bit pattern to the MAX6921, which will be a VFD
// drive pattern. Pattern is low 16 bits of hi int and low 4 bits of lo int
//
// Blank input is unchanged 
//

#define MAX6921_DELAY 1

void drive_max6921(int hi, int lo)
{
  int i;
  
  // Set up
  gpio_put(P_MAX_CLK, 0);
  gpio_put(P_MAX_LOAD, 0);

  // Clock out first 16 bits
  for(i=0; i<20; i++)
    {
      // Set up data
      if( hi & 0x80000 )
	{
	  gpio_put(P_MAX_DIN, 1);
	}
      else
	{
	  gpio_put(P_MAX_DIN, 0);
	}

      // Clock data in on rising edge
      sleep_us(MAX6921_DELAY);
      gpio_put(P_MAX_CLK, 1);

      sleep_us(MAX6921_DELAY);
      // Drop clock back down
      gpio_put(P_MAX_CLK, 0);

      // Shift bits up
      hi <<= 1;
    }

  // Clock out last 4 bits
  for(i=0; i<20; i++)
    {
      // Set up data
      if( lo & 0x80000 )
	{
	  gpio_put(P_MAX_DIN, 1);
	}
      else
	{
	  gpio_put(P_MAX_DIN, 0);
	}

      sleep_us(MAX6921_DELAY);
      // Clock data in on rising edge
      gpio_put(P_MAX_CLK, 1);

      sleep_us(MAX6921_DELAY);
      // Drop clock back down
      gpio_put(P_MAX_CLK, 0);

      // Shift bits up
      lo <<= 1;
    }

  // All data locked in, load it
  sleep_us(MAX6921_DELAY);
  gpio_put(P_MAX_LOAD, 1);
  sleep_us(MAX6921_DELAY);
  gpio_put(P_MAX_LOAD, 0);
  
  
}

////////////////////////////////////////////////////////////////////////////////
//
// VFD drive
//
////////////////////////////////////////////////////////////////////////////////

// The drivers are set using the bottom 20 bits of two integers. This gives
// 40 bits that can be controlled. Some are VFD drive and some are other GPIOs.
//
//
// All drive of fields of signals come through this function
// All fields fit into an integer

SF_INFO  sf_info[SF_NUM_FIELDS] =
  {
   {0x00000000, 0x00003FFF,  0,  0},   // Digits
   {0x000000FF, 0x00000000,  0,  0},   // Segments
   {0x00000000, 0x0001C000,  0, 14},   // LED anodes
   {0x000F8000, 0x00000000, 15,  0},   // KBD drive
   {0x00000000, 0x00000000,  0,  0},   // Filament drive
   {0x00007000, 0x00000000, 12,  0},   // Switch drive
  };

// Only used by core0
void set_serial_field(int field, int value)
{
  int mask_a, mask_b;
  int shift_a, shift_b;
  
  mask_a  = sf_info[field].bitmask_a;
  mask_b  = sf_info[field].bitmask_b;
  shift_a = sf_info[field].shift_a;
  shift_b = sf_info[field].shift_b;
  
  shadow_sf_a &= ~mask_a;
  shadow_sf_b &= ~mask_b;

  shadow_sf_a |= ((value << shift_a ) & mask_a);
  shadow_sf_b |= ((value << shift_b ) & mask_b);

  // Send the data again
  //drive_max6921(shadow_sf_a, shadow_sf_b);
}

// Only used by  core1
void set_serial_field2(int field, int value)
{
  int mask_a, mask_b;
  int shift_a, shift_b;
  
  mask_a  = sf_info[field].bitmask_a;
  mask_b  = sf_info[field].bitmask_b;
  shift_a = sf_info[field].shift_a;
  shift_b = sf_info[field].shift_b;
  
  shadow2_sf_a &= ~mask_a;
  shadow2_sf_b &= ~mask_b;

  shadow2_sf_a |= ((value << shift_a ) & mask_a);
  shadow2_sf_b |= ((value << shift_b ) & mask_b);

  // Send the data again
  //drive_max6921(shadow_sf_a, shadow_sf_b);
}

int char_to_segs(char c)
{
  for(int i=0; i<CHAR_SET_LEN; i++)
  {
    if( char_set[i].character == c )
      {
	return(char_set[i].segment_bitmap);
      }
  }
  return(0);
}
////////////////////////////////////////////////////////////////////////////////
//
//  Filament drive
//
////////////////////////////////////////////////////////////////////////////////

#define H_ON    0
#define H_OFF   1
#define L_ON    1
#define L_OFF   0

void filament_off(void)
{
  // L GPIOs drive low and H GPIOs drive high
  // Turns drive off

  gpio_put(P_VFI_D_H, H_OFF);
  gpio_put(P_VF_D_H,  H_OFF);
  gpio_put(P_VFI_D_L, L_OFF);
  gpio_put(P_VF_D_L,  L_OFF);
}

// Filament on, direction 1
void filament_on_1(void)
{
  gpio_put(P_VFI_D_H, H_OFF);    // VFI H off
  gpio_put(P_VF_D_L,  L_OFF);
  
  gpio_put(P_VFI_D_L, L_ON);    // VFI L on
  gpio_put(P_VF_D_H,  H_ON);  
}

// Filament on, direction 2
void filament_on_2(void)
{
  // Turn off then on
  gpio_put(P_VFI_D_L, L_OFF);    // VFI L on
  gpio_put(P_VF_D_H,  H_OFF);  

  gpio_put(P_VFI_D_H, H_ON);    // VFI H off
  gpio_put(P_VF_D_L,  L_ON);
}

// used to skip decimal point character
int offset = 0;
int switch_drive = 1;
int switch_num = 0;

////////////////////////////////////////////////////////////////////////////////
//
//
// Drive the serial drivers with core 1
//
//
////////////////////////////////////////////////////////////////////////////////

void core1_main(void)
{
  irq_set_mask_enabled( 0xFFFFFFFF, 0 );
  
  while(1)
    {
      // Scan the keyboard
      kb_tick++;

#if DEBUG_CORE1_TICK
      if( kb_tick == 99 )
	{
	  printf("\nTk:");
	}
#endif
      if( kb_tick == 95 )
	{
	  // read KBD sense lines
	  kbd_sense_data[kb_drv_num] = kbd_sense();

	  // read switch sense lines
	  sw_sense_data[switch_num] = sw_sense();
	}

      if( kb_tick > 100 )
	{
	  kb_tick = 0;
	  kb_drv_num++;
	  
	  kb_drv <<=1;
	  
	  if( kb_drv == 0x20 )
	    {
	      kb_drv = 0x01;
	      kb_drv_num = 0;
	    }

	  set_serial_field(SF_KBD_DRV, kb_drv);
	  
	  // Drive the switch lines and capture sense
	  switch_drive <<= 1;
	  switch_num++;
	  if( switch_drive == (1 << NUM_SWITCH_DRIVE) )
	    {
	      switch_drive = 1;
	      switch_num = 0;
	    }

	  set_serial_field(SF_SWITCH_DRV, switch_drive);
	  	}

      set_serial_field(SF_KBD_DRV, kb_drv);
      
      // Drive the filament alternately
      filament_drive = 1-filament_drive;
	
      if( filament_drive )
	{
	  filament_on_1();
	}
      else
	{
	  filament_on_2();
	}

      set_serial_field(SF_VFD_DIGITS,   digit_drive);
      
      int new_display_decimal_pos = display_decimal_pos;

      // The decimal point is a special character, we use the point of the display
      int drive_num = NUM_DIGITS - digit_num - 1;
#if ERRATA_UNJUMBLE
      switch(drive_num)
	{
	case 0:
	  drive_num = 1;
	  break;
	  
	case 1:
	  drive_num = 2;
	  break;
	  
	case 2:
	  drive_num = 0;
	  break;
	}
#endif
      
      if( (drive_num == new_display_decimal_pos) && (new_display_decimal_pos != -1) )
	{
	  set_serial_field(SF_VFD_SEGMENTS, char_to_segs(display[drive_num]) | 0x80);
	}
      else
	{
	  set_serial_field(SF_VFD_SEGMENTS, char_to_segs(display[drive_num]));
	}

      // Move on to next digit
      digit_num++;
      digit_drive <<= 1;
      
      if( digit_drive == (1<<NUM_DIGITS) )
	{
	  digit_drive = 1;
	  digit_num = 0;
	}

      // Scan the LEDs
      led_drive = 1-led_drive;

      // Copy data
      shadow2_sf_a = shadow_sf_a;
      shadow2_sf_b = shadow_sf_b;
      
      if( led_drive )
	{
	  // Put correct anodes into the shadow
	  set_serial_field2(SF_LED_ANODES, cathode_1_led_data);
	}
      else
	{
	  // Put correct anodes into the shadow
	  set_serial_field2(SF_LED_ANODES, cathode_2_led_data);
	}
      
      drive_max6921(shadow2_sf_a, shadow2_sf_b);
      
      if( led_drive )
	{
	  gpio_put(P_CATHODE1, 1);
	  gpio_put(P_CATHODE2, 0);

	}
      else
	{
	  gpio_put(P_CATHODE1, 0);
	  gpio_put(P_CATHODE2, 1);
	}

      // Let the display be stable for a while
      sleep_us(100);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Core 1 continuously scans the keyboard. This function then looks at the
// scan data and returns the keycode of any key seen.
//
// If test code is compiled in then keys come from the test sequences
// 
////////////////////////////////////////////////////////////////////////////////


int poll_keyboard(CALCULATOR_STATE *state)
{
  int code;
  int token = TOK_NO_TOK;

  // If test sequences are compiled in then the sequences override the
  // real keyboard

#if TEST_SEQUENCES
  if( test_sequence != SEQ_NONE )
    {
      TOKEN ttok;
      
      // Running a sequence
      TOKEN *sq = test_seq[test_sequence].seq;
      ttok = *(sq+seq_i);
      seq_i ++;

      if( ((ttok == TOK_NO_TOK) || (ttok == CTOK_TEST)) && (state->exec_running) )
	{
	  // If MISS LED is on end the test
	  if( !get_led_state(LED_MISS) )
	    {
	      // Wait for any program to stop running
	      seq_i--;
	      return(TOK_NO_TOK);
	    }
	}

#if DEBUG_TEST_SEQ
      printf("\nTest Seq:%d Tok:%03X", test_sequence, ttok);
#endif
      
      if( (ttok == TOK_NO_TOK) || (ttok == CTOK_TEST) )
	{
	  // End of sequence, or CTOK_TEST, check results
	  
	  if( test_seq[test_sequence].results[test_result_i].test_x )
	    {
	      if( state->x == test_seq[test_sequence].results[test_result_i].x )
		{
		  // Pass
		  printf("\nSeq:%d Passed", test_sequence);

		}
	      else
		{
		  // Fail
		  printf("\nSeq:%d Failed", test_sequence);
		  test_seq[test_sequence].passed = 0;
		}
	      
	      
	      if( (state->x - test_seq[test_sequence].results[test_result_i].x).AbsoluteValue() < TEST_E )
		{
		  // Pass
		  printf("\nSeq:%d Approx Passed", test_sequence);
		}
	      else
		{
		  // Fail
		  printf("\nSeq:%d Approx Failed", test_sequence);
		  test_seq[test_sequence].approx_passed = 0;
		}

	    }

	  // Test result
	  
	  if( test_seq[test_sequence].results[test_result_i].display != NULL )
	    {
	      if( strncmp(test_seq[test_sequence].results[test_result_i].display, display, strlen(test_seq[test_sequence].results[test_result_i].display)) != 0 )
		{
		  printf("\n----------------- Expected: '%s' Actual: '%s'", test_seq[test_sequence].results[test_result_i].display, display);
		  test_seq[test_sequence].passed = 0;

		  // Store result we got away
		  strncpy(test_seq[test_sequence].actual_results_display, display, DISPLAY_WIDTH); 
		}
	    }

	  // Move to next result set if required
	      if( ttok == CTOK_TEST )
	      {
		test_result_i++;
	      }
	}

      // If end of sequence, move to next
      if( ttok == TOK_NO_TOK )
	{
	  if( !test_sequence_all )
	    {
	      test_sequence = SEQ_NONE;
	      seq_i = 0;
	    }
	  else
	    {
	      test_sequence++;
	      seq_i = 0;
	      test_result_i = 0;
	      test_seq[test_sequence].passed = 1;
	      test_seq[test_sequence].approx_passed = 1;

	      if( test_sequence == NUM_TESTS )
		{
		  test_sequence = SEQ_NONE;
		}
	    }
	}

      if( ttok != CTOK_TEST )
	{
	  return(ttok);
	}
      else
	{
	  return(TOK_NO_TOK);
	}
    }
  
#endif
  
  code = keycode();
  token = keycode_to_token(code);

#if DEBUG_KB_POLL
  printf("\n\nToken:%03X", token);
#endif
  
  if( token != 0 )
    {
      if( held_key == 0 )
	{
	  // New key
	  held_key = token;

	  // Return token here for th enew key
	}
      else
	{
	  if( token == held_key )
	    {
	      // Key still held
	      // Return no key
	      token = TOK_NO_TOK;
	    }
	  else
	    {
	      // New key
	      held_key = token;
	    }
	}
    }
  else
    {
      held_key = 0;
      token = TOK_NO_TOK;
    }

#if DEBUG_KB_POLL
  printf("\nBefore filter:%03X", token);
#endif
  
  // Apply keyboard filter
  token = (state->keypress_filter_fn)(token);

#if DEBUG_KB_POLL
  printf("\nAfter filter:%03X", token);
#endif

  return(token);
}

////////////////////////////////////////////////////////////////////////////////
//
// Poll  slide switches
//
////////////////////////////////////////////////////////////////////////////////

// The mode switch is polled and tokens generated if the mode changes. That then
// feeds into state machines. When testing, those tokens can be injected and
// simulate slide switch movements.
// The CUT and PLACES switches could be read and set up as a value that is used
// when formatting the display, rather than use the same scheme as the mode switch.
// If that was done then testing wouldn't be ale to test the code for the CUT and
// PLACES switches. So, they are done in the same way.
//
////////////////////////////////////////////////////////////////////////////////

int switch_mode_last   = SWITCH_MODE_COMP;
int switch_mode        = SWITCH_MODE_COMP;
int switch_cut_last    = SWITCH_CUT_F;
int switch_cut         = SWITCH_CUT_F;
int switch_places_last = SWITCH_PLACES_0;
int switch_places      = SWITCH_PLACES_0;


typedef struct _SWITCH_SENSE_MAP
{
  int sense;
  int token;
} SWITCH_SENSE_MAP;

typedef struct _SWITCH_INFO
{
  int sense_index;

  int *current_setting;
  int *last_setting;
  
  SWITCH_SENSE_MAP map[];
} SWITCH_INFO;

SWITCH_INFO mode_switch_info =
  {
   MODE_SWITCH_SENSE,
   &switch_mode,
   &switch_mode_last,
   {
    {SWITCH_MODE_COMP,  TOK_COMP_MODE},
    {SWITCH_MODE_WRITE, TOK_WRITE_MODE},
    {SWITCH_MODE_CLEAR, TOK_CLEAR_MODE},
    {SWITCH_NULL,       TOK_NONE},
   }
  };

SWITCH_INFO cut_switch_info =
  {
   CUT_SWITCH_SENSE,
   &switch_cut,
   &switch_cut_last,
   {
    {SWITCH_CUT_F,   TOK_CUT_F},
    {SWITCH_CUT_CUT, TOK_CUT_CUT},
    {SWITCH_CUT_54,  TOK_CUT_54},
    {SWITCH_NULL,    TOK_NONE},
   }
  };

SWITCH_INFO places_switch_info =
  {
   PLACES_SWITCH_SENSE,
   &switch_places,
   &switch_places_last,
   {
    {SWITCH_PLACES_0,   TOK_PLACES_0},
    {SWITCH_PLACES_1,   TOK_PLACES_1},
    {SWITCH_PLACES_2,   TOK_PLACES_2},
    {SWITCH_PLACES_3,   TOK_PLACES_3},
    {SWITCH_PLACES_4,   TOK_PLACES_4},
    {SWITCH_PLACES_6,   TOK_PLACES_6},
    {SWITCH_NULL,       TOK_NONE},
   }
  };

int core_poll_switch(CALCULATOR_STATE *state, SWITCH_INFO *info)
{
  int tok = TOK_NONE;
  
  // Set up the switch settings and generate tokens if
  // it has changed from the last time it was read
  
  *(info->current_setting) = sw_sense_data[info->sense_index];

  if( *(info->current_setting) != *(info->last_setting) )
    {
      for(int i=0; info->map[i].sense != SWITCH_NULL; i++)
	{
	  if( info->map[i].sense == *(info->current_setting) )
	    {
	      tok = info->map[i].token;
	      break;
	    }
	}
    }

  *(info->last_setting) = *(info->current_setting);
  return(tok);
}

int poll_mode_switch(CALCULATOR_STATE *state)
{
  return(core_poll_switch(state, &mode_switch_info));

}

int poll_cut_switch(CALCULATOR_STATE *state)
{
  return(core_poll_switch(state, &cut_switch_info));

}

int poll_places_switch(CALCULATOR_STATE *state)
{
  return(core_poll_switch(state, &places_switch_info));

}

int poll_mode_switch2(CALCULATOR_STATE *state)
{
  int tok = TOK_NONE;
  
  // Set up the switch settings and generate tokens if
  // it has changed from the last time it was read
  switch_mode = sw_sense_data[MODE_SWITCH_SENSE];

  if( switch_mode != switch_mode_last )
    {
      // Generate token
      switch(switch_mode)
	{
	case SWITCH_MODE_COMP:
	  tok =TOK_COMP_MODE;
	  break;

	case SWITCH_MODE_WRITE:
	  tok =TOK_WRITE_MODE;
	  break;

	case SWITCH_MODE_CLEAR:
	  tok =TOK_CLEAR_MODE;
	  break;
	}
    }

  switch_mode_last = switch_mode;
  return(tok);
}

////////////////////////////////////////////////////////////////////////////////
//
// Set the display up in write mode
//
////////////////////////////////////////////////////////////////////////////////


void set_display_write_zero(CALCULATOR_STATE *s)
{
  sprintf(display, "             0");
}

void set_display_write_prn(CALCULATOR_STATE *s)
{
  char d[NUM_DIGITS+5];
  
  if( s->prog_entry == 0 )
    {
      sprintf(d, "%04d         P", free_steps(s));
    }

  if( (s->prog_entry >=1) && (s->prog_entry <=9) )
    {
      sprintf(d, "%04d        P%d", free_steps(s), s->prog_entry);
    }

  if( (s->prog_entry >=10) && (s->prog_entry <=99) )
    {
      sprintf(d, "%04d       P%d", free_steps(s), s->prog_entry);
    }

  strcpy(display, d);
}

////////////////////////////////////////////////////////////////////////////////
//
// Display when program steps shown
//
////////////////////////////////////////////////////////////////////////////////

#if DISPLAY_WRITE_FX201P

void set_display_write_steps(CALCULATOR_STATE *s)
{
  int dotp = 0;
  char str[40];
  int c_step = s->current_step;
  int step_tok;
  
  char stepa[5];
  char stepb[5];
  char stepc[5];

  sprintf(stepa, "  ");
  sprintf(stepb, "  ");
  sprintf(stepc, "  ");
  
  if( c_step >= 3 )
    {
      step_tok = s->program_step[s->current_program][s->current_step-2];
      step_tok &=0xFF;
      sprintf(stepa, "%02X", step_tok);
    }

  if( c_step >= 2 )
    {
      step_tok = s->program_step[s->current_program][s->current_step-1];
      step_tok &=0xFF;
      sprintf(stepb, "%02X", step_tok);
    }
        
  if( c_step >= 1 )
    {
      step_tok = s->program_step[s->current_program][s->current_step-0];
      step_tok &=0xFF;
      sprintf(stepc, "%02X", step_tok);
    }
  
  // Display current step number and the steps at that location
  sprintf(display, "%03d   %2s %2s %2s", s->current_step, stepa, stepb, stepc);
}
#endif

//------------------------------------------------------------------------------

#if DISPLAY_WRITE_PRO101

// The current step is in a store reference
// We are pointing at one of the digits of the referene

char refstr[5];

char *get_store_reference(CALCULATOR_STATE *s)
{
  int step_tok;
  char chr[3];
  int ref_start_step = 0;
  int done;
  int found;
  
  refstr[0] = '\0';
  chr[1] = '\0';
  
  // Find the start of the reference
  done = 0;
  found = 0;
  ref_start_step = 0;
  int i;

#if DEBUG_GET_STORE_REF
  printf("\n%s", __FUNCTION__);
  printf("\nLooking bacwards for non-numeric...");
#endif
  
  for(i=s->current_step; (i>0) && !done; i--)
    {
      step_tok = s->program_step[s->current_program][i];

#if DEBUG_GET_STORE_REF
      printf("\n  tok=%03X", step_tok);
#endif

      switch(step_tok)
	{
	case TOK_0:
	case TOK_1:
	case TOK_2:
	case TOK_3:
	case TOK_4:
	case TOK_5:
	case TOK_6:
	case TOK_7:
	case TOK_8:
	case TOK_9:
	  break;

	default:
	  // We have the non-numeric at the start, add one and that is the first reference digit
	  ref_start_step = i+1;
	  done = 1;
	  found = 1;
	  break;
	}
    }

  // If we have reached the start of the program and not found a non-numeric
  if( (i == 0) && (found == 0) )
    {
#if DEBUG_GET_STORE_REF
      printf("\n  Start of program");
#endif

      ref_start_step = 1;
      found = 1;
    }
  
  // Found it or reached start of program
  if( found )
    {
      // Build the reference
      done = 0;
      found = 0;

#if DEBUG_GET_STORE_REF
      printf("\n  Found start, building ref");
#endif

      for(int i=ref_start_step; (i<NUM_STEPS) && !done; i++)
	{
	  step_tok = s->program_step[s->current_program][i];

#if DEBUG_GET_STORE_REF
	  printf("\n  tok=%03X", step_tok);
#endif

	  switch(step_tok)
	    {
	    case TOK_0:
	    case TOK_1:
	    case TOK_2:
	    case TOK_3:
	    case TOK_4:
	    case TOK_5:
	    case TOK_6:
	    case TOK_7:
	    case TOK_8:
	    case TOK_9:
	      chr[0] = (s->program_step[s->current_program][i]) - TOK_0 +'0';	      
	      strcat(refstr, chr);
	      break;
	      
	    default:
	      // We have the non-numeric at the end, we are done
	      done = 1;
	      break;
	    }
	}
    }

#if DEBUG_GET_STORE_REF
  printf("\n  returning value:%s", refstr);
#endif

  return(refstr);
}

// Only update the display if something has changed
int last_current_step = -1;

void set_display_write_steps(CALCULATOR_STATE *s)
{
  int dotp = 0;
  char str[40];
  int c_step = s->current_step;
  int step_tok;
  int i;
  int found_k       = 0;
  int found_goto    = 0;;
  int found_prognum = 0;
  int found_if      = 0;
  int found_step    = 0;
  int found_store_ref  = 0;
  int done = 0;
  int found = 0;
  int step_i;

  // Only do work if the current step is different to last time
  if( last_current_step == (s->current_step) )
    {
      return;
    }

  last_current_step = s->current_step;
  
  // Determine the format to display the step in, we can look back in th eprogram for
  // special tokens. Numewric tokens are displayed as constants or references and are
  // multi digit when that happens. Everything else is a keycode

  // current_step can't be affected as we are using it to enter keystrokes
  
  // Step 0 is the program number
  if( s->current_step == 0 )
    {
      sprintf(display, "P%-2d           ", s->current_program);
      return;
    }

  int dot_pos = -1;
  
  step_tok = s->program_step[s->current_program][s->current_step];
#if DEBUG_GET_STORE_REF
  printf("\n%s", __FUNCTION__);
  printf("\n  tok=%03X", step_tok);
#endif

  switch(step_tok)
    {
    case TOK_K:
    case TOK_0:
    case TOK_1:
    case TOK_2:
    case TOK_3:
    case TOK_4:
    case TOK_5:
    case TOK_6:
    case TOK_7:
    case TOK_8:
    case TOK_9:
    case TOK_DOT:

#if DEBUG_GET_STORE_REF
  printf("\n  Found numeric token");
#endif

      // Look back in the program to see what statement we are in. The original PRO-101
      // had 15 of everything, but we don't so we need more context to work out
      // how to display things.
      //      set_led_state(LED_END, 0);
      
      found_k       = 0;    
      found_goto    = 0;    
      found_prognum = 0;    
      found_if      = 0;
      found_store_ref  = 0;

#if DEBUG_GET_STORE_REF
      printf("\n  Looking backwards to find informing token");
#endif

      for(i = (s->current_step); (i >= 0) && !done; i--)
	{
#if DEBUG_GET_STORE_REF
	  printf("\n  %03d: %03X %s", i, s->program_step[s->current_program][i], name_of_token(s->program_step[s->current_program][i]));
#endif

	  switch( s->program_step[s->current_program][i] )
	    {
	    case TOK_COLON:
	      // End of search, we stay in current statement when looking
	      done = 1;
	      break;

	    case TOK_XY:
	    case TOK_ENT:
	    case TOK_ANS:
	    case TOK_EQ:
	      found_store_ref = 1;
	      done = 1;
	      break;
	      
	    case TOK_K:
	      // This is a constant
	      found_k = 1;
	      done = 1;
	      
	      //	      printf("\nK step:%d", i);
	      break;

	    case TOK_ST:
	    case TOK_GOTO:
	      // This is a goto label
	      found_goto = 1;
	      done = 1;
	      break;

	    case TOK_CALL:
	      // This is a program number
	      found_prognum = 1;
	      done = 1;
	      break;

	    case TOK_IF:
	      // This is in an IF, we don't parse the references and labels here, that is
	      // done during execution
	      found_store_ref = 1;
	      done = 1;
	      break;
	    }
	}
      
      if( found_k )
	{
#if DEBUG_GET_STORE_REF
  printf("\n  Found K");
#endif
	  set_led_state(LED_K, 1);
	  
	  // i points at the TOK_K, build the constant
	  str[0] = '\0';
	  char chr[3];
	  chr[1] = '\0';

	  done = 0;

#if DEBUG_GET_STORE_REF
  printf("\n  Looking forwards from K for end of numeric");
#endif
	  
	  for(i+=2; (i <= NUM_STEPS) && !done; i++)
	    {
	      TOKEN s_tok = (s->program_step[s->current_program][i]);
	      
#if DEBUG_GET_STORE_REF
	      printf("\n  %03X: %03X", i, s_tok);
#endif
	      switch(s_tok)
		{
		case TOK_0:
		case TOK_1:
		case TOK_2:
		case TOK_3:
		case TOK_4:
		case TOK_5:
		case TOK_6:
		case TOK_7:
		case TOK_8:
		case TOK_9:
		case TOK_DOT:
		  if(s_tok == TOK_DOT )
		    {
#if DEBUG_GET_STORE_REF
		      printf("\n  Set DP pos: %03X", strlen(str));
#endif
		      // Set DP point
		      dot_pos = strlen(str);
		    }
		  else
		    {
		      
		      chr[0] = s_tok - TOK_0 +'0';
		      strcat(str, chr);
#if DEBUG_GET_STORE_REF
		      printf("\n  Building constant:'%s'", str);
#endif
		    }
		  break;

		case TOK_K:
		  break;
		  
		default:
#if DEBUG_GET_STORE_REF
		  printf("\n  Done, move current step to %03X", i - 1);
#endif

		  // All done, move current step here
		  s->current_step = i-1;
		  done = 1;
		  break;
		}
	    }

	  // Display current step number and constant
	  if( dot_pos != -1 )
	    {
	      display_decimal_pos = (NUM_DIGITS-1)-strlen(str)+dot_pos;
	    }

	  // Display constant and step number of last step holding a constant token
	  sprintf(display, "%03d%11s", s->current_step, str);
	}
      else if (found_store_ref )
	{
	  // Found a token that uses a store reference
	  display_decimal_pos = -1;
	  set_led_state(LED_K, 0);
	  
	  // Must be a store reference
	  sprintf(display, "%03d         %2s", s->current_step, get_store_reference(s));	  
	}
      else if (found_if)
	{
#if DEBUG_GET_STORE_REF
	  printf("\n  Found IF");
#endif

	  display_decimal_pos = -1;
	  set_led_state(LED_K, 0);
	}
      else
	{
	  // No special keyword found, so decode as store reference
	  display_decimal_pos = -1;
	  set_led_state(LED_K, 0);
	  
	  // Must be a store reference
	  sprintf(display, "%03d         %2s", s->current_step, get_store_reference(s));	  
	}
      break;

    case TOK_END:
      //      set_led_state(LED_END, 1);
      display_decimal_pos = -1;
      set_led_state(LED_K, 0);
      sprintf(display, "%03d       %4s", s->current_step, pro101_progname_of_token(step_tok));
      break;
      
    default:
      //set_led_state(LED_END, 0);
      display_decimal_pos = -1;
      set_led_state(LED_K, 0);
      
      // Display current step number and the steps at that location
      sprintf(display, "%03d       %4s", s->current_step, pro101_progname_of_token(step_tok));
      break;
    }
  
}
#endif

void set_display_run(CALCULATOR_STATE *s)
{
}

void set_display_clear(CALCULATOR_STATE *s)
{
}

void set_display_write(CALCULATOR_STATE *s)
{
  //  display_decimal_pos = -1;

  // We do not have ENT or ANS on
  set_led_state(LED_ENT, 0);
  set_led_state(LED_ANS, 0);
  
  switch( mode_write_fsm.current_state )
    {
    case STATE_WRITE_INIT:
      set_display_write_zero(s);
      break;

    case STATE_WRITE_PRN:
    case STATE_WRITE_WAIT_COLON:
      set_display_write_prn(s);
      break;

    case STATE_WRITE_STEPS:
    default:
      if( s->program_step[s->current_program][s->current_step] == TOK_END )
	{
	  set_led_state(LED_END, 1);
	}
      else
	{
	  set_led_state(LED_END, 0);
	}
      
      set_display_write_steps(s);
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Set the display up for COMP mode
//
////////////////////////////////////////////////////////////////////////////////



void set_display_comp_numeric(CALCULATOR_STATE *s)
{
  n = s->x;
  dotp = 0;

  icd ten = 10.0;
  icd half = 0.5;
  icd neg_one = -1.0;

#if DEBUG_FN_TRACE
  printf("\n%s", __FUNCTION__);
#endif
  
  if( s->program_step[s->current_program][s->current_step] == TOK_END )
    {
      set_led_state(LED_K, 1);
    }
  else
    {
      set_led_state(LED_K, 0);
    }

  if( s->error )
    {
      memset(new_display, ' ', DISPLAY_WIDTH);
      new_display[DISPLAY_WIDTH-1] = 'E';
      strcpy(display, new_display);
      display_decimal_pos = DISPLAY_WIDTH-1;
      return;
    }

  if( s->miss )
    {
      set_led_state(LED_MISS, 1);
      display_decimal_pos = DISPLAY_WIDTH-1;
      return;
    }
  else
    {
      set_led_state(LED_MISS, 0);
    }

  if( s->exec_running )
    {

      memset(new_display, ' ', DISPLAY_WIDTH);
      new_display[DISPLAY_WIDTH-1] = '-';
      strcpy(display, new_display);
      display_decimal_pos = DISPLAY_WIDTH;
      return;
    }
  
  // Turn into string
  strncpy(new_display, n.AsString().c_str(), NUM_DIGITS);

#if DEBUG_DISPLAY_FMT
  printf("\n-----------------------------------");
  printf("\nnew_display:%s", new_display);
#endif
  
  // Find dot position, the dot will be displayed using the decimal point segment
  // on most displays, so won't actually be a character.
  
  for(dotp = 0; dotp<strlen(new_display); dotp++)
    {
      if( new_display[dotp] == '.' )
	{
	  break;
	}
    }

#if DEBUG_DISPLAY_FMT
  printf("\ndot_p:%d", dotp);
#endif

  // If we have digits after the decimal point then display
  // trailing zeros up to that point
  // Allow at least dot_mul_i trailing zeros
  
  int dot_mul_i = strlen((s->dot_mul).AsString().c_str());

#if DEBUG_DISPLAY_FMT
  printf("\ndot_mul_i:%d", dot_mul_i);
#endif

  // Lose trailing zeros
  for(int i=strlen(new_display)-1; i>dotp+(s->dot_mul_i)-1; i--)
    {
      if( new_display[i] == '0' )
	{
	  new_display[i] = '\0';
	}
      else
	{
	  break;
	}
    }

#if DEBUG_DISPLAY_FMT
  printf("\nnew_display:%s", new_display);
#endif
  
  // Remove any + from the front
  if( new_display[0] == '+' )
    {
      int j;
      
      for(j=1; j< strlen(new_display); j++)
	{
	  new_display[j-1] = new_display[j];
	}
      new_display[j-1] = '\0';
    }

#if DEBUG_DISPLAY_FMT
  printf("\nnew_display:%s", new_display);
#endif

  char temp_buf[1000];
  int new_dp_pos = display_decimal_pos;
  
  strcpy(temp_buf, new_display);
  sprintf(new_display, "%14s", temp_buf);


  // Find any decimal point and store position for
  // display
  for(int k=0; k<strlen(new_display); k++)
    {
      if( new_display[k] == '.' )
	{
	  new_dp_pos = k;
	  for(int m=k; m>0; m--)
	    {
	      new_display[m] = new_display[m-1];
	    }
	  new_display[0] = ' ';
	}
    }
  

  // Add in ENT or ANS fields and LED
  char store_ref[10];

  //
  // ENT mode
  //
  
  if( s->ent_mode_on )
    {
      set_led_state(LED_ENT, 1);
      
      switch(s->store_index_type)
	{
	case SIT_I:
	  new_display[0] = ' ';
	  new_display[1] = ' ';
	  break;

	case SIT_IM:
	  sprintf(store_ref, "%02d", s->store_index);
	  new_display[1] = store_ref[1];
	  new_display[0] = (store_ref[0]=='0'?' ':store_ref[0]);
	  break;

	default:
	  sprintf(store_ref, "%02d", s->store_index);
	  new_display[1] = store_ref[1];
	  new_display[0] = (store_ref[0]=='0'?' ':store_ref[0]);
	  break;
	}
    }
  else
    {
      set_led_state(LED_ENT, 0);
    }

  //
  // ANS mode
  //
  
  if( s->ans_mode_on )
    {
      set_led_state(LED_ANS, 1);

      switch(s->store_index_type)
	{
	case SIT_I:
	  new_display[0] = ' ';
	  new_display[1] = ' ';
	  break;

	default:
	  sprintf(store_ref, "%02d", s->store_index);
	  new_display[1] = store_ref[1];
	  new_display[0] = (store_ref[0]=='0'?' ':store_ref[0]);
	  break;
	}
    }
  else
    {
      set_led_state(LED_ANS, 0);
    }
  
  // If we have ENT or ANS mode on then add the LED and store
  // index to the display
  
#if 0
  char buffer[400];
  
  sprintf(buffer, "   %s", s->x.AsString().c_str());
  printf("\nX:%s", buffer);
  printf("\nDISP:'%s'", display);
  printf("\nNEW: '%s'", new_display);
#endif

  //
  // Only do an update if something has changed
  //
  
  if( strcmp(display, new_display) != 0 )
    {
      strcpy(display, new_display);
      display_decimal_pos = new_dp_pos;
    }
}

void set_display_comp(CALCULATOR_STATE *s)
{
  set_led_state(LED_END, 0);
  set_led_state(LED_K,   0);
  
  switch( mode_comp_fsm.current_state )
    {
    case STATE_COMP_PRN:
    case STATE_COMP_WAIT_COLON:
      set_display_write_prn(s);
      break;

    default:
      set_display_comp_numeric(s);
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Polls the hardware inputs:
//
//  Keybaord
//  Slide switches
//
// Also drives the following:
//
//  TOK_NO_TOK if no input, which drives programs
//  Test sequences
//
////////////////////////////////////////////////////////////////////////////////

void process_hw_inputs(void)
{
  int key = poll_keyboard(&state);
  queue_token(key);
  
  // Poll slide switches
  if( (key = poll_mode_switch(&state)) != TOK_NONE )
    {
      queue_token(key);
    }
  
  if( (key = poll_cut_switch(&state)) != TOK_NONE )
    {
      queue_token(key);
    }
  
  if( (key = poll_places_switch(&state)) != TOK_NONE )
    {
      queue_token(key);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////////////////////////////////



int main()
{
  int gpio_states;
  
  ////////////////////////////////////////////////////////////////////////////////
  //
  // Overclock as needed
  //
  ////////////////////////////////////////////////////////////////////////////////
  
  //#define OVERCLOCK 135000
  //#define OVERCLOCK 200000
#define OVERCLOCK 270000
  //#define OVERCLOCK 360000
  
#if OVERCLOCK > 270000
  /* Above this speed needs increased voltage */
  vreg_set_voltage(VREG_VOLTAGE_1_20);
  sleep_ms(1000);
#endif
  
  /* Overclock */
  set_sys_clock_khz( OVERCLOCK, 1 );

  stdio_init_all();
  print_gpio_functions();
  
  // Set up filament GPIOs. These an destroy the driving transistors if not done
  // correctly

  // Drive to safe states
  filament_off();

  set_gpio_output(P_VFI_D_H);
  set_gpio_output(P_VFI_D_L);
  set_gpio_output(P_VF_D_H);
  set_gpio_output(P_VF_D_L);

  set_gpio_output(P_MAX_LOAD);
  set_gpio_output(P_MAX_BLANK);
  set_gpio_output(P_MAX_DIN);
  set_gpio_output(P_MAX_CLK);

  set_gpio_output(P_CATHODE1);
  set_gpio_output(P_CATHODE2);
  
  // never blank
  gpio_put(P_MAX_BLANK, 0);
  
  set_gpio_input(P_SW0);
  set_gpio_input(P_SW1);
  set_gpio_input(P_SW2);
  set_gpio_input(P_SW3);
  set_gpio_input(P_SW4);
  gpio_set_pulls(P_SW0, 0, 1);
  gpio_set_pulls(P_SW1, 0, 1);
  gpio_set_pulls(P_SW2, 0, 1);
  gpio_set_pulls(P_SW3, 0, 1);
  gpio_set_pulls(P_SW4, 0, 1);

  // Clear all programs
  clear_all_programs(&state);
  
  sprintf(display, "   PRo-101    ");
    
  // Core 1 captures data and sends transactions to core 0 which
  // sends them over USB
  multicore_launch_core1(core1_main);

  sleep_ms(2000);

  printf("\n");
  printf("\n                                  ***********************");
  printf("\n                                  * PRO-101 Recreation  *");
  printf("\n                                  ***********************");
  printf("\n");

  printf("\nSize of Icd: %d", sizeof(icd));
    
  // Load slot 1 at startup
  address = 1;
  parameter = 1;
  load_ram(parameter, DO_MP_BOTH);
  
  // Sit in loop sending transatins to USB

  // drive a test pattern

  int da = 0;
  int db = 0;

  // Drive KBD lines
  set_serial_field(SF_KBD_DRV, 0x10);
  set_serial_field(SF_LED_ANODES, 0x01);
  set_serial_field(SF_SWITCH_DRV, 0x01);

  int tick = 0;
  int cath = 1;
  
#if 0
  for(int j=0; j<NUM_DIGITS; j++)
    {
      display[j] = j;
    }
#endif

  char buffer[400];
  
  init_state(&state);

  while(1)
    {
      //tick++;

      //      set_serial_field(SF_LED_ANODES, tick/200);
      
      if( tick > 1000 )
	{
	  printf("\nTick   KBD:%02X  SW:%02X", kbd_sense(), sw_sense());
	  tick = 0;
	}

      process_hw_inputs();
      
#if 0
      int k = keycode();
      if( k != 0 )
	{
	  sprintf(display, "   %04X %03X   ", k, keycode_to_token(k) );
	  printf("\n   %04X %03X", k, keycode_to_token(k) );
	}
#endif

      // All tokens pass through the top_fsm, they are then passed on to the
      // calculator mode FSMs if they are to do with calculations (keystrokes and
      // MODE switch settings.
      // The formatting switch settings (CUT and PLACES) set global variables
      // and are processed in the top_fsm

      drive_fsms();
      serial_loop();
    }
}
