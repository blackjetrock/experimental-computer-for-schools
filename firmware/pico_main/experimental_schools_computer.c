//////////////////////////////////////////////////////////////////////////////
//
// Experimental Schools Computer Simulator
//
////////////////////////////////////////////////////////////////////////////////
//
// Simulates the IBM experimental schools computer
//
// Runs on RP Pico
// 0.96" OLED display
// Tactile switch keys
//
// USB CLI
// Wifi hot spot with screen display 
//
////////////////////////////////////////////////////////////////////////////////

#include "switches.h"
#include "version.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/flash.h"
#include "hardware/clocks.h"
#include "hardware/structs/bus_ctrl.h"

#define DEBUG_STOP_LOOP while(1) {}

// Some logic to analyse:

#include "hardware/structs/pwm.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

#include "oled.h"
#include "sdcard.h"

#include "fsms.h"
#include "esc_fsms.h"

#include "esc.h"
#include "esc_desktop_display.h"
#include "extracode.h"

////////////////////////////////////////////////////////////////////////////////

int status[7];
int kbd_scan_code = 0;

#define CHAR_MS_DELAY 20

//------------------------------------------------------------------------------
//
// Decode key tokens
//

KEY_INFO key_info[] =
  {
    {"KI RESET   ", TOK_KEY_KI_RESET},
    {"A          ", TOK_KEY_A},
    {"B          ", TOK_KEY_B},
    {"C          ", TOK_KEY_C},
    {"0          ", TOK_KEY_0},
    {"1          ", TOK_KEY_1},
    {"2          ", TOK_KEY_2},
    {"3          ", TOK_KEY_3},
    {"4          ", TOK_KEY_4},
    {"5          ", TOK_KEY_5},
    {"6          ", TOK_KEY_6},
    {"7          ", TOK_KEY_7},
    {"8          ", TOK_KEY_8},
    {"9          ", TOK_KEY_9},
    {"RUN        ", TOK_KEY_RUN},
    {"STOP       ", TOK_KEY_STOP},
    {"RELOAD     ", TOK_KEY_RELOAD},
    {"CHECK      ", TOK_KEY_CHECK},
    {"DUMP       ", TOK_KEY_DUMP},
    {"LOAD IAR   ", TOK_KEY_LOAD_IAR},
    {"LOAD ADDR  ", TOK_KEY_LOAD_ADDR},
    {"LOAD STORE ", TOK_KEY_LOAD_STORE},
    {"DECR ADDR  ", TOK_KEY_DECR_ADDR},
    {"INCR ADDR  ", TOK_KEY_INCR_ADDR},
    {"NORM RESET ", TOK_KEY_NORMAL_RESET},
    {"CLEAR      ", TOK_KEY_CLEAR},
    {"DOT        ", TOK_KEY_DOT},
    {"MINUS      ", TOK_KEY_MINUS},
  };

#define NUM_KEY_INFO (sizeof(key_info)/sizeof(KEY_INFO))

char *key_name(int tok)
{
  for(int i=0; i<NUM_KEY_INFO; i++)
    {
      if( key_info[i].tok == tok )
        {
          return(key_info[i].name);
        }
    }
  return("????");
}

////////////////////////////////////////////////////////////////////////////////
//
// Tests
//

#define MAX_TEST_FAIL_BUFFER 160

// Initialise registers and store
typedef enum _INIT_CODE
  {
    IC_SET_REG_N = 10,
    IC_SET_REG_V,
    IC_SET_STORE_A,
    IC_SET_STORE_V,
    IC_END,
  } INIT_CODE;

typedef struct _INIT_INFO
{
  INIT_CODE code;
  uint64_t   n;
} INIT_INFO;

// The following enum is also used to identify registers other than R0..9
// when testing results. E.g. IAR

typedef enum _TEST_CODE
  {
    TC_REG_N = 20,
    TC_REG_IAR,
    TC_REG_LINK,
    TC_REG_ADDR,
    TC_CL,
    TC_REG_KI,
    TC_STORE_N,
    TC_MUST_BE,
    TC_MUST_BE_STOPPED,
    TC_MUST_BE_NOT_STOPPED,
    TC_END_SECTION,    // End of sub section results. Check up to here (or a TC_END) by TOK_TEST_CHECK_RES token
    // Do not put a spurious TC_SECTION_END before a TC_END
    TC_END,
  } TEST_CODE;

char *tc_names[] =
  {
    "TC_REG_N",
    "TC_REG_IAR",
    "TC_REG_LINK",
    "TC_REG_ADDR",
    "TC_CL",
    "TC_REG_KI",
    "TC_STORE_N",
    "TC_MUST_BE",
    "TC_MUST_BE_STOPPED",
    "TC_MUST_BE_NOT_STOPPED",
    "TC_END_SECTION",
    "TC_END",
  };

#define MAX_TC_REG_BUF 20

#define TEST_LOAD_STORE_LEN 100

typedef struct _TEST_LOAD_STORE
{
  int data[TEST_LOAD_STORE_LEN];
} TEST_LOAD_STORE;

typedef struct _TEST_INFO
{
  TEST_CODE       code;
  uint64_t        n;
} TEST_INFO;

typedef struct _ESC_TEST_INFO
{
  char            *desc;
  INIT_INFO       *init_codes;
  TOKEN           *seq;
  TEST_INFO       *result_codes;
  int              passed;
  TEST_LOAD_STORE *store_data;
  char             fail_text[MAX_TEST_FAIL_BUFFER];
} ESC_TEST_INFO;

#define NUM_TESTS (sizeof(tests)/sizeof(ESC_TEST_INFO))

////////////////////////////////////////////////////////////////////////////////

#define TEXT_PARAMETER_LEN 40

////////////////////////////////////////////////////////////////////////////////
//
//
#define MAX_ERROR_BUFFER 200

void error_msg(char *fmt, ...)
{
  char line[MAX_ERROR_BUFFER+1];
  
  va_list args;
  va_start(args, fmt);

  vsnprintf(line, MAX_ERROR_BUFFER, fmt, args);
  va_end(args);

#if ERRORS_ON
  printf("\n*** %s ***\n", line);
#endif
  
}

void warning_msg(char *fmt, ...)
{
  char line[MAX_ERROR_BUFFER+1];
  
  va_list args;
  va_start(args, fmt);

  vsnprintf(line, MAX_ERROR_BUFFER, fmt, args);
  va_end(args);

  printf("\n*** WARNING: %s ***\n", line);
}



char tc_reg_buffer[MAX_TC_REG_BUF+1];

////////////////////////////////////////////////////////////////////////////////

void serial_help(void);
void prompt(void);
void prompt_breakpoint(void);

int write_state_to_file(ESC_STATE *es, char *fn);
int read_file_into_state(char *fn, ESC_STATE *es);
int cat_file(char *fn);

int wfn_iar_address(ESC_STATE *es, void *fi, char *line);
int wfn_iar_a_flag(ESC_STATE *es, void *fi, char *line);
int wfn_breakpoint_address(ESC_STATE *es, void *fi, char *line);
int wfn_breakpoint_a_flag(ESC_STATE *es, void *fi, char *line);
int wfn_kb_register(ESC_STATE *es, void *fi, char *line);
#if 0
int wfn_address_register(ESC_STATE *es, void *fi, char *line);
#endif
int wfn_link_register(ESC_STATE *es, void *fi, char *line);
int wfn_instruction_register(ESC_STATE *es, void *fi, char *line);
int wfn_store_data(ESC_STATE *es, void *fi, char *line);
int wfn_store(ESC_STATE *es, void *fi, char *line);
int wfn_suppressed_display(ESC_STATE *es, void *fi, char *line);
int wfn_step_extracode(ESC_STATE *es, void *fi, char *line);

void update_computer_display(ESC_STATE *es);
void register_assign_register_uint64(ESC_STATE *s, int dest, uint64_t n);
REGISTER_DOUBLE_WORD read_any_size_register(ESC_STATE *s, int n);
REGISTER_DOUBLE_WORD read_any_size_register_absolute(ESC_STATE *s, int n);

ESC_TEST_INFO tests[];
void display_on_line(ESC_STATE *s, int display, int line_no, char *fmt, ...);
char *display_register_and_contents(ESC_STATE *s, int regno);
char *display_store_and_contents(ESC_STATE *s, SINGLE_WORD address);
char *display_store_word(SINGLE_WORD w);
int display_two_any_size_register_on_line(ESC_STATE *s, int display, int lineno, int regno1, int regno2, int contents);

void stage_a_display(ESC_STATE *s, int display, int a);
void stage_b_display(ESC_STATE *s, int display, int a);
void stage_c_display(ESC_STATE *s, int display, int a);

#define NO_CONTENTS 0
#define CONTENTS    1

//------------------------------------------------------------------------------

int display_any_size_register_on_line(ESC_STATE *s, int display, int line_no, int regno, int contents);

void clear_lines_3_to_6(ESC_STATE *s, int display);
void clear_line(ESC_STATE *s, int display, int n);
void clear_display(ESC_STATE *s, int display);

//------------------------------------------------------------------------------

volatile uint32_t touch_key_raw = 0;
int get_qt_key_code(void);
////////////////////////////////////////////////////////////////////////////////
//
// Register access
//

// Register identiiers.
// 0..9 are R0..R9

typedef enum _REGN
  {
    REGN_0 = 0,
    REGN_1 = 1,
    REGN_2 = 2,
    REGN_3 = 3,
    REGN_4 = 4,
    REGN_5 = 5,
    REGN_6 = 6,
    REGN_7 = 7,
    REGN_8 = 8,
    REGN_9 = 9,
    REGN_IAR,
    REGN_AUX_IAR,
  } REGN;

void write_register(ESC_STATE *s, REGN n, REGISTER_DOUBLE_WORD data)
{
  switch(n)
    {
    case REGN_0:
    case REGN_1:
    case REGN_2:
    case REGN_3:
    case REGN_4:
    case REGN_5:
    case REGN_6:
    case REGN_7:
      s->store[200+n] = (REGISTER_SINGLE_WORD)data;
      break;

    case REGN_8:
    case REGN_9:
      // R8 and R9 are in two locations each. Lower address is lower half of register
      s->store[200+n] = (REGISTER_SINGLE_WORD) (data & 0xFFFFFFFF);
      s->store[210+n] = (REGISTER_SINGLE_WORD) ((data & 0xFFFFFFFF00000000) >> 32);
      break; 
    }
}

// Read any register
// For SW, cast result.

REGISTER_DOUBLE_WORD read_register(ESC_STATE *s, REGN n)
{
  switch(n)
    {
    case REGN_0:
    case REGN_1:
    case REGN_2:
    case REGN_3:
    case REGN_4:
    case REGN_5:
    case REGN_6:
    case REGN_7:
      return (REGISTER_DOUBLE_WORD)(s->store[200+n]);
      break;

    case REGN_8:
    case REGN_9:
      // R8 and R9 are in two locations each. Lower address is lower half of register
      return ( ((REGISTER_DOUBLE_WORD)(s->store[200+n])) | (((REGISTER_DOUBLE_WORD)(s->store[210+n])) << 32));
      break; 
    }
}

////////////////////////////////////////////////////////////////////////////////

char *tc_reg_name(TEST_CODE tc)
{
  if( tc < TC_REG_N )
    {
      snprintf(tc_reg_buffer, MAX_TC_REG_BUF, "%d", tc);
      return(tc_reg_buffer);
    }
  
  return(tc_names[tc-TC_REG_N]);
}

// Load the store before running the test. Words are terminated by a -1


//------------------------------------------------------------------------------
//
// Test failure information
//

char test_fail_buffer[MAX_TEST_FAIL_BUFFER];

void clear_test_fail_buffer(void)
{
  test_fail_buffer[0] = '\0';
}


// Add more information to the test fail buffer
void test_fail_info(char *fmt, ...)
{
  char line[MAX_TEST_FAIL_BUFFER+1];
  
  va_list args;
  va_start(args, fmt);

  vsnprintf(line, MAX_TEST_FAIL_BUFFER, fmt, args);
  va_end(args);

  printf("\n-----------%s---", line);
  
  // How much space is available for more text?
  // Include a space delimiter and the newline at the end.
  int n = MAX_TEST_FAIL_BUFFER-strlen(test_fail_buffer)-1-1;

  printf("\nn=%d, strlen(tfb)=%d strlen(line)=%d", n, strlen(test_fail_buffer), strlen(line));
  
  if( n < 0 )
    {
      test_fail_buffer[0] = '!';
      printf("\n***%s***%d**", test_fail_buffer, n);
      return;
    }

  strcat(test_fail_buffer, " ");
  strncat(test_fail_buffer, line, n);

  printf("\n+++++++++++%s+++", test_fail_buffer);
}

////////////////////////////////////////////////////////////////////////////////
//

void load_store_from_test(ESC_STATE *s, int test_number)
{
  // Load the store
  
  for(int i=0; (i<TEST_LOAD_STORE_LEN) && (tests[test_number].store_data->data[i] != -1); i++)
    {
      s->store[i] = tests[test_number].store_data->data[i];
    }
}

////////////////////////////////////////////////////////////////////////////////

int keypress = 0;
int parameter = 0;
int parameter_a_flag = 0;

int auto_increment_parameter = 0;
int auto_increment_address   = 0;
char text_parameter[TEXT_PARAMETER_LEN+1] = "";
int address               = 0;
int test_number           = 0;
int test_run_single_test  = 0;
int test_running          = 0;
int test_waiting_for_stop = 0;
int test_done_init        = 0;
int test_step             = 0;

////////////////////////////////////////////////////////////////////////////////
//
// GPIOs
//

void set_gpio_output(const int gpio)
{
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_OUT);
}

void set_gpio_input(const int gpio)
{
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_IN);
}

#if ESC_TYPE_DESKTOP

// We set GPIOs and the UART

const int PIN_QT_DRDY =  26;
const int PIN_QT_WS   =  17;

void set_kbd_gpios(void)
{
  set_gpio_input(PIN_QT_DRDY);

  // Set WAKE/SYNC high
  set_gpio_output(PIN_QT_WS);
  gpio_put(PIN_QT_WS, 1);

  // Set the GPIO pin mux to the UART - 12 is TX, 13 is RX
  // Takes over GPIO directions
  gpio_set_function(12, GPIO_FUNC_UART);
  gpio_set_function(13, GPIO_FUNC_UART);

  printf("\nQT Baud rate set to %d", uart_init (uart0, 9600));
  uart_set_format (uart0, 8, 1, UART_PARITY_NONE);

}


#endif

#if ESC_TYPE_SMALL

// KBD
const int PIN_KBD_DRV0 =  0;
const int PIN_KBD_DRV1 =  1;
const int PIN_KBD_DRV2 =  2;
const int PIN_KBD_DRV3 =  3;

const int PIN_KBD_SENS0 =   4;
const int PIN_KBD_SENS1 =   5;
const int PIN_KBD_SENS2 =   6;
const int PIN_KBD_SENS3 =   7;
const int PIN_KBD_SENS4 =   8;
const int PIN_KBD_SENS5 =   9;
const int PIN_KBD_SENS6 =  10;



//------------------------------------------------------------------------------
//
// Set up GPIOs for the keyboard

void kbd_gpio_drv(const int gpio)
{
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_OUT);
}

void kbd_gpio_sens(const int gpio)
{
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_IN);
  gpio_set_pulls(gpio, 0, 1);
}

const int kbd_drv_gpios[] =
  {
    PIN_KBD_DRV0,   
    PIN_KBD_DRV1,   
    PIN_KBD_DRV2,   
    PIN_KBD_DRV3,   
  };

#define NUM_KBD_DRV (sizeof(kbd_drv_gpios)/sizeof(const int))

const int kbd_sens_gpios[] =
  {
    PIN_KBD_SENS0,   
    PIN_KBD_SENS1,   
    PIN_KBD_SENS2,   
    PIN_KBD_SENS3,   
    PIN_KBD_SENS4,   
    PIN_KBD_SENS5,   
    PIN_KBD_SENS6,
  };

#define NUM_KBD_SENS (sizeof(kbd_sens_gpios)/sizeof(const int))

void set_kbd_gpios(void)
{
  kbd_gpio_drv(PIN_KBD_DRV0);
  kbd_gpio_drv(PIN_KBD_DRV1);
  kbd_gpio_drv(PIN_KBD_DRV2);
  kbd_gpio_drv(PIN_KBD_DRV3);

  kbd_gpio_sens(PIN_KBD_SENS0);
  kbd_gpio_sens(PIN_KBD_SENS1);
  kbd_gpio_sens(PIN_KBD_SENS2);
  kbd_gpio_sens(PIN_KBD_SENS3);
  kbd_gpio_sens(PIN_KBD_SENS4);
  kbd_gpio_sens(PIN_KBD_SENS5);
  kbd_gpio_sens(PIN_KBD_SENS6);
    
}

int kbd_read_sense(void)
{
  int res = 0;
  
  for(int i=0; i<NUM_KBD_SENS; i++)
    {
      if( gpio_get(kbd_sens_gpios[i]) )
	{
	  res |= 1<<i;
	}
    }
  return(res);
}

// Scan the keyboard.
// Drive each drive line and scan the sense lines, then feed the keycodes seen
// into a simple state machine for debounce.
// Debounce counts up to  press and dow to a release
// Key codes injected into FSM on press

#define NUM_KBD_T_SLOTS  10

enum
  {
    KBD_DB_STATE_IDLE = 1,
    KBD_DB_STATE_PRESSING,
    KBD_DB_STATE_RELEASING,
  };

int kbd_t_slot = 0;
int kbd_drv_i = 0;
int kbd_sense = 0;
int kbd_output_scan_code = 0;
int kbd_last_output_scan_code = 0;

int kbd_last_scan_code = 0;
int kbd_db_state = KBD_DB_STATE_IDLE;

#define KBD_DB_COUNT_MAX 100
int kbd_db_count = 0;

void cli_kbd_dump(void)
{
  printf("\nKeyboard State\n");

  printf("\nScan code:%03X Last scan code:%03X", kbd_scan_code, kbd_last_scan_code);
  printf("\nOutput san code:%03X", kbd_output_scan_code);

  printf("\nI:%d  T Slot:%d", kbd_drv_i, kbd_t_slot);
  printf("\nSense:%02X", kbd_sense);
}

// Drive required sense line
void kbd_drive(void)
{
  int i;

  // Turn drive lines off
  for(i=0; i<NUM_KBD_DRV; i++)
    {
      gpio_put(kbd_drv_gpios[i], 0);
    }
  
  // Move to next drive line
  kbd_drv_i = (kbd_drv_i + 1 ) % NUM_KBD_DRV;
  
  // Drive required line
  gpio_put(kbd_drv_gpios[kbd_drv_i], 1);
}

// Send a keypress to the FSM

struct _KEYMAP
{
  int scan_code;
  TOKEN token;
}
  key_map[] =
  {
    {0x1C0, TOK_KEY_CLEAR},
    {0x101, TOK_KEY_MINUS},
    {0x102, TOK_KEY_DOT},
    {0x090, TOK_KEY_0},
    {0x104, TOK_KEY_1},
    {0x108, TOK_KEY_2},
    {0x110, TOK_KEY_3},
    {0x120, TOK_KEY_4},
    {0x140, TOK_KEY_5},
    {0x081, TOK_KEY_6},
    {0x082, TOK_KEY_7},
    {0x084, TOK_KEY_8},
    {0x088, TOK_KEY_9},
    {0x010, TOK_KEY_DUMP},
    {0x020, TOK_KEY_CHECK},
    {0x040, TOK_KEY_RELOAD},

    {0x1A0, TOK_KEY_NORMAL_RESET},
    {0x002, TOK_KEY_KI_RESET},
    {0x181, TOK_KEY_LOAD_IAR},
    {0x182, TOK_KEY_LOAD_ADDR},
    {0x190, TOK_KEY_INCR_ADDR},
    {0x188, TOK_KEY_DECR_ADDR},
    {0x184, TOK_KEY_LOAD_STORE},
    {0x0A0, TOK_KEY_A},
    {0x0C0, TOK_KEY_B},
    {0x001, TOK_KEY_C},
    {0x004, TOK_KEY_RUN},
    {0x008, TOK_KEY_STOP},
  };

#define NUM_KEY_MAPS (sizeof(key_map)/sizeof( struct _KEYMAP))
  
void kbd_queue_key(int k)
{
  for(int i=0; i<NUM_KEY_MAPS; i++)
    {
      if( k == key_map[i].scan_code )
	{
	  queue_token(key_map[i].token);
#if DEBUG_KEYS
          printf("\nKey: %s", key_name(key_map[i].token)); 
#endif
	  return;
	}
    }
}

#endif

//------------------------------------------------------------------------------

void cli_version(void)
{
  printf("\nVersion:1.1.%d Compile time:%s %s\n", VERSION_INC, __DATE__, __TIME__);
}

//------------------------------------------------------------------------------

void cli_display_test(void)
{
  escdd_main();
}

int test_loop_count = 0;
#define TEST_EVERY_N_LOOPS  10

int test_res_i = 0;

int last_token = TOK_NONE;

// read scan code
void kbd_read(ESC_STATE *s)
{
#if 0

  // Hopefully transient error during extracode implementation
  if( s->store[168] == 0 )
    {
      printf("\n**** 168 == 0 *******\n\n");
      while(0)
        {
        }
    }
  
#endif
  

#if ESC_TYPE_SMALL  
  // If the drive index is zero then reset the scan code
  if( kbd_drv_i == 0 )
    {
      kbd_scan_code = 0;
    }
  
  kbd_sense = kbd_read_sense();

  // Only build scan code if a key is pressed
  if( kbd_sense != 0 )
    {
      // Save last scan code
      kbd_last_scan_code = kbd_scan_code;
      
      // Build scan code
      kbd_scan_code = (kbd_drv_i << 7) | kbd_sense;
    }
#endif

#if ESC_TYPE_DESKTOP

  
  // Get the keycode of the key currently pressed if there is one.
  kbd_scan_code = get_qt_key_code();

  if( kbd_scan_code == TOK_NONE )
    {
      last_token = TOK_NONE;
    }
  
  if( kbd_scan_code != TOK_NONE )
    {
      // Key pressed, new key?
      if( kbd_scan_code != last_token )
	{
	  queue_token(kbd_scan_code);
	}
      
      last_token = kbd_scan_code;
    }
#endif
  else
    {
      // No key pressed, check for a running test after N idle loops
      // We allow the FSMs to run for a while between keystrokes
      // If we are waiting for a stop condition then don't process the test until we see a stop
      
      if( test_waiting_for_stop || ((++test_loop_count) <= TEST_EVERY_N_LOOPS) )
	{
	  // Wait another loop before running test
	}
      else
	{
	  test_loop_count = 0;
	  
	  if( test_running )
	    {
	      // Initialise?
	      if( test_done_init )
		{
		  // Init done, queue token
		  TOKEN t = tests[test_number].seq[test_step];

		  int rn = -1;
		  int test_type = TC_END;		  
		  int done = 0;

#if DUMP_STATE_STAGE_C
                  cli_dump_state();
#endif

#if DEBUG_TEST_SEQ
		  printf("\n  Token:%d", t);
#endif
		  switch(t)
		    {
		    case TOK_TEST_WAIT_FOR_STOP:

		      // Wait and run in one operation so the program doesn't run and end before we are waiting for it
		      // to stop
		      
		      test_waiting_for_stop = 1;
		      s->run = 1;

		      // Skip past this code
		      test_step++;
#if DEBUG_TEST_SEQ
		      printf("\n  TOK_TEST_WAIT_FOR_STOP  test_waiting_for_stop:%d", test_waiting_for_stop);
#endif
		      break;
		      
		    case TOK_NONE:
		    case TOK_TEST_CHECK_RES:
		      
		      // Test sequence finished
		      // Test results

		      // This test is not waiting for a stop any more
		      test_waiting_for_stop = 0;

#if DEBUG_TEST_SEQ                      
		      printf("\n  Checking results  test_waiting_for_stop:%d", test_waiting_for_stop);
				      
		      printf("\nChecking results for test: %s", tests[test_number].desc);
#endif
		      // Which type of test do we do?
		      test_type = TC_END;
			  
		      while(!done)
			{
#if DEBUG_TEST_SEQ
			  printf("\n  Test_res_i:%d TC code:%d", test_res_i, t);
#endif
			  switch(tests[test_number].result_codes[test_res_i].code)
			    {
			    case TC_STORE_N:
			      test_type = TC_STORE_N;
			      rn = tests[test_number].result_codes[test_res_i].n;
#if DEBUG_VERBOSE_TEST
			      printf("\nTesting store[%04X]", rn);
#endif
			      break;

			    case TC_REG_N:
			      test_type = TC_REG_N;
			      rn = tests[test_number].result_codes[test_res_i].n;
#if DEBUG_VERBOSE_TEST
			      printf("\nTesting R[%d]", rn);
#endif
			      break;

			    case TC_REG_IAR:
                            case TC_REG_LINK:
			    case TC_REG_KI:
			    case TC_REG_ADDR:
			    case TC_CL:

			      rn = tests[test_number].result_codes[test_res_i].code;
#if DEBUG_TEST_SEQ
			      printf("\n  TC_REG_xxx:%d rn=%d", t, rn);
#endif
			      break;

			    case TC_MUST_BE_STOPPED:
#if DEBUG_TEST_SEQ
                              printf("\nTesting for stopped. Stop = %d Run=%d", s->stop, s->run);
#endif
			      if( s->stop )
				{
				  // Stop is set, all ok
				}
			      else
				{
				  test_fail_info("Stop = 0");
				  tests[test_number].passed = 0;
				}
			      break;

			    case TC_MUST_BE_NOT_STOPPED:
#if DEBUG_TEST_SEQ
                              printf("\nTesting for not stopped. Stop = %d Run=%d", s->stop, s->run);
#endif
			      if( !s->stop )
				{
				  // Stop is not set, all ok
				}
			      else
				{
				  test_fail_info("Stop = 1");
				  tests[test_number].passed = 0;
				}
			      break;
			      
			    case TC_MUST_BE:
#if DEBUG_TEST_SEQ
			      printf("\nTest type: %s", tc_reg_name(test_type));
#endif
			      switch(test_type)
				{
				  // Test against the store
				case TC_STORE_N:
				  if( load_from_store(s, rn) == tests[test_number].result_codes[test_res_i].n )
				    {
#if DEBUG_TEST_SEQ
				      printf("\nStore[%02X] (%08X) == %016llX", rn, load_from_store(s, rn), tests[test_number].result_codes[test_res_i].n);
#endif
				    }
				  else
				    {
#if DEBUG_TEST_SEQ
				      // Not OK
				      printf("\nStore[%02X] (%08X) <> %016llX", rn, load_from_store(s, rn), tests[test_number].result_codes[test_res_i].n);
#endif
				      test_fail_info("Store[%02X] (%08X) <> %016llX", rn, load_from_store(s, rn), tests[test_number].result_codes[test_res_i].n);
				      tests[test_number].passed = 0;
				    }
				  break;

				  // Test against a register
				case TC_END:
				case TC_REG_N:
				  if( rn != -1 )
				    {
				      switch(rn)
					{
					case TC_REG_N:
					case TC_STORE_N:
					case TC_MUST_BE:
					case TC_END:
					  printf("\nTest code %d used as register index compare");
					  break;
					  
					case TC_REG_IAR:
                                        case TC_REG_LINK:
					case TC_REG_KI:
					case TC_REG_ADDR:
					case TC_CL:
					default:
					  if( read_any_size_register(s, rn) == tests[test_number].result_codes[test_res_i].n )
					    {
#if DEBUG_TEST_SEQ
					      // All OK
					      printf("\nR[%d] == %08xd, OK", rn, tests[test_number].result_codes[test_res_i].n);
#endif
					    }
					  else
					    {
#if DEBUG_TEST_SEQ
					      // Not OK
					      printf("\nR[%d] <> %016llx", rn, tests[test_number].result_codes[test_res_i].n);
#endif
					      test_fail_info("R[%s] (%016llx) <> %016llx", tc_reg_name(rn), read_any_size_register(s, rn), tests[test_number].result_codes[test_res_i].n);
					      
					      tests[test_number].passed = 0;
					    }
					  break;
					}
				      break;
				    }
				  break;
				}
			      test_type = TC_END;
			      break;
			      
			    case TC_END_SECTION:
			      // All ok, we aren't done yet, keep going
#if DEBUG_TEST_SEQ
			      printf("\nTC_END_SECTION");

#endif
			      // Skip the end section code
			      test_step++;
			      done = 1;			      
			      break;
			      
			    case TC_END:
#if DEBUG_TEST_SEQ
			      printf("\nResult check done\n");
#endif
			      done = 1;
			      test_running = 0;
			      test_waiting_for_stop = 0;
			      
			      // Reset result pointer ready for next test
			      test_res_i = 0;
			      break;
			  
			    default:
#if DEBUG_TEST_SEQ
			      printf("\nUnknown test TC code (test %d, i=%d, code=%d)", test_number, test_res_i, tests[test_number].result_codes[test_res_i].code);
#endif
			      test_running = 0;
			      done = 1;
			      break;
			    }
		      
			  test_res_i++;
			}

		      // Test has run, see if we should run another, or stop
		      // If we have checked results due to a section end then continue running the test
		      if( test_running )
			{
#if DEBUG_TEST_SEQ
			  printf("\nTC_SECTION_END so continuing...");
#endif
			}
		      else
			{
			  if( !test_run_single_test )
			    {
			      // Copy fail text, if test failed
			      if( !tests[test_number].passed )
				{
				  strcpy(tests[test_number].fail_text, test_fail_buffer);
				}

#if DEBUG_VERBOSE_TEST
			      printf("\nMoving to next test...");
#endif	      
			      // Move to next test if there is one
			      test_number++;
			      clear_test_fail_buffer();
			      
			      if( strcmp( tests[test_number].desc, "--END--") != 0 )
				{
				  
				  test_run_single_test = 0;
				  test_running   = 1;
				  test_done_init = 0;
				}
			      else
				{
				  // End of tests, stop
				  test_running = 0;
				  
				  printf("\nTest END marker found, stopping tests...");
				}
			    }
			  else
			    {
			      // Copy fail text, if test failed
			      if( !tests[test_number].passed )
				{
				  strcpy(tests[test_number].fail_text, test_fail_buffer);
				}
			    }
			} 
		      break;
		      
		    default:
#if DEBUG_TEST_SEQ
		      printf("\n Queuing token %d\n", t);
#endif
		      queue_token(t);
		      test_step++;
		      break;
		    }
		}
	      else
		{
		  // Init not done, do it
		  int i = 0;
		  int rn = 0;
		  int done = 0;

#if DEBUG_VERBOSE_TEST
		  printf("\n================================================");
		  printf("\nInitialising test: %s", tests[test_number].desc);
		  printf("\n================================================");
#endif
		  test_res_i = 0;
		  
		  load_store_from_test(s, test_number);
		  
		  while(!done)
		    {
		      switch(tests[test_number].init_codes[i].code)
			{
			case IC_SET_REG_N:
			  rn = tests[test_number].init_codes[i].n;
			  break;

			case IC_SET_REG_V:
			  register_assign_register_uint64(s, rn, tests[test_number].init_codes[i].n );
			  break;
		      
			case IC_END:
			  done = 1;
			  test_done_init = 1;

			  // Set up test sequence step number
			  test_step = 0;
			  break;
		      
			default:
			  printf("\nUnknown test IC code (test %d, i=%d)", test_number, i);
			  test_running = 0;
			  break;
			}

		      i++;
		    }
		}
	    }
	}
    }

#if ESC_TYPE_SMALL  
  // On last drive, process the scan
  if(kbd_drv_i == (NUM_KBD_DRV - 1))
    {
      // Copy scan code over
      kbd_output_scan_code = kbd_scan_code;

      // If scan code is different to the last scan code then if it is non zero,
      // inject a keycode into the main FSM
      if( (kbd_last_output_scan_code != kbd_output_scan_code) && (kbd_output_scan_code != 0) )
	{
#if DEBUG_KEY_SCAN
	  printf("\nKeyscan code :%03X", kbd_output_scan_code);
#endif
	  kbd_queue_key(kbd_output_scan_code);
	}

      kbd_last_output_scan_code = kbd_output_scan_code;
    }

#endif
  
}

#if ESC_TYPE_SMALL

void kbd_debounce(void)
{
  // Key pressed?
  if( kbd_scan_code == 0 )
    {
      // No key pressed now
      // Count down
      if( kbd_db_count > 0 )
	{
	  kbd_db_count--;
	}
    }
  else
    {
      // Key pressed now
      // If same key pressed as last scan then count up, else count down
      if( kbd_scan_code == kbd_last_scan_code )
	{
	  if( kbd_db_count < KBD_DB_COUNT_MAX )
	    {
	      kbd_db_count++;
	    }
	}
      else
	{
	  if( kbd_db_count > 0 )
	    {
	      kbd_db_count--;
	    }
	}
    }
}



void kbd_scan(ESC_STATE *s)
{
  kbd_t_slot = (kbd_t_slot + 1 ) % NUM_KBD_T_SLOTS;

  switch(kbd_t_slot)
    {
    case 1:
      kbd_drive();
      break;

    case 9:
      kbd_read(s);      
      break;
    }
}

#endif

////////////////////////////////////////////////////////////////////////////////
//
//
//

// The computer instance

ESC_STATE esc_state;

// Display
#define MAX_DSP   ((MAX_LINE+1)*NUM_LINES)

char dsp[1000];

char *get_display(void)
{
  return(dsp);
}


////////////////////////////////////////////////////////////////////////////////
//
// Read any register
//
// Always returns a double word as it could be a double word register
//

REGISTER_DOUBLE_WORD read_any_size_register(ESC_STATE *s, int n)
{
  if( n == TC_REG_IAR )
    {
      return((REGISTER_DOUBLE_WORD)s->iar.address);
    }

  if( n == TC_REG_LINK )
    {
      return((REGISTER_DOUBLE_WORD)s->link_register);
    }

  if( n == TC_REG_KI )
    {
      return((REGISTER_DOUBLE_WORD)s->keyboard_register);
    }

  if( n == TC_REG_ADDR )
    {
      return((REGISTER_DOUBLE_WORD)s->address_register);
    }

  if( n == TC_CL )
    {
      return(s->control_latch);
    }
  
  if( IS_SW_REGISTER(n) )
    {
      //      return((REGISTER_DOUBLE_WORD)SW_REG_CONTENTS(n));
      return(read_register(s, n));
    }

  if( IS_DW_REGISTER(n) )
    {
      //return((REGISTER_DOUBLE_WORD)DW_REG_CONTENTS(n));
      return(read_register(s, n));
    }

  error_msg("Unrecognised register:R%d", n);
}

////////////////////////////////////////////////////////////////////////////////
//
// Read any register and return absolute value
//

REGISTER_DOUBLE_WORD read_any_size_register_absolute(ESC_STATE *s, int n)
{
  if( IS_SW_REGISTER(n) )
    {
      //      return(REMOVED_SW_SIGN((REGISTER_DOUBLE_WORD)SW_REG_CONTENTS(n)));
      return(REMOVED_SW_SIGN(read_register(s, n)));
    }

  if( IS_DW_REGISTER(n) )
    {
      return(REMOVED_DW_SIGN(read_register(s,n)));
    }

  error_msg("Unrecognised register:R%d", n);
}

////////////////////////////////////////////////////////////////////////////////
//
// BCD
//
//
// Normalise a value after a binary addition
//
//

REGISTER_SINGLE_WORD single_sum_normalise(REGISTER_SINGLE_WORD v)
{
#if DEBUG_BCD_CORRECTION
  printf("\nValue:%08X", v);
#endif
  
  // Add 6 to each non-bcd digit
  for(int i=0; i<sizeof(REGISTER_SINGLE_WORD)*8; i+=4)
    {
      // get digit value
      int digit = ((v & (0xF << i)) >> i);

#if DEBUG_BCD_CORRECTION
      printf("\nDigit test:%d", digit);
#endif
      // Add 6 if not bcd
      switch(digit)
	{
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	  v += (0x6<<i);
	  printf("\n  Added 6:%08X", v);
	  break;
	}
    }

#if DEBUG_BCD_CORRECTION
  printf("\nValue:%08X", v);
#endif

  return(v);
}


REGISTER_DOUBLE_WORD double_sum_normalise(REGISTER_DOUBLE_WORD v)
{
#if DEBUG_BCD_CORRECTION
  printf("\n%s: Value:%016llX", __FUNCTION__, v);
#endif
  
  // Add 6 to each non-bcd digit
  for(int i=0; i<sizeof(REGISTER_DOUBLE_WORD)*8; i+=4)
    {
      // Get digit value
      REGISTER_DOUBLE_WORD digit = ((v & (0xFL << i)) >> i);

#if DEBUG_BCD_CORRECTION
      printf("\nDigit test:%d", digit);
#endif
      // Add 6 if not bcd
      switch(digit)
	{
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	  v += (0x6L<<i);
	  printf("\n  Added 6:%016llX", v);
	  break;
	}
    }

#if DEBUG_BCD_CORRECTION
  printf("\nValue:%016llX", v);
#endif

  return(v);
}

////////////////////////////////////////////////////////////////////////////////
//
// Ensure signs are tagged
// If there's no explicit plus then change it

int fix_sign(int sign)
{
  switch(sign)
    {
    case WORD_SIGN_MINUS:
    case WORD_SIGN_PLUS:
      break;
      
    default:
      sign = WORD_SIGN_PLUS;
      break;
    }
  return(sign);
}

////////////////////////////////////////////////////////////////////////////////
//
// Nines complement
//

REGISTER_SINGLE_WORD bcd_sw_nines_complement(REGISTER_SINGLE_WORD n)
{
  return(0x00999999 - n);
}

REGISTER_DOUBLE_WORD bcd_dw_nines_complement(REGISTER_DOUBLE_WORD n)
{
  return(0x0000999999999999L - n);
}

////////////////////////////////////////////////////////////////////////////////

// Get sign from any size register

int any_size_sign(ESC_STATE *s, int regno)
{
  if( IS_SW_REGISTER(regno) )
    {
      //      return( SW_SIGN(SW_REG_CONTENTS(regno)) );
      return( SW_SIGN(read_register(s, regno)) );
    }

  if( IS_DW_REGISTER(regno) )
    {
      //      return( DW_SIGN(DW_REG_CONTENTS(regno)) );
      return( DW_SIGN(read_register(s, regno)) );
    }

  error_msg("Unrecognised register:R%d", regno);
}

void set_any_size_sign(ESC_STATE *s, int regno, int sign)
{
  if( IS_SW_REGISTER(regno) )
    {
      //SW_REG_CONTENTS(regno) = SET_SW_SIGN(s->R[regno], sign);
      write_register(s, regno, SET_SW_SIGN(read_register(s, regno), sign));
      return;
    }

  if( IS_DW_REGISTER(regno) )
    {
      //DW_REG_CONTENTS(regno) = SET_DW_SIGN(s->RD[regno-8], sign);
      write_register(s, regno, SET_DW_SIGN(read_register(s, regno), sign));
      return;
    }

  error_msg("Unrecognised register:R%d", regno);
}

void set_any_size_rh6(ESC_STATE *s, int regno, int rh6)
{
  REGISTER_DOUBLE_WORD reg_contents = read_register(s, regno);
  
  if( IS_SW_REGISTER(regno) )
    {
      //      int reg_contents = SW_REG_CONTENTS(regno);
      //      SW_REG_CONTENTS(regno) = (reg_contents & 0xF0000000) | rh6;
      write_register(s, regno, (reg_contents & 0xFF000000) | rh6);
      return;
    }

  if( IS_DW_REGISTER(regno) )
    {
      //int reg_contents = DW_REG_CONTENTS(regno);
      //      int reg_contents = (REGISTER_SINGLE_WORD)read_register(s, regno);
      //DW_REG_CONTENTS(regno) = (reg_contents & 0xF000000000000000) | rh6;
      write_register(s, regno, (reg_contents & 0xFFFFFFFFFF000000) | rh6);
      return;
    }

  error_msg("Unrecognised register:R%d", regno);

}

//------------------------------------------------------------------------------
//
// Get and set the RH 6 digits of any size register
//

#if 0
SINGLE_WORD any_size_rh6(ESC_STATE *s, int regno)
{
  if( IS_SW_REGISTER(regno) )
    {
      int reg_contents = SW_REG_CONTENTS(regno);
      reg_contents &= 0x00FFFFFF;      

      //printf("\nRH6 of %d is %08X", regno, reg_contents);
      return(reg_contents);
    }

  if( IS_DW_REGISTER(regno) )
    {
      DOUBLE_WORD reg_contents = DW_REG_CONTENTS(regno);
      reg_contents &= 0x00FFFFFF;      
      //      DW_REG_CONTENTS(regno) = sign;
      return((SINGLE_WORD)reg_contents);
    }

  error_msg("Unrecognised register:R%d", regno);
}
#else

SINGLE_WORD any_size_rh6(ESC_STATE *s, int regno)
{
  REGISTER_DOUBLE_WORD reg_contents = read_register(s, regno);

  reg_contents &= 0x00FFFFFF;      
  //      DW_REG_CONTENTS(regno) = sign;
  return((SINGLE_WORD)reg_contents);
}

#endif

//------------------------------------------------------------------------------

REGISTER_SINGLE_WORD invert_sw_sign(REGISTER_SINGLE_WORD n)
{
  REGISTER_SINGLE_WORD r = n;
  
  if( SW_SIGN(n) == WORD_SIGN_PLUS)
    {
      r = SET_SW_SIGN(r, WORD_SIGN_MINUS);
    }

  if( SW_SIGN(n) == WORD_SIGN_MINUS)
    {
      r = SET_SW_SIGN(r, WORD_SIGN_PLUS);
    }
  
  return(r);
}

REGISTER_DOUBLE_WORD invert_dw_sign(REGISTER_DOUBLE_WORD n)
{
  REGISTER_DOUBLE_WORD r = n;
  
  if( DW_SIGN(n) == WORD_SIGN_PLUS)
    {
      r = SET_DW_SIGN(r, WORD_SIGN_MINUS);
    }

  if( DW_SIGN(n) == WORD_SIGN_MINUS)
    {
      r = SET_DW_SIGN(r, WORD_SIGN_PLUS);
    }
  
  return(r);
}

////////////////////////////////////////////////////////////////////////////////
//
// 24 bits BCD addition, positive only (no sign digit or exponent)
// 
//
////////////////////////////////////////////////////////////////////////////////

// Plain BCD addition

REGISTER_SINGLE_WORD bcd_addition_single(REGISTER_SINGLE_WORD a, REGISTER_SINGLE_WORD b)
{
  REGISTER_SINGLE_WORD c = 0;
  
  // Add each digit, testing for digit >=10 and also propagating carry to next digit
  //
  // Add 6 to each non-bcd digit

  int carry = 0;
  
  for(int i=0; i<sizeof(REGISTER_SINGLE_WORD)*8; i+=4)
    {
      // Get digit value
      int a_digit = ((a & (0xF << i)) >> i);
      int b_digit = ((b & (0xF << i)) >> i);
      int c_digit = a_digit + b_digit + carry;
      carry = 0;
      
#if DEBUG_BCD_CORRECTION
      printf("\n%s: Add: a:%d + b:%d = %d", __FUNCTION__, a_digit, b_digit, c_digit);
#endif
      // Add 6 if not bcd, we may need to propagate a carry (9+9 = 18, so 6 is added and 24 results
      // which is 0x18)
      
      switch(c_digit)
	{
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	  // Add 6, then propagate a carry
	  c_digit = (c_digit + 6) % 16;
	  carry = 1;
	  break;
	}

      // Build c, a digit at a time
      c += (c_digit << i);

#if DEBUG_BCD_CORRECTION
      printf("\n%s: Result so far: %08X", __FUNCTION__, c);
#endif

    }

  // Mask out the top two digits as there is probably an overflow from a nines-complement addition
  c &= 0x00FFFFFF;
  
#if DEBUG_BCD_CORRECTION
  printf("\n%s: Result: %08X", __FUNCTION__, c);
#endif
  
  return(c);
  
}


REGISTER_DOUBLE_WORD bcd_addition_double(REGISTER_DOUBLE_WORD a, REGISTER_DOUBLE_WORD b)
{
  REGISTER_DOUBLE_WORD c = 0;
  
  // Add each digit, testing for digit >=10 and also propagating carry to next digit
  //
  // Add 6 to each non-bcd digit

  int carry = 0;
  REGISTER_DOUBLE_WORD mask;
  
  for(int i=0; i<sizeof(REGISTER_DOUBLE_WORD)*8; i+=4)
    {
      mask = (REGISTER_DOUBLE_WORD)0xFL << i;
      
      // Get digit value
      REGISTER_DOUBLE_WORD a_digit = ((a & mask) >> i);
      REGISTER_DOUBLE_WORD b_digit = ((b & mask) >> i);
      REGISTER_DOUBLE_WORD c_digit = a_digit + b_digit + carry;
      carry = 0;
      
#if DEBUG_BCD_CORRECTION

      printf("\n%s: Add: a:%016llX + b:%016llX = %lld", __FUNCTION__, a_digit, b_digit, c_digit);
      printf("\na:%016llX a(2):%016llX a(3):%016llX", a, a & (0xFL <<i), mask);
#endif
      // Add 6 if not bcd, we may need to propagate a carry (9+9 = 18, so 6 is added and 24 results
      // which is 0x18)
      
      switch(c_digit)
	{
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	  // Add 6, then propagate a carry
	  c_digit = (c_digit + 6L) % 16L;
	  carry = 1;
	  break;
	}

      // Build c, a digit at a time
      c += (c_digit << i);

#if DEBUG_BCD_CORRECTION
      printf("\n%s: Result so far: %08X", __FUNCTION__, c);
#endif

    }

  // Mask out the top two digits as there is probably an overflow from a nines-complement addition
  c &= 0x0000FFFFFFFFFFFFL;
  
#if DEBUG_BCD_CORRECTION
  printf("\n%s: Result: %016llX", __FUNCTION__, c);
#endif
  
  return(c);
  
}

////////////////////////////////////////////////////////////////////////////////
// BCD single word addition
// Signed
//
// Both positive: Add, check for overflow
// Both negative: Add, check for overflow
// One negative, one positive: Nines complement negative, add, drop carry, add one, check for overflow (inverted)

REGISTER_SINGLE_WORD bcd_sw_addition(ESC_STATE *s, REGISTER_SINGLE_WORD a, REGISTER_SINGLE_WORD b)
{
  REGISTER_SINGLE_WORD c;
  int a_sign;
  int b_sign;

#if DEBUG_SW_BCD_SUM
  printf("\n--------------------------------");
  printf("\n%s:a=%08X  b=%08X", __FUNCTION__, a, b);
#endif
  
  // Remove and save the signs

  a_sign = SW_SIGN(a);
  b_sign = SW_SIGN(b);

  switch(a_sign)
    {
    case WORD_SIGN_MINUS:
    case WORD_SIGN_PLUS:
      break;
      
    default:
      a_sign = WORD_SIGN_PLUS;
      break;
    }

  switch(b_sign)
    {
    case WORD_SIGN_MINUS:
    case WORD_SIGN_PLUS:
      break;
      
    default:
      b_sign = WORD_SIGN_PLUS;
      break;
    }
  
#if DEBUG_SW_BCD_SUM
  printf("\nasgn=%d  bsgn=%d", a_sign, b_sign);
#endif

  a = REMOVED_SW_SIGN(a);
  b = REMOVED_SW_SIGN(b);

  // If signs are different then we need to work out what the sign of the result is
  int res_sign;
  
  if( a > b )
    {
      res_sign = a_sign;
    }
  else
    {
      res_sign = b_sign;
    }

#if DEBUG_SW_BCD_SUM
  printf("\na(rs)=%08X  b(rs)=%08X result sign:%d", a, b, res_sign);
#endif

  // If both numbers are positive or both are negative the just add the digits
  if( ((a_sign == WORD_SIGN_MINUS) && (b_sign == WORD_SIGN_MINUS)) ||
      ((a_sign == WORD_SIGN_PLUS) && (b_sign == WORD_SIGN_PLUS)) )
    {
#if DEBUG_SW_BCD_SUM
      printf("\nSigns identical");
#endif

      c = bcd_addition_single(a, b);

#if DEBUG_SW_BCD_SUM
      printf("\nc=%08X", c);
#endif

      c = single_sum_normalise(c);

#if DEBUG_SW_BCD_SUM
      printf("\nc=%08X", c);
#endif
      
      if( OVERFLOW_SW(c) )
	{
#if DEBUG_SW_BCD_SUM
	  printf("\nOverflow occurred");

	  error_msg( "Overflow (%08X)", c);
	  s->error = 1;
	  s->stop= 1;
#endif
	}

      // Signs are unchanged
      c = SET_SW_SIGN(c, a_sign);
      return(c);
    }

  // If we get here then the signs of the numbers are different
  // If number negative then use tens complemet
#if DEBUG_SW_BCD_SUM
  printf("\nSigns different");
#endif

  if( a_sign == WORD_SIGN_MINUS )
    {
      a = bcd_sw_nines_complement(a);
    }

  if( b_sign == WORD_SIGN_MINUS )
    {
      b = bcd_sw_nines_complement(b);
    }

#if DEBUG_SW_BCD_SUM
  printf("\n%s:a=%08X  b=%08X", __FUNCTION__, a, b);
#endif
  
#if DEBUG_SW_BCD_SUM
  printf("\nSigns different");
#endif

  c = bcd_addition_single(a, b);
  
  c = single_sum_normalise(c);
      
#if DEBUG_SW_BCD_SUM
  printf("\nc=%08X", c);
#endif


  // Overflow condition is reversed
  if( !OVERFLOW_SW(c) )
    {
#if DEBUG_SW_BCD_SUM
      printf("\nOverflow occurred");
#endif

      error_msg("Overflow (%08X)", c);
    }

  // Drop the carry
  c = CLEAR_SW_CARRY(c);

  // Add one
  c = bcd_addition_single(c, 1);
  
  c = single_sum_normalise(c);
  
  // If result is negative then nines complement it and add one as we use sign plus digits form for numbers
  // This format matches the floating point format
  if( res_sign == WORD_SIGN_MINUS )
    {
      c = bcd_sw_nines_complement(c);
      c = bcd_addition_single(c, 1);
      c = single_sum_normalise(c);
    }
  
  // The sign of the result will be that of the largest argument absoulte value
  c = SET_SW_SIGN(c, res_sign);
    
#if DEBUG_SW_BCD_SUM
  printf("\nc=%08X (9c:%08X)", c, bcd_sw_nines_complement(c));
#endif
  
  return(c);
}

////////////////////////////////////////////////////////////////////////////////
//
// BCD double word integer addition
//
// Signed
//
// Both positive: Add, check for overflow
// Both negative: Add, check for overflow
// One negative, one positive: Nines complement negative, add, drop carry, add one, check for overflow (inverted)

REGISTER_DOUBLE_WORD bcd_dw_addition(REGISTER_DOUBLE_WORD a, REGISTER_DOUBLE_WORD b)
{
  REGISTER_DOUBLE_WORD c;
  int a_sign;
  int b_sign;

#if DEBUG_DW_BCD_SUM
  printf("\n--------------------------------");
  printf("\n%s:a=%016llX  b=%016llX", __FUNCTION__, a, b);
#endif
  
  // Remove and save the signs
  a_sign = DW_SIGN(a);
  b_sign = DW_SIGN(b);

  a_sign = fix_sign(a_sign);
  b_sign = fix_sign(b_sign);
  
#if DEBUG_DW_BCD_SUM
  printf("\nasgn=%d  bsgn=%d", a_sign, b_sign);
#endif

  a = REMOVED_DW_SIGN(a);
  b = REMOVED_DW_SIGN(b);

  // If signs are different then we need to work out what the sign of the result is
  int res_sign;
  
  if( a > b )
    {
      res_sign = a_sign;
    }
  else
    {
      res_sign = b_sign;
    }

#if DEBUG_DW_BCD_SUM
  printf("\na(rs)=%016llX  b(rs)=%016llX result sign:%d", a, b, res_sign);
#endif

  // If both numbers are positive or both are negative the just add the digits
  if( ((a_sign == WORD_SIGN_MINUS) && (b_sign == WORD_SIGN_MINUS)) ||
      ((a_sign == WORD_SIGN_PLUS) && (b_sign == WORD_SIGN_PLUS)) )
    {
#if DEBUG_DW_BCD_SUM
      printf("\nSigns identical");
#endif

      c = bcd_addition_double(a, b);

#if DEBUG_DW_BCD_SUM
      printf("\nc=%016llX", c);
#endif

      c = double_sum_normalise(c);

#if DEBUG_DW_BCD_SUM
      printf("\nc=%016llX", c);
#endif
      
      if( OVERFLOW_DW(c) )
	{
#if DEBUG_DW_BCD_SUM
	  printf("\nOverflow occurred");

	  error_msg( "Overflow (%08X)", c);
#endif
	}

      // Signs are unchanged
      c = SET_DW_SIGN(c, a_sign);
      return(c);
    }

  // If we get here then the signs of the numbers are different
  // If number negative then use tens complemet
#if DEBUG_DW_BCD_SUM
  printf("\nSigns different result sign:%d", res_sign);
#endif

  if( a_sign == WORD_SIGN_MINUS )
    {
      a = bcd_dw_nines_complement(a);
    }

  if( b_sign == WORD_SIGN_MINUS )
    {
      b = bcd_dw_nines_complement(b);
    }

#if DEBUG_DW_BCD_SUM
  printf("\n%s:a=%016llX  b=%016llX", __FUNCTION__, a, b);
#endif
  
#if DEBUG_DW_BCD_SUM
  printf("\nSigns different");
#endif

  c = bcd_addition_double(a, b);
  
  c = double_sum_normalise(c);
      
#if DEBUG_DW_BCD_SUM
  printf("\nc=%016llX", c);
#endif


  // Overflow condition is reversed
  if( !OVERFLOW_DW(c) )
    {
#if DEBUG_DW_BCD_SUM
      printf("\nOverflow occurred");
#endif

      error_msg("Overflow (%08X)", c);
    }

  // Drop the carry
  c = CLEAR_DW_CARRY(c);

  // Add one
  c = bcd_addition_double(c, 1);
  
  c = double_sum_normalise(c);
  
  // If result is negative then nines complement it and add one as we use sign plus digits form for numbers
  // This format matches the floating point format
  if( res_sign == WORD_SIGN_MINUS )
    {
#if DEBUG_DW_BCD_SUM
      printf("res_sign is negative");
#endif
      
      c = bcd_dw_nines_complement(c);
      c = bcd_addition_double(c, 1);
      c = double_sum_normalise(c);
    }
  
  // The sign of the result will be that of the largest argument absoulte value
  c = SET_DW_SIGN(c, res_sign);
    
#if DEBUG_DW_BCD_SUM
  printf("\nc=%016llX (9c:%016llX)", c, bcd_sw_nines_complement(c));
#endif
  
  return(c);
}

////////////////////////////////////////////////////////////////////////////////
//
// Convert store value to an address.
// Addresses have no sign or exponent
//
////////////////////////////////////////////////////////////////////////////////

ADDRESS convert_store_to_address(SINGLE_WORD store_value)
{
  return(BOUND_ADDRESS(REMOVED_SW_SIGN(BOUND_ADDRESS(store_value))));
}

////////////////////////////////////////////////////////////////////////////////
//
// Convert BCD to binary
//

int bcd_to_binary(SINGLE_WORD bcd)
{
  char line[50];
  int binary;

#if DEBUG_BCD_TO_BINARY
  printf("\n%s: BCD:%X", __FUNCTION__, bcd);
#endif
  
  sprintf(line, "%X", bcd);
  sscanf(line, "%d", &binary);

#if DEBUG_BCD_TO_BINARY
  printf(" Bin1:%d", binary);
#endif

  // Limit to 200 as a store address
  if( binary < 0 )
    {
      binary = 0;
    }

  if( binary >199 )
    {
      binary = 199;
    }

#if DEBUG_BCD_TO_BINARY
  printf(" Bin2:%d", binary);
#endif
  
  
  return(binary);
}

////////////////////////////////////////////////////////////////////////////////

// Address is signed so has to have that stripped. It is never negative.
//
// It is also in BCD so has to be converted to binary to get an array index
// for the store array
//
SINGLE_WORD load_from_store(ESC_STATE *s, ADDRESS address)
{
  int idx = 0;
  SINGLE_WORD retval = 0;
  
  idx = bcd_to_binary(REMOVED_SW_SIGN(address));
  retval = s->store[idx];
  
#if DEBUG_LOAD_FROM_STORE
  printf("\n%s:address:%04X idx:%d retval=%08X", __FUNCTION__, address, idx, retval);
#endif

  return(retval);
}

void write_sw_to_store(ESC_STATE *s, ADDRESS address, REGISTER_SINGLE_WORD d)
{
  s->store[bcd_to_binary(REMOVED_SW_SIGN(address))] = d;
}

#if 0
REGISTER_DOUBLE_WORD get_register(ESC_STATE *s, int reg)
{
  return((REGISTER_DOUBLE_WORD)s->R[reg]);
}
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Various register assignments
//

void register_assign_register(ESC_STATE *s, int dest, int src)
{
#if DEBUG_REG_ASSIGN
  printf("\n%s:dest:%d src:%d", __FUNCTION__, dest, src);
#endif
  
  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src) )
    {
#if DEBUG_REG_ASSIGN
      printf("\nSW REG, SW REG");
#endif
      write_register(s, dest, read_register(s, src));
      //SW_REG_CONTENTS(dest) = SW_REG_CONTENTS(src);
    }
  
  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src) )
    {
#if DEBUG_REG_ASSIGN
      printf("\nDOUBLE WORD, DOUBLE WORD");
#endif
      write_register(s, dest, read_register(s, src));
      //DW_REG_CONTENTS(dest) = DW_REG_CONTENTS(src);

#if DEBUG_REG_ASSIGN
      printf("\n%s:dest:%016llX", __FUNCTION__, read_register(s, dest));
#endif
    }

  if( IS_SW_REGISTER(dest) && IS_DW_REGISTER(src) )
    {
#if DEBUG_REG_ASSIGN
      printf("\nSINGLE WORD <= DOUBLE WORD");
#endif
      write_register(s, dest, DW_TO_SW(read_register(s, src)));
      //SW_REG_CONTENTS(dest) = DW_TO_SW(DW_REG_CONTENTS(src));

#if DEBUG_REG_ASSIGN
      printf("\n%s:dest:%016llX", __FUNCTION__, read_register(s, dest));
#endif
    }

  if( IS_DW_REGISTER(dest) && IS_SW_REGISTER(src) )
    {
#if DEBUG_REG_ASSIGN
      printf("\nSINGLE WORD <= DOUBLE WORD");
#endif
      write_register(s, dest, SW_TO_DW(read_register(s, src)));
      //DW_REG_CONTENTS(dest) = SW_TO_DW(SW_REG_CONTENTS(src));

#if DEBUG_REG_ASSIGN
      printf("\n%s:dest:%016llX", __FUNCTION__, read_register(s, dest));
#endif
    }
}

//------------------------------------------------------------------------------

void register_assign_register_literal(ESC_STATE *s, int dest,  int literal)
{
  if( IS_SW_REGISTER(dest) )
    {
      write_register(s, dest, SET_SW_SIGN((REGISTER_SINGLE_WORD) literal, WORD_SIGN_PLUS));
      //SW_REG_CONTENTS(dest) = SET_SW_SIGN((REGISTER_SINGLE_WORD) literal, WORD_SIGN_PLUS);
      
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      write_register(s, dest, SET_DW_SIGN((REGISTER_DOUBLE_WORD) literal, WORD_SIGN_PLUS));
      //DW_REG_CONTENTS(dest) = (REGISTER_DOUBLE_WORD) literal;
    }
}

//------------------------------------------------------------------------------
//
// Literal is always positive.

void register_assign_sum_register_literal(ESC_STATE *s, int dest, int src, int literal)
{
#if DEBUG_REG_ASSIGN
  printf("\n%s:dest:%d src:%d lit:%d", __FUNCTION__, dest, src, literal);
#endif
  
  if( IS_SW_REGISTER(dest) )
    {
#if DEBUG_REG_ASSIGN
      printf("\nSINGLE WORD REG");
#endif

      REGISTER_SINGLE_WORD t;

      t = SET_SW_SIGN((REGISTER_SINGLE_WORD) literal, WORD_SIGN_PLUS);

      write_register(s, dest, bcd_sw_addition(s, read_register(s, src), t));
    //SW_REG_CONTENTS(dest) = bcd_sw_addition(s, SW_REG_CONTENTS(src), t);
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      REGISTER_DOUBLE_WORD t;

#if DEBUG_REG_ASSIGN
      printf("\nDOUBLE WORD REG");
#endif

      t = SET_DW_SIGN((REGISTER_DOUBLE_WORD) literal, WORD_SIGN_PLUS);
      write_register(s, dest, bcd_dw_addition(read_register(s, src), t));
  //  DW_REG_CONTENTS(dest) = bcd_dw_addition(DW_REG_CONTENTS(src), t);

#if DEBUG_REG_ASSIGN
      printf("\n%s:dest:%016llX (lit):%016llX", __FUNCTION__, read_register(s, dest), t);
#endif

    }
}

//------------------------------------------------------------------------------
//
// Subtract a register from a constant
//
// Swap register sign and add
// Literal is always positive

void register_assign_sub_literal_register(ESC_STATE *s, int dest, int literal, int src)
{
  if( IS_SW_REGISTER(dest) )
    {
      REGISTER_SINGLE_WORD t;

      //      t       = SET_SW_SIGN((REGISTER_SINGLE_WORD) SW_REG_CONTENTS(src), WORD_SIGN_MINUS);
      //      t = invert_sw_sign(SW_REG_CONTENTS(src));
      t = invert_sw_sign((REGISTER_SINGLE_WORD)read_register(s, src));
      
      literal = SET_SW_SIGN((REGISTER_SINGLE_WORD) literal,   WORD_SIGN_PLUS);

      //      SW_REG_CONTENTS(dest) = bcd_sw_addition(s, (REGISTER_SINGLE_WORD) literal, t);
      write_register(s, dest, bcd_sw_addition(s, (REGISTER_SINGLE_WORD) literal, t));
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      REGISTER_DOUBLE_WORD t;

      //t       = SET_DW_SIGN((REGISTER_DOUBLE_WORD) DW_REG_CONTENTS(src), WORD_SIGN_MINUS);
      //      t = invert_dw_sign(DW_REG_CONTENTS(src));
      t = invert_dw_sign(read_register(s, src));
      
      literal = SET_DW_SIGN((REGISTER_DOUBLE_WORD) literal,   WORD_SIGN_PLUS);

    //DW_REG_CONTENTS(dest) = bcd_dw_addition((REGISTER_DOUBLE_WORD) literal, t);
      write_register(s, dest, bcd_dw_addition((REGISTER_SINGLE_WORD) literal, t));
    }
}

void register_assign_sub_register_literal(ESC_STATE *s, int dest, int src, int literal)
{
  if( IS_SW_REGISTER(dest) )
    {
      REGISTER_SINGLE_WORD t;

      t = SET_SW_SIGN((REGISTER_SINGLE_WORD) literal, WORD_SIGN_MINUS);

     // SW_REG_CONTENTS(dest) = bcd_sw_addition(s, (REGISTER_SINGLE_WORD) t, SW_REG_CONTENTS(src));
      write_register(s, dest,  bcd_sw_addition(s, (REGISTER_SINGLE_WORD) t, (REGISTER_SINGLE_WORD)read_register(s, src)));
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      REGISTER_DOUBLE_WORD t;

      t = SET_DW_SIGN((REGISTER_DOUBLE_WORD) literal, WORD_SIGN_MINUS);

    // DW_REG_CONTENTS(dest) = bcd_dw_addition((REGISTER_DOUBLE_WORD) t, DW_REG_CONTENTS(src));
      write_register(s, dest,  bcd_dw_addition((REGISTER_DOUBLE_WORD) t, read_register(s, src)));
    }
}

void register_assign_sum_register_register(ESC_STATE *s, int dest, int src1, int src2)
{
  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src1) && IS_SW_REGISTER(src2) )
    {
      //      SW_REG_CONTENTS(dest) = bcd_sw_addition(s, SW_REG_CONTENTS(src1), SW_REG_CONTENTS(src2));
      write_register(s, dest, bcd_sw_addition(s, read_register(s, src1), read_register(s, src2)));
      return;
    }

  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src1) && IS_DW_REGISTER(src2) )
    {
      //      DW_REG_CONTENTS(dest) = bcd_dw_addition(DW_REG_CONTENTS(src1), DW_REG_CONTENTS(src2));
      write_register(s, dest, bcd_dw_addition(read_register(s, src1), read_register(s, src2)));
      return;
    }

  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src1) && IS_SW_REGISTER(src2) )
    {
      //      DW_REG_CONTENTS(dest) = bcd_dw_addition(DW_REG_CONTENTS(src1), SW_TO_DW(SW_REG_CONTENTS(src2)));
      write_register(s, dest, bcd_dw_addition(read_register(s, src1), SW_TO_DW(read_register(s, src2))));
      return;
    }

  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src1) && IS_DW_REGISTER(src2) )
    {
      //  SW_REG_CONTENTS(dest) = bcd_sw_addition(s, SW_REG_CONTENTS(src1), DW_TO_SW(DW_REG_CONTENTS(src2)));
      write_register(s, dest, bcd_sw_addition(s, read_register(s, src1), DW_TO_SW(read_register(s, src2))));
      return;
    }

  // error
  error_msg("Registers of different sizes");
}

////////////////////////////////////////////////////////////////////////////////

#if 0
void register_assign_sub_register_register(ESC_STATE *s, int dest, int src1, int src2)
{
#if DEBUG_REGISTER_ASSIGN
  printf("\n%s: dest:%d src1:%d src2:%d", __FUNCTION__, dest, src1, src2);
  printf("\n%s: issw:%d issw:%d issw:%d", __FUNCTION__, IS_SW_REGISTER(dest), IS_SW_REGISTER(src1), IS_SW_REGISTER(src2) );
  printf("\n%s: isdw:%d isdw:%d isdw:%d", __FUNCTION__, IS_DW_REGISTER(dest), IS_DW_REGISTER(src1), IS_DW_REGISTER(src2) );
  
#endif
  
  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src1) && IS_SW_REGISTER(src2) )
    {
#if DEBUG_REGISTER_ASSIGN
      printf("\nSW SW SW");
#endif
      //      SW_REG_CONTENTS(dest) = bcd_sw_addition(s, SW_REG_CONTENTS(src1), invert_sw_sign( SW_REG_CONTENTS(src2)));
      write_register(s, dest, bcd_sw_addition(s, read_register(s, src1), invert_sw_sign( read_register(s, src2))));
      return;
    }

  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src1) && IS_DW_REGISTER(src2) )
    {
#if DEBUG_REGISTER_ASSIGN
      printf("\nDW DW DW");
#endif
      //      DW_REG_CONTENTS(dest) = bcd_dw_addition(DW_REG_CONTENTS(src1), invert_dw_sign( DW_REG_CONTENTS(src2)));
      write_register(s, dest, bcd_dw_addition(read_register(s, src1), invert_dw_sign( read_register(s, src2))));
      
      return;
    }

  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src1) && IS_SW_REGISTER(src2) )
    {
#if DEBUG_REGISTER_ASSIGN
      printf("\nDW DW SW");
#endif
      //      DW_REG_CONTENTS(dest) = bcd_dw_addition(DW_REG_CONTENTS(src1), invert_dw_sign(SW_TO_DW(SW_REG_CONTENTS(src2))));
      write_register(s, dest, bcd_dw_addition(read_register(s, src1), invert_dw_sign( SW_TO_DW(read_register(s, src2)))));
      return;
    }

  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src1) && IS_DW_REGISTER(src2) )
    {
        
#if DEBUG_REGISTER_ASSIGN
      printf("\nSW SW DW");
#endif
      //      SW_REG_CONTENTS(dest) = bcd_sw_addition(s, SW_REG_CONTENTS(src1), invert_sw_sign(DW_TO_SW(DW_REG_CONTENTS(src2))));
      write_register(s, dest, bcd_sw_addition(s, read_register(s, src1), invert_sw_sign( DW_TO_SW(read_register(s, src2)))));
      return;
    }

  // error
  error_msg("Registers of different sizes");
}
#else
void register_assign_sub_register_register(ESC_STATE *s, int dest, int src1, int src2)
{
  int is_dest_dw = 0;
  int is_src1_dw = 0;
  int is_src2_dw = 0;

  is_dest_dw = IS_DW_REGISTER(dest);
  is_src1_dw = IS_DW_REGISTER(src1);
  is_src2_dw = IS_DW_REGISTER(src2);

  REGISTER_DOUBLE_WORD w_dest = read_register(s, dest);
  REGISTER_DOUBLE_WORD w_src1 = read_register(s, src1);
  REGISTER_DOUBLE_WORD w_src2 = read_register(s, src2);

#if DEBUG_REGISTER_ASSIGN
  printf("\n%s: dest:%d src1:%d src2:%d", __FUNCTION__, dest, src1, src2);
  printf("\n%s: isdw:%d isdw:%d isdw:%d", __FUNCTION__, is_dest_dw, is_src1_dw, is_src2_dw );
  
#endif

  // Convert SW values to DW
  if( !is_src1_dw )
    {
      w_src1 = SW_TO_DW(w_src1);
    }

  if( !is_src2_dw )
    {
      w_src2 = SW_TO_DW(w_src2);
    }

  // Perform arithmetic
  w_dest = bcd_dw_addition(w_src1, invert_dw_sign( w_src2));

  // Convert DW back to SW if needed
  if( !is_dest_dw )
    {
      w_dest = DW_TO_SW(w_dest);
    }
  
  // Write value back

#if DEBUG_REGISTER_ASSIGN
  printf("\n%s: src1:%016X", __FUNCTION__, w_src1 );
  printf("\n%s: src2:%016X", __FUNCTION__, w_src2 );
  printf("\n%s: src3:%016X", __FUNCTION__, w_dest );
  
#endif
  
  write_register(s, dest, w_dest);

}
#endif

////////////////////////////////////////////////////////////////////////////////

void register_assign_register_uint64(ESC_STATE *s, int dest, uint64_t n)
{
  if( IS_SW_REGISTER(dest) )
    {
      //      SW_REG_CONTENTS(dest) = (REGISTER_SINGLE_WORD)n;
      write_register(s, dest, n);
      return;
    }

  if( IS_DW_REGISTER(dest) )
    {
      //      DW_REG_CONTENTS(dest) = (REGISTER_DOUBLE_WORD) n;
      write_register(s, dest, n);
      return;
    }

  // error
  error_msg("Register unknown *%d), dest");
}

#if 0
#define SHIFT_INST(SHIFT_TYPE,SHIFT_OP)					\
									\
  void register_ ## SHIFT_TYPE ## _shift(ESC_STATE *s, int dest, int n)	\
  {									\
    int sign;								\
    REGISTER_SINGLE_WORD sw_data;					\
    REGISTER_DOUBLE_WORD dw_data;					\
									\
    if( IS_SW_REGISTER(dest) )						\
      {									\
	sign = SW_SIGN(SW_REG_CONTENTS(dest));				\
	sw_data = REMOVED_SW_SIGN(SW_REG_CONTENTS(dest)) SHIFT_OP (4*n); \
	sw_data = REMOVED_SW_UNUSED(sw_data);				\
	SW_REG_CONTENTS(dest) = SET_SW_SIGN(sw_data, sign);		\
	return;								\
      }									\
									\
    if( IS_DW_REGISTER(dest) )						\
      {									\
	sign = DW_SIGN(DW_REG_CONTENTS(dest));				\
	dw_data = REMOVED_DW_SIGN(DW_REG_CONTENTS(dest)) SHIFT_OP (4*n); \
	dw_data = REMOVED_DW_UNUSED(dw_data);				\
	DW_REG_CONTENTS(dest) = SET_DW_SIGN(dw_data, sign);		\
	return;								\
      }									\
									\
    error_msg("%s: Register unknown *%d", __FUNCTION__, dest);		\
  }

#else
#define SHIFT_INST(SHIFT_TYPE,SHIFT_OP)					\
									\
  void register_ ## SHIFT_TYPE ## _shift(ESC_STATE *s, int dest, int n)	\
  {									\
    int sign;								\
    REGISTER_SINGLE_WORD sw_data;					\
    REGISTER_DOUBLE_WORD dw_data;					\
									\
    if( IS_SW_REGISTER(dest) )						\
      {									\
	sign = SW_SIGN(read_register(s, dest));                          \
	sw_data = REMOVED_SW_SIGN(DW_TO_SW(read_register(s, dest))) SHIFT_OP (4*n); \
	sw_data = REMOVED_SW_UNUSED(sw_data);				\
        write_register(s, dest, SET_SW_SIGN(sw_data, sign));		\
	return;								\
      }									\
									\
    if( IS_DW_REGISTER(dest) )						\
      {									\
	sign = DW_SIGN(read_register(s, dest));                        \
	dw_data = REMOVED_DW_SIGN(read_register(s, dest)) SHIFT_OP (4*n); \
	dw_data = REMOVED_DW_UNUSED(dw_data);				\
	write_register(s, dest, SET_DW_SIGN(dw_data, sign));             \
	return;								\
      }									\
									\
    error_msg("%s: Register unknown *%d", __FUNCTION__, dest);		\
  }
#endif


SHIFT_INST(left,<<);
SHIFT_INST(right,>>);


////////////////////////////////////////////////////////////////////////////////
//
// Shift a DW right
//
// Sign is preserved
//

DOUBLE_WORD shift_dw_right(DOUBLE_WORD x)
{
  int sign = DW_SIGN(x);
  
  // Remove sign
  x = REMOVED_DW_SIGN(x);
  
  // Shift
  x >>= 4;

  // Put sign back
  x  = SET_DW_SIGN(x,  sign);

  return(x);
}

DOUBLE_WORD shift_dw_left(DOUBLE_WORD x)
{
  int sign = DW_SIGN(x);
  
  // Remove sign
  x = REMOVED_DW_SIGN(x);
  
  // Shift
  x <<= 4;

  // Put sign back
  x  = SET_DW_SIGN(x,  sign);

  return(x);
}

// Most significant digit shifted to left most position

SINGLE_WORD sw_msd_to_left(SINGLE_WORD x)
{
  SINGLE_WORD y;
  
  int sign;
  int exp;
  int digits;

  // Split the number
  sign   = STORE_GET_SIGN(x);
  exp    = STORE_GET_EXPONENT(x);
  digits = STORE_GET_DIGITS(x);

  // If exponent is 6 then we can't shift
  if( exp == 6 )
    {
      return(x);
    }
  
  // If zero then we can't do this
  if( digits == 0 )
    {
      return(x);
    }

  // Shift the digits while there's room, while updating the
  // exponent
  while( ((digits & 0x00F00000) == 0) && (exp <=4) )
    {
      digits <<=4;
      exp++;
    }

  // Rebuild the result
  y = STORE_SET_EXPONENT(y, exp);
  y = STORE_SET_SIGN(y,     sign);
  y = STORE_SET_DIGITS(y,   digits);

#if DEBUG_MSD_SHIFT
  printf("\n%s: ", __FUNCTION__);
  printf("\nx:%08X  y:%08X", x, y); 
#endif

  return(y);
}

SINGLE_WORD sw_shift_right(SINGLE_WORD x, int n)
{
  SINGLE_WORD y;
  
  int sign;
  int exp;
  int digits;

  // Split the number
  sign   = STORE_GET_SIGN(x);
  exp    = STORE_GET_EXPONENT(x);
  digits = STORE_GET_DIGITS(x);

  // If exponent is zero then we can't do this
  if( exp == 0 )
    {
      return(x);
    }
  
  // Shift the digits

  for(int i=0; i<n; i++)
    {
      digits >>=4;
      exp--;
    }

  // Rebuild the result
  y = STORE_SET_EXPONENT(y, exp);
  y = STORE_SET_SIGN(y,     sign);
  y = STORE_SET_DIGITS(y,   digits);

#if DEBUG_SW_SHIFT
  printf("\n%s: ", __FUNCTION__);
  printf("\nx:%08X  y:%08X", x, y); 
#endif

  return(y);
}

////////////////////////////////////////////////////////////////////////////////
//
// Floating point operations
//
////////////////////////////////////////////////////////////////////////////////
//
// These operations are written in a way that means that they should be
// re-writable in the instruction set of the ESC, so can be implemented
// as extracodes as the original did.
//
//------------------------------------------------------------------------------

SINGLE_WORD fp_add(ESC_STATE *s, SINGLE_WORD a, SINGLE_WORD b, int normalise)
{
  int exp_a, exp_b, exp_r;
  int exp_diff;
  int digits_a, digits_b;
  int sign_a, sign_b, sign_r;
  int digits_r;
  SINGLE_WORD result;

#if DEBUG_FP
  printf("\n%s: ", __FUNCTION__);
  printf("\na:%016X  b:%016X", a, b); 
#endif

  // trivial case
  if( STORE_GET_DIGITS(a) == 0 )
    {

#if DEBUG_FP_SUMMARY
      printf("\n%s: %s + ", __FUNCTION__, display_store_word(a));
      printf("%s = ", display_store_word(b));
      printf("%s", display_store_word(b));
#endif

      return(b);
    }

  if( STORE_GET_DIGITS(b) == 0 )
    {
#if DEBUG_FP_SUMMARY
      printf("\n%s: %s + ", __FUNCTION__, display_store_word(a));
      printf("%s = ", display_store_word(b));
      printf("%s", display_store_word(a));
#endif

      return(a);
    }
  
#if 1  
  if( normalise)
    {
      // Before we do any calculations, move the MS digit to one digit right of the left most position so we get
      // full resolution for the result and to make the test of exponents meaningful. We leave one digit to the 
      // This is done to both a and b
      
      a = sw_msd_to_left(a);
      b = sw_msd_to_left(b);
      a = sw_shift_right(a, 1);
      b = sw_shift_right(b, 1);
    }
#endif

  digits_a  =  STORE_GET_DIGITS(a);
  digits_b  =  STORE_GET_DIGITS(b);
  sign_a =  STORE_GET_SIGN(a);
  sign_b =  STORE_GET_SIGN(b);
  
  exp_a = STORE_GET_EXPONENT(a);
  exp_b = STORE_GET_EXPONENT(b);

  
  if( exp_a > exp_b )
    {
      // We shift a
      for(int i= (exp_a - exp_b); i>0; i--)
	{
	  digits_a >>= 4;
	}
      exp_r = exp_b;
      sign_r = sign_b;
    }
  else
    {
      // We shift b
      for(int i= (exp_b - exp_a); i>0; i--)
	{
	  digits_b >>= 4;
	}
      exp_r = exp_a;
      sign_r = sign_a;
    }

#if DEBUG_FP
  printf("\nexp_a:%016X  exp_b:%016X", exp_a, exp_b);
  printf("\ndigits_a:%016X  digits_b:%016X", digits_a, digits_b); 
#endif

  // Add digits
  result = bcd_sw_addition(s, SET_SW_SIGN(digits_a, sign_a), SET_SW_SIGN(digits_b,sign_b));

  // Put exponent back
  result = STORE_SET_EXPONENT(result,exp_r);

#if DEBUG_FP
  printf("\na:%s", display_store_word(a));
  printf("\nb:%s", display_store_word(b));
  printf("\nresult:%s", display_store_word(result));
  
#endif

#if DEBUG_FP_SUMMARY
  printf("\n%s: %s + ", __FUNCTION__, display_store_word(a));
  printf("%s = ", display_store_word(b));
  printf("%s", display_store_word(result));
#endif

  // Find smaller number and shift it so the exponents are the same
  return(result);
}


////////////////////////////////////////////////////////////////////////////////
//
// FP Subtract
//
////////////////////////////////////////////////////////////////////////////////

SINGLE_WORD fp_subtract(ESC_STATE *s, SINGLE_WORD a, SINGLE_WORD b, int normalise)
{
#if DEBUG_FP
  printf("\n%s: ", __FUNCTION__);
  printf("\na:%016X  b:%016X", a, b); 
#endif

  b = invert_sw_sign(b);

  return(fp_add(s, a, b, normalise));
}

////////////////////////////////////////////////////////////////////////////////
//
// FP Multiply
//
////////////////////////////////////////////////////////////////////////////////

// Uses double word registers internally

SINGLE_WORD fp_multiply(ESC_STATE *s, SINGLE_WORD a, SINGLE_WORD b)
{
  int exp_a, exp_b, exp_r;
  int exp_diff;
  int digits_a, digits_b;
  DOUBLE_WORD digits_r;
  int sign_a, sign_b, sign_r;
  SINGLE_WORD result;
  DOUBLE_WORD shifted_arg_digits;
  SINGLE_WORD tested_arg_digits;
  
  digits_a  =  STORE_GET_DIGITS(a);
  digits_b  =  STORE_GET_DIGITS(b);
  sign_a =  STORE_GET_SIGN(a);
  sign_b =  STORE_GET_SIGN(b);
  exp_a = STORE_GET_EXPONENT(a);
  exp_b = STORE_GET_EXPONENT(b);

  digits_r = 0;
  digits_r = SET_DW_SIGN(digits_r, WORD_SIGN_PLUS);
  
  // We add the smaller argument each time
  if( exp_a > exp_b )
    {
      shifted_arg_digits = digits_a;
      tested_arg_digits = digits_b;
    }
  else
    {
      shifted_arg_digits = digits_b;
      tested_arg_digits = digits_a;
    }

  // Work without sign or exponent
  shifted_arg_digits = SET_DW_SIGN(shifted_arg_digits, WORD_SIGN_PLUS);
  tested_arg_digits  = SET_SW_SIGN(tested_arg_digits,  WORD_SIGN_PLUS);
  
#if DEBUG_FP
  printf("\n%s: ", __FUNCTION__);
  printf("\na:%016X  b:%016X", a, b); 
#endif

  // We add one arg <digit> number of times then shift until all 6 digits are
  // processed.
  // Exponent then sorted out (added)
  // Sign then sorted out (xor'd)
  // Any overflow with addition => error

  for(int i=0; i<6; i++)
    {
#if DEBUG_FP
      printf("\nTested:%08X  shifted:%016llX", tested_arg_digits, shifted_arg_digits); 
#endif

      // test
      if( (tested_arg_digits & 0xF) != 0 )
        {
          for(int j=0; j<(tested_arg_digits & 0xF); j++)
            {
              digits_r = bcd_dw_addition(digits_r, shifted_arg_digits);
            }
        }
    
      // Remove sign
      shifted_arg_digits = REMOVED_DW_SIGN(shifted_arg_digits);
      tested_arg_digits  = REMOVED_SW_SIGN(tested_arg_digits);
    
      // Shift
      tested_arg_digits >>= 4;
      shifted_arg_digits <<=4;
      shifted_arg_digits = SET_DW_SIGN(shifted_arg_digits, WORD_SIGN_PLUS);
      tested_arg_digits  = SET_SW_SIGN(tested_arg_digits,  WORD_SIGN_PLUS);
    }

  
#if DEBUG_FP
  printf("\nexp_a :%016X  exp_b :%016X", exp_a, exp_b);
  printf("\nsign_a:%016X  sign_b:%016X", sign_a, sign_b);
  printf("\ndigits_a:%016X  digits_b:%016X digits_r:%016X", digits_a, digits_b, digits_r); 
#endif

  // We now need to shift the result so it will fit in a single word.
  // Shift so most significant digit is in the left most position, that will
  // give us the greatest resolution.

  exp_r = exp_a + exp_b;

#if DEBUG_FP
  printf("\nShifting to find MSD...");
  printf("\ndigits_r:%016llX exp_r:%d", digits_r, exp_r);
#endif
  
  while( ((digits_r & (DOUBLE_WORD)0x0000F00000000000L)==0) && ((digits_r & (DOUBLE_WORD)0x0000FFFFFFFFFFFFL) !=0) )
    {
      digits_r = shift_dw_left(digits_r);

      // Keep track of the number of decimal places

      exp_r++;
#if DEBUG_FP
      printf("\ndigits_r:%016llX exp_r:%d", digits_r, exp_r);
#endif
    }

#if DEBUG_FP
  printf("\nShifting done");
  printf("\ndigits_r:%016llX exp_r:%d", digits_r, exp_r);
#endif

#if DEBUG_FP
  printf("\nShifting down to fit in 6 places");
  printf("\ndigits_r:%016llX exp_r:%d", digits_r, exp_r);
#endif

  // Shift down  so the MSD is in the MS position for a single word
  // The position of the MSD was fixed above, so this is a shift of
  // a fixed number of digits.
  for(int p=0; p<6; p++)
    {
      digits_r = shift_dw_right(digits_r);

      // Keep track of the number of decimal places
      exp_r--;
    }

#if DEBUG_FP
  printf("\ndigits_r:%016llX exp_r:%d", digits_r, exp_r);
#endif
  
  // Keep result in a single word
  result = digits_r;
  
  // Put exponent back
  result = STORE_SET_EXPONENT(result, exp_r);

  // Put sign back
  if( sign_a == sign_b )
    {
      sign_r = WORD_SIGN_PLUS;
    }
  else
    {
      sign_r = WORD_SIGN_MINUS;
    }

  result = STORE_SET_SIGN(result, sign_r);
  
#if DEBUG_FP
  printf("\na:%s", display_store_word(a));
  printf("\nb:%s", display_store_word(b));
  printf("\nresult:%s", display_store_word(result));
  printf("\n%s END", __FUNCTION__);
 
#endif

#if DEBUG_FP_SUMMARY
  printf("\n%s: %s * ", __FUNCTION__, display_store_word(a));
  printf("%s = ", display_store_word(b));
  printf("%s", display_store_word(result));
#endif

  // Find smaller number and shift it so the exponents are the same
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// FP Divide
//
////////////////////////////////////////////////////////////////////////////////

// result = a / b

SINGLE_WORD fp_divide(ESC_STATE *s, SINGLE_WORD a, SINGLE_WORD b)
{
  int exp_a, exp_b, exp_r;
  int exp_diff;
  int digits_a, digits_b, digits_r;
  int sign_a, sign_b, sign_r;
  SINGLE_WORD result;
  SINGLE_WORD shifted_digits;   // arg a shifted
  SINGLE_WORD tested_digits;    // arg b test against and subtracted from
  SINGLE_WORD added_digits;         // number added to result

  // Before we do any calculations, move the MS digit to the left most position so we get
  // full resolution for the result. This is done to both a and b

#if DEBUG_FP
  printf("\n%s", __FUNCTION__);
  printf("\na:%016X b:%016X", a, b);
#endif

  a = sw_msd_to_left(a);
  b = sw_msd_to_left(b);

  digits_a  =  STORE_GET_DIGITS(a);
  digits_b  =  STORE_GET_DIGITS(b);
  sign_a =  STORE_GET_SIGN(a);
  sign_b =  STORE_GET_SIGN(b);
  exp_a = STORE_GET_EXPONENT(a);
  exp_b = STORE_GET_EXPONENT(b);

#if DEBUG_FP
  printf("\nAfter MSD shift");
  printf("\na:%016X b:%016X", a, b);
  printf("\ndigits_r       = %016X", digits_r);
#endif
  
  // The number added is a 1, but is in various digit positions
  // Sign added just before addition
  added_digits = 1;

  // Always shift arg b
  shifted_digits = digits_b;

  // Always test against arg a
  tested_digits = digits_a;

  // result digits, sign and exponent added later
  digits_r = 0;
  digits_r = STORE_SET_SIGN(digits_r, WORD_SIGN_PLUS);

  // Work without sign or exponent
  shifted_digits = STORE_SET_SIGN(shifted_digits, WORD_SIGN_PLUS);
  tested_digits  = STORE_SET_SIGN(tested_digits,  WORD_SIGN_PLUS);
  
#if DEBUG_FP
  printf("\n%s: ", __FUNCTION__);
  printf("\na:%016X  b:%016X", a, b); 
  printf("\nadded_digits   = %016X", added_digits);
  printf("\ntested_digits  = %016X", tested_digits);
  printf("\nshifted_digits = %016X", shifted_digits);
#endif


  // We shift until a non zero digit is in RH side of shifted digits
  int number_of_shifts = 0;

  if( (shifted_digits & 0x00FFFFFF)  == 0 )
    {
      // Divide by zero
#if DEBUG_FP_SUMMARY
      printf("\n%s: %s / ", __FUNCTION__, display_store_word(a));
      printf("%s = "      , display_store_word(b));
      printf("%s"         , display_store_word(result));
      
      printf("\nDivide by zero");
#endif
      // Error
      return(0xC0999999);
    }

#if 1
  while( (shifted_digits & 0x00F00000) == 0 )
    {
      shifted_digits <<=4;
      added_digits <<= 4;
      number_of_shifts++;
    }
#endif
  
  // We add one arg <digit> number of times then shift until all 6 digits are
  // processed.
  // Exponent then sorted out (added)
  // Sign then sorted out (xor'd)
  // Any overflow with addition => error

  shifted_digits = STORE_SET_SIGN(shifted_digits, WORD_SIGN_PLUS);
	
#if DEBUG_FP
  printf("\n**After initial shift left**");
  printf("\ndigits_r       = %016X", digits_r);
  printf("\nadded_digits   = %016X", added_digits);
  printf("\ntested_digits  = %016X", tested_digits);
  printf("\nshifted_digits = %016X", shifted_digits);
#endif
  
  int out = 100;
  added_digits = 0xC0100000;
  
  for(int i=0; (i<6) && (out > 0); i++,out--)
    {
      // Can we subtract?
#if DEBUG_FP
      printf("\n**Sub possible?**");
      printf("\ndigits_r       = %016X", digits_r);
      printf("\nadded_digits   = %016X", added_digits);
      printf("\ntested_digits  = %016X", tested_digits);
      printf("\nshifted_digits = %016X", shifted_digits);
#endif
      
      while( (REMOVED_SW_SIGN(tested_digits) >= REMOVED_SW_SIGN(shifted_digits)) && (REMOVED_SW_SIGN(shifted_digits) != 0) )
	{
#if DEBUG_FP
	  printf("\n**Can subtract**");
#endif
	  
	  // Add one to digit position
	  digits_r = fp_add(s, digits_r, added_digits, 0);
	  
	  tested_digits = fp_subtract(s, tested_digits, shifted_digits, 0);
#if DEBUG_FP
	  printf("\n**In Loop**");
	  printf("\ndigits_r       = %016X", digits_r);
	  printf("\nadded_digits   = %016X", added_digits);
	  printf("\ntested_digits  = %016X", tested_digits);
	  printf("\nshifted_digits = %016X", shifted_digits);
#endif
	}
      
      // Move to next position
      // Remove sign
      shifted_digits = REMOVED_SW_SIGN(shifted_digits);
      added_digits  = REMOVED_SW_SIGN(added_digits);
      
      // Shift
      added_digits   >>= 4;
      shifted_digits >>= 4;
      shifted_digits = STORE_SET_SIGN(shifted_digits, WORD_SIGN_PLUS);
      added_digits   = STORE_SET_SIGN(added_digits,  WORD_SIGN_PLUS);
    }
  
#if DEBUG_FP
  printf("\n**out of loops**");
  printf("\ndigits_r       = %016X", digits_r);
  printf("\nadded_digits   = %016X", added_digits);
  printf("\ntested_digits  = %016X", tested_digits);
  printf("\nshifted_digits = %016X", shifted_digits);
#endif
  
  
#if DEBUG_FP
  printf("\nexp_a :%016X  exp_b :%016X", exp_a, exp_b);
  printf("\nsign_a:%016X  sign_b:%016X", sign_a, sign_b);
  printf("\ndigits_a:%016X  digits_b:%016X digits_r:%016X", digits_a, digits_b, digits_r); 
#endif

  result = digits_r;

  //exp_r = exp_a - exp_b + 5 - number_of_shifts;
  exp_r = 5 - (exp_b - exp_a); // + 5 - number_of_shifts;
  result = STORE_SET_EXPONENT(result, exp_r);
    
#if DEBUG_FP
  printf("\nexp_r :%016X", exp_r);
#endif

  while( exp_r > 6 )
    {
      printf("\nsw loop");
#if DEBUG_FP
      printf("\nresult:%016X", result); 
#endif

      // Shift result right so it fits
      result = sw_shift_right(result, 1);
      exp_r--;
    }

#if DEBUG_FP
  printf("\nexp_r after normalise:%016X", exp_r);
#endif
  
  // Put exponent back as same number of decimal places as arg b
  result = STORE_SET_EXPONENT(result, exp_r);

  // Put sign back
  if( sign_a == sign_b )
    {
      sign_r = WORD_SIGN_PLUS;
    }
  else
    {
      sign_r = WORD_SIGN_MINUS;
    }

  result = STORE_SET_SIGN(result, sign_r);
  
#if DEBUG_FP
  printf("\na:%s", display_store_word(a));
  printf("\nb:%s", display_store_word(b));
  printf("\nNum of shifts:%d", number_of_shifts);
  printf("\nresult:%s", display_store_word(result));
  
#endif

#if DEBUG_FP_SUMMARY
  printf("\n%s: %s / ", __FUNCTION__, display_store_word(a));
  printf("%s = "      , display_store_word(b));
  printf("%s"         , display_store_word(result));
#endif

  // Find smaller number and shift it so the exponents are the same
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// IAR handling
//
// IAR is a simple two digit counter, plus a top half/bottom half flag for
// 4 digit instructions
//
// Logic:
//   If in KI reset then IAR does not advance
//
//   Digit a determines type of instruction
//    If a = 7,8, or 9 then IAR incremented and half flag cleared (it shouldn't be set)
//    Otherwise
//       If half flag is set then clear it and increment IAR
//       Otherwise set half flag
//
// Note: IAR is BCD
//
////////////////////////////////////////////////////////////////////////////////
//

// Load IAR
// Only lower two digits are loaded so if we are in extracode we stay in extracode
//
//
// We can detect exiting an extracode here if the hundreds digit goes from 1 (or non-zero?)
// to zero

void load_iar_bcd(ESC_STATE *s, int bcdval)
{
#if DEBUG_LOAD_IAR
  printf("\n%s:%08X", __FUNCTION__, bcdval);
#endif
  
  // Exiting extracode?
  if( IS_EXTRACODE )
    {
#if DEBUG_EXTRACODE
      printf("\n*** IAR being loaded in extracode***");
#endif
      if( (bcdval & 0xF00) == 0 )
        {
#if DEBUG_EXTRACODE
          printf("\n*** IAR will be in lower store ***");
#endif
          s->exiting_extracode = 1;

#if DEBUG_EXTRACODE
          printf("\nexiting_extracode=%d\n", s->exiting_extracode);
#endif
          
          // We will be exiting extracode, put up the appropriate display for
          // stage C, if we are in extracode step setting
          if( setup_step_extracode )
            {
#if DEBUG_EXTRACODE
              printf("\n*** Exiting extracode ***");
#endif
              s->stop = 1;
              
              // Put display up
              display_line_2(s, DISPLAY_UPDATE);
              display_on_line(s, DISPLAY_UPDATE, 2, "%3X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
              display_on_line(s, DISPLAY_UPDATE, 3, "%3X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
              display_on_line(s, DISPLAY_UPDATE, 4, "%3X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
              display_on_line(s, DISPLAY_UPDATE, 5, "               ");
            }
          else
            {
              // Not stepping extracode, so we would have been in run mode.
              // We need to stop if not running
              //s->run = 0;
              s->extracode_run = 0;
              if( !s->run )
                {
                  s->stop = 1;
                }
            }
        }
      else
        {
          // Not exiting an extracode subroutine
          s->exiting_extracode = 0;

#if DEBUG_EXTRACODE
          printf("\nexiting_extracode=%d\n", s->exiting_extracode);
#endif
        }
    }
  
  // Clear current digits
  //  s->iar.address &= 0x000;
  s->iar.address = BOUND_ADDRESS(bcdval);

#if DEBUG_LOAD_IAR
  printf("\n%s:%08X", __FUNCTION__, s->iar.address);
#endif

}

////////////////////////////////////////////////////////////////////////////////
//
// Calculate the address to put in the LINK register
//
// This is the next address, ignoring half addresses
//
////////////////////////////////////////////////////////////////////////////////

IAR calculate_link_address(IAR current_iar)
{
  IAR retval = current_iar;

  // We return the address of the next word

  // IAR is BCD so add one

  // Wrap from 099 to 000
  if( retval.address == 0x99 )
    {
      retval.address = 0x000;
    }
  else
    {
      retval.address = single_sum_normalise(bcd_addition_single(retval.address,1));
    }

  // Bound to three digits
  retval.address &= 0xFFF;
  retval.a_flag = 0;

  return(retval);
}


//------------------------------------------------------------------------------
//
// Move to next IAR while executing code.
//
// Also detects instrcution as single address instructions and three address
// instructions are treated differently
//

void next_iar(ESC_STATE *s)
{
  FN_ENTRY;

#if DEBUG_NEXT_IAR
  printf("\nIAR before:%s", display_iar(s, SPEC_IAR));
#endif

  int digit_a = INST_A_FIELD(s->instruction_register);
  
  if( s->ki_reset_flag && !IS_EXTRACODE )
    {
      return;
    }
  
  switch(digit_a)
    {
    case 7:
    case 8:
    case 9:
      if( s->iar.a_flag )
	{
	  warning_msg("8 digit instruction has A flag set");
	}
      
      // 8 digit instruction so move to next address
      // While executing, address wraps from 0x99 to 0x00
      if( s->iar.address == 0x99 )
        {
          s->iar.address = 0x000;
        }
      else
        {
          load_iar_bcd(s, single_sum_normalise(bcd_addition_single(s->iar.address,1)));
        }

      // IAR only three digits
      s->iar.address &= 0xFFF;
      s->iar.a_flag = 0;
      break;
      
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      if( s->iar.a_flag )
	{
          // While executing, address wraps from 0x99 to 0x00
          if( s->iar.address == 0x99 )
            {
              s->iar.address = 0x000;
            }
          else
            {
              load_iar_bcd(s, single_sum_normalise(bcd_addition_single(s->iar.address,1)));
            }
          
	  // IAR only three digits
	  s->iar.address &= 0xFFF;
	  s->iar.a_flag = 0;
	}
      else
	{
	  s->iar.a_flag = !(s->iar.a_flag);
	}
      break;
      
    default:
      // error
      error_msg("Unknown digit a");
      
      break;
    }

#if DEBUG_NEXT_IAR
  printf("\nIAR after: %s", display_iar(s, SPEC_IAR));
#endif
  
  FN_EXIT;
}

////////////////////////////////////////////////////////////////////////////////
//
//  Enter ERROR state
// 
////////////////////////////////////////////////////////////////////////////////

void enter_error_state(ESC_STATE *s, char *msg)
{
  // Stop execution
  s->run  = 0;
  s->stop = 1;

  display_on_line(s, DISPLAY_UPDATE, 2, "            %c  ", ERROR_RECTANGLE);

  printf("\n**** Error state:%s ****\n", msg);
  printf("\n    IAR:%s\n", display_iar(s, SPEC_IAR));
}


////////////////////////////////////////////////////////////////////////////////
//
// Displays standard line 2 with IAR and instruction
//
// Instruction display matches Fig 10 in IEE document.
//
// Three address instructions displayed fully.
// Instructions at XX displaye don left of field
// Instructions at XXA displaye don right
//
////////////////////////////////////////////////////////////////////////////////

void display_line_2(ESC_STATE *s, int display)
{
  char inst_str[9];
  int digit_a;
  
  inst_str[8] = '\0';
  
  if( !display )
    {
      return;
    }
  
  // We display IAR and the decoded digits of the instruction from the stage A decode

  // A different display if we have just exited an extracode or we are in an extracode subroutine 
  // AUX IAR is used to find the instruction, as that works for the extracodes.
  // the previous stages are from the extracode subroutine instructions for extracodes.
  SINGLE_WORD extracode_inst = load_from_store(s, s->aux_iar.address);

#if DEBUG_EXTRACODE
  printf("\nexiting extracode = %d extracode_inst=%08X", s->exiting_extracode, extracode_inst);
#endif
  
  // If stepping extracode then we want to see the subroutine instructions

  digit_a = s->inst_digit_a;
  
  if( s->exiting_extracode || (IS_EXTRACODE && !setup_step_extracode) )
    {
      // Force inst_digit_a to use the extracode instruction digit a, not the last subroutine
      // instruction digit a
      digit_a = INST_A_FIELD(extracode_inst);

#if DEBUG_EXTRACODE
      printf("\nUsing digit A set to %d", digit_a);
#endif
    }
  
  switch(digit_a)
    {
    case 7:
    case 8:
    case 9:
      if( s->exiting_extracode || (IS_EXTRACODE && !setup_step_extracode) )
        {
          // Get the instruction from the auxiliary IAR as that has the extracode address in it
          // The auxiliary IAR isn't updated during th eextracode subroutine execution

          
          inst_str[0] = INST_A_FIELD(extracode_inst)+'0';
          inst_str[1] = INST_B_FIELD(extracode_inst)+'0';
          inst_str[2] = INST_C_FIELD(extracode_inst)+'0';
          inst_str[3] = INST_D_FIELD(extracode_inst)+'0';
          inst_str[4] = INST_E_FIELD(extracode_inst)+'0';
          inst_str[5] = INST_F_FIELD(extracode_inst)+'0';
          inst_str[6] = INST_G_FIELD(extracode_inst)+'0';
          inst_str[7] = INST_H_FIELD(extracode_inst)+'0';
        }
      else
        {
          inst_str[0] = INST_A_FIELD(s->instruction_register)+'0';
          inst_str[1] = INST_B_FIELD(s->instruction_register)+'0';
          inst_str[2] = INST_C_FIELD(s->instruction_register)+'0';
          inst_str[3] = INST_D_FIELD(s->instruction_register)+'0';
          inst_str[4] = INST_E_FIELD(s->instruction_register)+'0';
          inst_str[5] = INST_F_FIELD(s->instruction_register)+'0';
          inst_str[6] = INST_G_FIELD(s->instruction_register)+'0';
          inst_str[7] = INST_H_FIELD(s->instruction_register)+'0';
        }
      break;

    default:
      if( s->aux_iar.a_flag )
	{
	  inst_str[0] = ' ';
	  inst_str[1] = ' ';
	  inst_str[2] = ' ';
	  inst_str[3] = ' ';
	  inst_str[4] = INST_E_FIELD(s->instruction_register)+'0';
	  inst_str[5] = INST_F_FIELD(s->instruction_register)+'0';
	  inst_str[6] = INST_G_FIELD(s->instruction_register)+'0';
	  inst_str[7] = INST_H_FIELD(s->instruction_register)+'0';
	}
      else
	{
	  inst_str[0] = INST_A_FIELD(s->instruction_register)+'0';
	  inst_str[1] = INST_B_FIELD(s->instruction_register)+'0';
	  inst_str[2] = INST_C_FIELD(s->instruction_register)+'0';
	  inst_str[3] = INST_D_FIELD(s->instruction_register)+'0';
	  inst_str[4] = ' ';
	  inst_str[5] = ' ';
	  inst_str[6] = ' ';
	  inst_str[7] = ' ';
	}
      break;
    }

#if DEBUG_STOP
  char stopch;
  stopch = (s->stop)?'S':' ';
#else
  char stopch = ' ';
#endif
  
  if( s->ki_reset_flag )
    {
      // Keyboard inout on line 1?  **TODO**
      display_on_line(s, DISPLAY_UPDATE, 2, "K  %s %c %c", inst_str, s->stage, stopch);
    }
  else
    {

      // If we are exiting extracode then we want to display the AUX IAR (extracode instruction
      // store address). Unless we are stepping extracode then we want to display the
      // subroutine instruction address
      if( s->exiting_extracode || (IS_EXTRACODE && !setup_step_extracode))
        {
          // In an extracode instruction, if we aren't stepping through the extracode
          // then the IAR is not showing the program address, but the extracode
          // instruction address. Do not display it.
          //          display_on_line(s, DISPLAY_UPDATE, 1, "%02s",       display_iar(s, SPEC_IAR));
          //display_on_line(s, DISPLAY_UPDATE, 1, "   ");
          display_on_line(s, DISPLAY_UPDATE, 2, "%02s %s %c %c", display_iar(s, SPEC_AUX_IAR), inst_str, s->stage, stopch);
        }
      else
        {
          //     display_on_line(s, DISPLAY_UPDATE, 1, "%02s",       display_iar(s, SPEC_IAR));
          display_on_line(s, DISPLAY_UPDATE, 2, "%02s %s %c %c", display_iar(s, SPEC_IAR), inst_str, s->stage, stopch);
        }

    }
}

////////////////////////////////////////////////////////////////////////////////


void display_three_address_values(ESC_STATE *s, int display)
{
  printf("\n%s display=%d", __FUNCTION__, display);

  if( display )
    {
      // If exiting from an extracode subroutine, display the extracode values, not
      // the s->Aa values as they will be from the last subroutine instruction.
      if( s->exiting_extracode )
        {
          display_on_line(s, display, 3, "%s", display_store_and_contents_from_tar(s, 0x100));
          display_on_line(s, display, 4, "%s", display_store_and_contents_from_tar(s, 0x101));
          display_on_line(s, display, 5, "%s", display_store_and_contents_from_tar(s, 0x102));
        }
      else
        {
          display_on_line(s, display, 3, "%s", display_store_and_contents(s, s->Aa1));
          display_on_line(s, display, 4, "%s", display_store_and_contents(s, s->Aa2));
          display_on_line(s, display, 5, "%s", display_store_and_contents(s, s->Aa3));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Instruction decode
//
// Completes instructions that require a third stage
// Moves IAR to next instruction

void stage_c_decode(ESC_STATE *s, int display)
{
  SINGLE_WORD a1v, a2v, a3v;
  SINGLE_WORD tst;
  int extreme_left_digit = 0;
  int extreme_right_digit = 0;
  int src_sign;
  int is_gt_zero = 0;
  int is_zero = 0;
  int is_lt_zero = 0;
  REGISTER_SINGLE_WORD  store_value;
  
  FN_ENTRY_DISPLAY;
  
#if DEBUG_STAGES
  printf(" [Stage C: AUXIAR:%03X%s IAR:%03X%s] (top entry)", s->aux_iar.address, s->aux_iar.a_flag?"A":" ", s->iar.address, s->iar.a_flag?"A":" ");
#endif

  // The input instructions stop at stage B so that the keyboard register can be loaded with
  // a value. At the beginning of stage C, the entered value is loaded as required.
  
  if( s->on_restart_load_aa )
    {
#if DEBUG_RESTART
      printf("\nLoading aa (%02X) with %s", s->inst_aa, display_store_word(s->keyboard_register));
#endif
      // Load aa with KB register
      write_sw_to_store(s, s->inst_aa, s->keyboard_register);
      s->on_restart_load_aa = 0;
      s->stop = 0;
      
      clear_keyboard_register(s);
    }
  
  if( s->on_restart_load_aa1 )
    {
#if DEBUG_RESTART
      printf("\nLoading Aa1 (%02X) with %s", s->Aa1, display_store_word(s->keyboard_register));
#endif
      // Load Aa1 with KB register
      write_sw_to_store(s, s->Aa1, s->keyboard_register);
      s->on_restart_load_aa1 = 0;
      s->stop = 0;
      clear_keyboard_register(s);
    }
  
  // Decode the instruction
  // First the digits 1-4
  // A different display if we have just exited an extracode. 
  // AUX IAR is used to find the instruction, as that works for the extracodes.
  // the previous stages are from the extracode subroutine instructions for extracodes.
  
  SINGLE_WORD extracode_inst = load_from_store(s, s->aux_iar.address);

#if DEBUG_EXTRACODE
  printf("\nexiting extracode = %d extracode_inst=%08X", s->exiting_extracode, extracode_inst);
#endif
  
  // If we are stepping extracode then we do want to display the subroutine instruction
#if 0
  if( s->exiting_extracode || (IS_EXTRACODE && !setup_step_extracode) )
    {
      // Force inst_digit_a to use the extracode instruction digit a, not the last subroutine
      // instruction digit a
      // Do same for digit b
      // This ensures the correct instruction is decoded if exiting extracode
      s->inst_digit_a = INST_A_FIELD(extracode_inst);
      s->inst_digit_b = INST_B_FIELD(extracode_inst);
    }
#endif
  
#if DEBUG_EXTRACODE
      printf("\nInst digit A set to %d", s->inst_digit_a);
#endif

#if DEBUG_STAGES
  printf(" [Stage C: AUXIAR:%03X%s IAR:%03X%s] (entry)", s->aux_iar.address, s->aux_iar.a_flag?"A":" ", s->iar.address, s->iar.a_flag?"A":" ");
#endif

  switch(s->inst_digit_a)
    {
    case 0:
      switch(s->inst_digit_b)
	{
	case 0:
          register_assign_sum_register_literal(s, s->reginst_rc, s->reginst_rc, s->reginst_literal);
          break;

        case 1:
	  // (Rc) <= (Rc) - d
	  register_assign_sub_register_literal(s, s->reginst_rc, s->reginst_rc, s->reginst_literal);
	  break;

	case 2:
	  // (Rc) <= d - (Rc)
	  register_assign_sub_literal_register(s, s->reginst_rc, s->reginst_literal, s->reginst_rc);

	  // display_line_2(s, display);
	  // display_on_line(s, display, 3, "%s", display_register_and_contents(s, s->reginst_rc));
	  // display_on_line(s, display, 4, "               ");
	  // display_on_line(s, display, 5, "               ");
	  // display_on_line(s, display, 6, "               ");
	  break;
	  
	case 3:
	  // (Rc) <= d
	  register_assign_register_literal(s, s->reginst_rc, s->reginst_literal);

	  // display_line_2(s, display);
	  // display_on_line(s, display, 3, "%s", display_register_and_contents(s, s->reginst_rc));
	  // display_on_line(s, display, 4, "               ");
	  // display_on_line(s, display, 5, "               ");
	  // display_on_line(s, display, 6, "               ");
	  break;

	case 4:
	  // Not used
	  break;
	  
	case 5:
	  // Test (Rc)
#if DEBUG_TEST
	  printf("\nTEST (5) [R%d] = %08X", s->reginst_rc, read_any_size_register_absolute(s, s->reginst_rc));
#endif
	  switch(s->inst_digit_d)
	    {
	    case 0:
	      is_zero = 0;
	      
#if DEBUG_TEST
	      printf("\nTEST (5) ");
#endif
	      
	      if( IS_SW_REGISTER(s->reginst_rc) )
		{
                  //		  if( (SW_REG_CONTENTS(s->reginst_rc) & 0xFFFFFF) == 0 )
                  if( (read_register(s, s->reginst_rc) & 0xFFFFFF) == 0 )
		    {
		      is_zero = 1;
		    }
		}

	      if( IS_DW_REGISTER(s->reginst_rc) )
		{
		  if( (read_register(s, s->reginst_rc) & 0xFFFFFFFFFFFF) == 0 )
		    {
		      is_zero = 1;
		    }
		}
	      
	      if( is_zero )
		{
#if DEBUG_TEST
		  printf("\nCL = 1 after test (is = 0)");
#endif
		  s->control_latch = 1;
		}
	      else
		{
#if DEBUG_TEST
		  printf("\nCL = 0 after test (is not = 0)");
#endif
		  s->control_latch = 0;
		}
	      break;
	      
	    case 1:
	      is_gt_zero = 0;
	      
	      if( IS_SW_REGISTER(s->reginst_rc) )
		{
#if DEBUG_TEST
		  printf("\nR%d = %016X", s->reginst_rc, read_register(s, s->reginst_rc));
                  printf("\nR%d = %08X", s->reginst_rc, read_register(s, s->reginst_rc));
                  printf("\nR%d = %08X", s->reginst_rc, SW_SIGN(read_register(s, s->reginst_rc)));
#endif

                  //		  if( (SW_SIGN(SW_REG_CONTENTS(s->reginst_rc)) == WORD_SIGN_PLUS) && ((SW_REG_CONTENTS(s->reginst_rc) & 0xFFFFFF) != 0) )
                  if( (SW_SIGN(read_register(s, s->reginst_rc)) == WORD_SIGN_PLUS) &&
                      ((read_register(s, s->reginst_rc) & 0xFFFFFF) != 0) )
		    {
		      is_gt_zero = 1;
		    }
		}

	      if( IS_DW_REGISTER(s->reginst_rc) )
		{
		  if( (DW_SIGN(read_register(s, s->reginst_rc)) == WORD_SIGN_PLUS) && ((read_register(s, s->reginst_rc) & 0xFFFFFFFFFFFF) != 0) )
		    {
		      is_gt_zero = 1;
		    }
		}
	      
	      if( is_gt_zero )
		{
#if DEBUG_TEST
		  printf("\nCL = 1 after test (is > 0)");
#endif
		  s->control_latch = 1;
		}
	      else
		{
#if DEBUG_TEST
		  printf("\nCL = 0 after test (is not > 0)");
#endif
		  s->control_latch = 0;
		}
	      break;
	      
	    case 2:
              // Check for R < 0
              // If zero then no
              // The reason for the check of zero is that it is possible for the sign to be -ve
              // and the value to be zero.
	      is_lt_zero = 0;
              is_zero = 0;
	      
#if DEBUG_TEST
	      printf("\nTEST (5) ");
#endif
              
	      if( IS_SW_REGISTER(s->reginst_rc) )
		{
		  if( (read_register(s, s->reginst_rc) & 0xFFFFFF) == 0 )
		    {
		      is_zero = 1;
		    }
		}

	      if( IS_DW_REGISTER(s->reginst_rc) )
		{
		  if( (read_register(s, s->reginst_rc) & 0xFFFFFFFFFFFF) == 0 )
		    {
		      is_zero = 1;
		    }
		}
	      
	      if( is_zero )
		{
#if DEBUG_TEST
		  printf("\nCL = 0 after test (is < 0) as is 0");
#endif
		  s->control_latch = 0;
		}
              else
                {
#if DEBUG_TEST
		  printf("\nNot zero, test sign...");
#endif

                  // Not zero, so check sign
                  is_lt_zero = 0;
                  
                  if( IS_SW_REGISTER(s->reginst_rc) )
                    {
                      if( SW_SIGN(read_register(s, s->reginst_rc)) == WORD_SIGN_MINUS )
                        {
#if DEBUG_TEST
                          printf("\nSW value < 0 (%016X)", read_register(s, s->reginst_rc));
#endif

                          is_lt_zero = 1;
                        }
                    }
                  
                  if( IS_DW_REGISTER(s->reginst_rc) )
                    {
                      if( DW_SIGN(read_register(s, s->reginst_rc)) == WORD_SIGN_MINUS )
                        {
                          is_lt_zero = 1;
                        }
                    }
                }
              
	      if( is_lt_zero )
		{
#if DEBUG_TEST
		  printf("\nCL = 1 after test");
#endif
		  s->control_latch = 1;
		}
	      else
		{
#if DEBUG_TEST
		  printf("\nCL = 0 after test");
#endif
		  s->control_latch = 0;
		}

	      break;
	      
	    case 3:

	      if( IS_SW_REGISTER(s->reginst_rc) )
		{
		  extreme_left_digit = (read_register(s, s->reginst_rc) & 0x00F00000) >> (5*4);
		}

	      if( IS_DW_REGISTER(s->reginst_rc) )
		{
		  extreme_left_digit = (read_register(s, s->reginst_rc) & 0x0000F00000000000) >> (11*4);
		}

#if DEBUG_TEST
	      printf("\nTEST extreme left:%d ", extreme_left_digit);
#endif
	      
	      if( extreme_left_digit == 0 )
		{
		  s->control_latch = 1;
		}
	      else
		{
		  s->control_latch = 0;
		}
	      break;


	    case 4:
	      if( IS_SW_REGISTER(s->reginst_rc) )
		{
		  extreme_right_digit = (read_register(s, s->reginst_rc) & 0x0000000F) >> 0;
		}
	      
	      if( IS_DW_REGISTER(s->reginst_rc) )
		{
		  extreme_right_digit = (read_register(s, s->reginst_rc) & 0x000000000000000F) >> 0;
		}
	      
#if DEBUG_TEST
	      printf("\nTEST extreme right:%d ", extreme_right_digit);
#endif
	      
	      if( extreme_right_digit == 0 )
		{
		  s->control_latch = 1;
		}
	      else
		{
		  s->control_latch = 0;
		}
	      break;
	      
	    }

	  // display_line_2(s, display);
	  // clear_lines_3_to_6(s, display);
	  // display_any_size_register_on_line(s, display, 3, s->reginst_rc,  CONTENTS);
	  // display_on_line(s, display, 6, "CL            %d", s->control_latch);
	  
	  break;
	  
	  // Shift (Rc) left d places 
	case 6:
	  register_left_shift(s, s->reginst_rc, s->reginst_literal);

	  // display_line_2(s, display);
	  // clear_lines_3_to_6(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;
	  
	  // Shift (Rc) right d places
	case 7:
	  register_right_shift(s, s->reginst_rc, s->reginst_literal);

	  // display_line_2(s, display);
	  // clear_lines_3_to_6(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;

	case 8:
	  // Not used
          enter_error_state(s, "08xx not used");
	  break;

	case 9:
	  // Not used
          enter_error_state(s, "09xx not used");
	  break;
        }

      // All instructions move to next IAR (no branches)
      next_iar(s);
      break;
      
    case 1:
#if DEBUG_STAGES
      printf("\ninst_digit_b:%d", s->inst_digit_b);
#endif
      switch(s->inst_digit_b)
	{
	case 0:
	  // Add registers
	  register_assign_sum_register_register(s, s->reginst_rc, s->reginst_rc, s->reginst_rd);
	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;

	case 1:
	  // Subtract registers (Rc)-(Rd)
	  register_assign_sub_register_register(s, s->reginst_rc, s->reginst_rc, s->reginst_rd);
	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;
	  
	case 2:
	  // Subtract registers (Rd)-(Rc)
#if DEBUG_STAGES
          printf("\nSubtract registers (Rd)-(Rc)");
#endif

	  register_assign_sub_register_register(s, s->reginst_rc, s->reginst_rd, s->reginst_rc);
	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;
	  
	case 3:
	  // Register assign (Rc) <-(Rd)
	  register_assign_register(s, s->reginst_rc, s->reginst_rd);
	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;

	  // Copy right hand 6 digits of Rd into Rc. Works with single and double length registers
	case 4:
	  // First the sign
	  src_sign = any_size_sign(s, s->reginst_rd);
	  //@printf("\nsrc_sign=%d", src_sign);
	  
	  set_any_size_sign(s, s->reginst_rc, src_sign);

	  //printf("\nR[]=%08X", s->R[s->reginst_rc]);
	  
	  // Then the RH six digits
	  int rh6 = any_size_rh6(s, s->reginst_rd);
	  set_any_size_rh6(s, s->reginst_rc, rh6);
	  //printf("\nR[]=%08X", s->R[s->reginst_rc]);

	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;

	case 5:
	  // Not used
	  break;
	  
	  // Shift (Rc) left (Rd) places 
	case 6:
	  register_left_shift(s, s->reginst_rc, read_any_size_register_absolute(s, s->reginst_rd));

	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;
	  
	  // Shift (Rc) right (Rd) places
	case 7:
	  register_right_shift(s, s->reginst_rc, read_any_size_register_absolute(s, s->reginst_rd));

	  // display_line_2(s, display);
	  // display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
	  break;

	case 8:
	  // Not used
          enter_error_state(s, "18xx not used");
	  break;

	case 9:
          // This instruction is used to stop and display X,Y,Z when running in an extracode subroutine.
          // Check if we are in an extracode and behave appropriately.

          if( IS_EXTRACODE )
            {
              //
              // Stop and display X, Y, Z
              // 
              s->stop = 1;
              s->inst_update_display = 1;
              // display_line_2(s, DISPLAY_UPDATE);
              // display_on_line(s, DISPLAY_UPDATE, 3, "%3X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
              // display_on_line(s, DISPLAY_UPDATE, 4, "%3X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
              // display_on_line(s, DISPLAY_UPDATE, 5, "%3X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
              // display_on_line(s, DISPLAY_UPDATE, 6, "               ");

            }
          else
            {
              // Stop and display (Rc) and (Rd)
              // 
              s->stop = 1;
              s->inst_update_display = 1;
              // display_line_2(s, DISPLAY_UPDATE);
              // display_two_any_size_register_on_line(s, DISPLAY_UPDATE, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
            }
	  break;
	}
      
      // Instruction complete, move IAR on
      next_iar(s);

      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      switch(s->inst_digit_b)
	{
	case 0:
          // Copy (Aa) into R0 and R1 as follows. If Aa contains
	  // data (recognisable by a sign in digit position 1), copy the
	  // exponent digit into R0 and the sign and significant digits into
	  // R1. If Aa contains an instruction (recognisable by a decimal
	  // digit in position 1), copy the left-hand four digits into Ro and
	  // the right-hand four digits into Ri
          
	  // Fixed registers
	  s->reginst_rc = 0;
	  s->reginst_rd = 1;
	  
	  store_value = load_from_store(s, s->inst_aa);
	  
	  if( (STORE_GET_SIGN(store_value)==WORD_SIGN_PLUS) || (STORE_GET_SIGN(store_value)==WORD_SIGN_MINUS) )
	    {
	      // Data (value)
#if 0
	      s->R[0] = STORE_GET_EXPONENT(store_value);
	      s->R[0] = SET_SW_SIGN(s->R[0], WORD_SIGN_PLUS);
	      s->R[1] = STORE_GET_DIGITS(store_value);
	      s->R[1] = SET_SW_SIGN(s->R[1], STORE_GET_SIGN(store_value));
#endif
	      write_register(s, 0, STORE_GET_EXPONENT(store_value));
	      write_register(s, 0, SET_SW_SIGN(read_register(s, 0), WORD_SIGN_PLUS));
              write_register(s, 1, STORE_GET_DIGITS(store_value));
              write_register(s, 1, SET_SW_SIGN(read_register(s, 1), STORE_GET_SIGN(store_value)));

	    }
	  else
	    {
	      // Instruction
#if 0
	      s->R[0] = STORE_GET_LH4_DIGITS(store_value);
	      s->R[0] = SET_SW_SIGN(s->R[0], WORD_SIGN_PLUS);
	      s->R[1] = STORE_GET_RH4_DIGITS(store_value);
	      s->R[1] = SET_SW_SIGN(s->R[1], WORD_SIGN_PLUS);
#endif
	      write_register(s, 0, STORE_GET_LH4_DIGITS(store_value));
	      write_register(s, 0, SET_SW_SIGN(read_register(s, 0), WORD_SIGN_PLUS));
	      write_register(s, 1, STORE_GET_RH4_DIGITS(store_value));
	      write_register(s, 1, SET_SW_SIGN(read_register(s, 1), WORD_SIGN_PLUS));

	    }

	  next_iar(s);
	  
	  // display_line_2(s, display);
	  // display_on_line(s, display, 3, "               ");
	  // display_on_line(s, display, 4, "%s", display_store_and_contents(s, s->inst_aa));
	  // display_on_line(s, display, 5, "%s", display_register_and_contents(s, s->reginst_rc));
	  // display_on_line(s, display, 6, "%s", display_register_and_contents(s, s->reginst_rd));
	  break;
	  
	case 1:
	  // Store (R0) and (R1) in location Aa in data format; i.e. copy (R0)
	  // into the exponent position, and copy (RO into the sign and
	  // significant digit positions. If the number is outside the range
	  // that can be held in a storage location, set the ERROR indicator
	  // and stop

	  // Fixed registers
	  s->reginst_rc = 0;
	  s->reginst_rd = 1;

	  store_value = 0;
	  store_value = STORE_SET_EXPONENT(store_value, read_register(s, 0));
	  store_value = STORE_SET_SIGN(    store_value, SW_SIGN(read_register(s, 1)));
	  store_value = STORE_SET_DIGITS(  store_value, SW_DIGITS(read_register(s, 1)));
	  write_sw_to_store(s, s->inst_aa, store_value);
	  
#if DEBUG_INST_21
	  printf("\nINST 21");
	  printf("\nWriting %08X to store location %02X", store_value, s->inst_aa);
#endif

          next_iar(s);
	  // display_line_2(s, display);
	  // display_on_line(s, display, 3, "               ");
	  // display_on_line(s, display, 4, "%s", display_store_and_contents(s, s->inst_aa));
	  // display_on_line(s, display, 5, "%s", display_register_and_contents(s, s->reginst_rc));
	  // display_on_line(s, display, 6, "%s", display_register_and_contents(s, s->reginst_rd));
	  break;
	  
	case 2:
	  // Store (R0) and (R1) in location Aa in instruction format; i.e.
	  // copy (R0) into the left-hand four digit positions and (R1) into
	  // the right-hand four digit positions
          // Fixed registers
	  s->reginst_rc = 0;
	  s->reginst_rd = 1;

	  store_value = 0;
	  store_value = STORE_SET_LH4_DIGITS(store_value, read_register(s, 0));
	  store_value = STORE_SET_RH4_DIGITS(store_value, read_register(s, 1));
	  write_sw_to_store(s, s->inst_aa, store_value);
	  
#if DEBUG_INST_22
	  printf("\nINST 22");
	  printf("\nWriting %08X to store location %02X", store_value, s->inst_aa);
#endif

          next_iar(s);
	  // display_line_2(s, display);
	  // display_on_line(s, display, 3, "               ");
	  // display_on_line(s, display, 4, "%s", display_store_and_contents(s, s->inst_aa));
	  // display_on_line(s, display, 5, "%s", display_register_and_contents(s, s->reginst_rc));
	  // display_on_line(s, display, 6, "%s", display_register_and_contents(s, s->reginst_rd));
	  break;
	  
	case 4:
	  // Unconditional branch

	  // Calculate and store the link address
          s->iar = calculate_link_address(s->iar);
	  s->link_register = s->iar.address;
	  
	  // Now over-write that IAR with the address we want to jump to
          load_iar_bcd(s, s->inst_aa);

          //	  s->iar.address = s->inst_aa;

          s->iar.a_flag = 0;

	  // Unconditional branch
	  // IAR already set up.

	  // display_line_2(s, display);
	  // display_on_line(s, display, 3, "%s", display_store_and_contents(s, s->inst_aa));
	  break;

        case 5:
          // Branch if control latch is 1
          if( s->control_latch == 1 )
            {
#if DEBUG_BRANCH
              printf("\n25xx BRANCH");
#endif
              // Calculate and store the link address
              s->iar = calculate_link_address(s->iar);
              s->link_register = s->iar.address;
	  
              // Now over-write that IAR with the address we want to jump to
              //	      s->iar.address = s->inst_aa;
              load_iar_bcd(s, s->inst_aa);
              s->iar.a_flag = 0;
            }
          else
            {
              // Move to next IAR if test fails
              next_iar(s);
            }
      
          break;

        case 6:
      	  // Branch if control latch is 0
	  if( s->control_latch == 0 )
	    {
              // Calculate and store the link address
              s->iar = calculate_link_address(s->iar);
              s->link_register = s->iar.address;

	      // Now over-write that IAR with the address we want to jump to
              //	      s->iar.address = s->inst_aa;
              load_iar_bcd(s, s->inst_aa);
	      s->iar.a_flag = 0;
	    }
	  else
	    {
	      // Move to next IAR if test fails
	      next_iar(s);
	    }
          break;
      
        case 7:
      	  // Store contents of link address in Aa
	  write_sw_to_store(s, s->inst_aa, s->link_register);

          next_iar(s);
          break;

          // x8 and x9 done in stage B
#if 0          
        case 8:
      	  // Stop and when restarted transfer keyboard register contents into Aa
	  s->stop = 1;
	  s->on_restart_load_aa = 1;
          s->update_display = 1;
          
          next_iar(s);
          break;
          
        case 9:
      	  // Display
          next_iar(s);

	  // Stop and display (Aa)
	  s->stop = 1;
          break;
#endif
	}
      
      break;

    case 7:
    case 8:
    case 9:
      
#if EXTRACODE_FRAMEWORK
      // Put up stage B display, we have to force this display as we are running in the upper store after
      //entering extracode above.
      //      update_display();

      
      // Set up the extracode framework, if we aren't already in extracode
      // Extracode instructions aren't allowed in extracode subroutines
      if( !IS_EXTRACODE )
        {
          enter_extracode(s);
        }
      else
        {
          // Error
          enter_error_state(s, "Threee address instruction found in extracode subroutine");
        }
#else
      
#endif
      break;
    }

#if DEBUG_STAGES
  printf(" [Stage C: AUXIAR:%03X%s IAR:%03X%s] exit", s->aux_iar.address, s->aux_iar.a_flag?"A":" ", s->iar.address, s->iar.a_flag?"A":" ");
#endif

#if DUMP_STATE_STAGE_C
  cli_dump_state();
#endif

  // Stage C only displayed if we aren't in extracode
  // If we are then it is displayed when we exit
  if( !IS_EXTRACODE )
    {
      stage_c_display(s, display, s->inst_digit_a);
    }

  if( s->exiting_extracode )
    {
#if DEBUG_EXTRACODE
      printf("\nExiting extracode...");
#endif
      
      // This is always displayed, so over-ride display setting
      stage_c_display(s, DISPLAY_UPDATE, s->inst_digit_a);

      // Not exiting any more
      s->exiting_extracode = 0;
    }
  
  FN_EXIT;
  
}

////////////////////////////////////////////////////////////////////////////////
// Stage B
// Decode absolute addresses
// Execute if possible
////////////////////////////////////////////////////////////////////////////////

void stage_b_decode(ESC_STATE *s, int display)
{
  FN_ENTRY_DISPLAY;
  int display_override = display;
  
#if DEBUG_STAGES
  printf(" [Stage B: AUXIAR:%03X%s IAR:%03X%s] ", s->aux_iar.address, s->aux_iar.a_flag?"A":" ", s->iar.address, s->iar.a_flag?"A":" ");
#endif

  switch(s->inst_digit_a)
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      switch(s->inst_digit_b)
	{
        case 8:
      	  // Stop and when restarted transfer keyboard register contents into Aa
          // Only stop if we aren't in an extracode subroutine and running the instruction
          // action. 

          if( s->run, 1 )
            {
              s->stop = 1;
            }

	  s->on_restart_load_aa = 1;
          s->update_display = 1;
          display_override = DISPLAY_UPDATE;
          next_iar(s);
          break;
          
        case 9:
      	  // Display
          next_iar(s);

	  // Stop and display (Aa)
          // If running extracode we don't stop, but use the stage step stops
          if( s->run,1 )
            {
              s->stop = 1;
            }
          display_override = DISPLAY_UPDATE;
          break;

	}
      
      break;
    }
  
  stage_b_display(s, display_override, s->inst_digit_a);
  
  FN_EXIT;
}

////////////////////////////////////////////////////////////////////////////////
//
// Display for stage A
//
////////////////////////////////////////////////////////////////////////////////

// Display varies sightly depending on instruction field a

void stage_a_display(ESC_STATE *s, int display, int a)
{
  // Common Items

  clear_display(s, display);
  
  // Top line clear
  clear_line(s, display, 1);

  display_line_2(s, display);
  clear_line(s, display, 3);
  
  switch(a)
    {
      // Register instructions
    case 0:
      switch(s->inst_digit_b)
	{
	case 0:
	case 1:
	case 2:
	case 3:

        case 5:
        case 6:
        case 7:
	  display_line_2(s, display);
	  clear_lines_3_to_6(s, display);
	  display_any_size_register_on_line(s, display, 3, s->reginst_rc, NO_CONTENTS);
          break;
        }
      
    case 1:
      display_line_2(s, display);
      clear_lines_3_to_6(s, display);
      display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, NO_CONTENTS);
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      switch(s->inst_digit_b)
	{
	case 0:
	case 1:
	case 2:
	  display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->inst_ap, display_store_word(load_from_store(s, s->inst_ap)));
	  display_any_size_register_on_line(s, display, 4, 0, NO_CONTENTS);
          display_any_size_register_on_line(s, display, 5, 1, NO_CONTENTS);
          break;
          
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
	  display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->inst_ap, display_store_word(load_from_store(s, s->inst_ap)));
          break;
        }
      
      break;
      
      // Absolute addressing
    case 7:
      clear_line(s, display, 4);
      clear_line(s, display, 5);
      clear_line(s, display, 6);
      break;
      
      // Relative
    case 8:
      display_any_size_register_on_line(s, display, 4, 3, CONTENTS);
      display_any_size_register_on_line(s, display, 5, 4, CONTENTS);
      display_any_size_register_on_line(s, display, 6, 5, CONTENTS);
      break;

      // Indirect
    case 9:

      display_on_line(s, display, 4, "%2X    %s", s->Ap1, display_store_word(load_from_store(s, s->Ap1)));
      display_on_line(s, display, 5, "%2X    %s", s->Ap2, display_store_word(load_from_store(s, s->Ap2)));
      display_on_line(s, display, 6, "%2X    %s", s->Ap3, display_store_word(load_from_store(s, s->Ap3)));
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Display for stage B
//
////////////////////////////////////////////////////////////////////////////////


void stage_b_display(ESC_STATE *s, int display, int a)
{
  // Common Items
  FN_ENTRY;

#if DEBUG_STAGE_DISPLAY
  printf("\n%s:display:%d  inst_ab:%d%d", __FUNCTION__, display, a, s->inst_digit_b);
#endif
  
  clear_display(s, display);
  
  // Top line clear
  clear_line(s, display, 1);

  display_line_2(s, display);
  clear_line(s, display, 3);
  
  switch(a)
    {
      // Register instructions
    case 0:
      switch(s->inst_digit_b)
        {
        case 5:
	  display_line_2(s, display);
	  clear_lines_3_to_6(s, display);
	  display_any_size_register_on_line(s, display, 3, s->reginst_rc,  CONTENTS);
	  display_on_line(s, display, 6, "CL            %d", s->control_latch);
          break;

        default:
          display_line_2(s, display);
          clear_lines_3_to_6(s, display);
          display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
          break;
        }
      break;
      
    case 1:
      switch(s->inst_digit_b)
        {
        case 9:
          if( IS_EXTRACODE )
            {
              //
              // Display X, Y, Z
              // 
              display_line_2(s, DISPLAY_UPDATE);
              display_on_line(s, DISPLAY_UPDATE, 3, "%3X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
              display_on_line(s, DISPLAY_UPDATE, 4, "%3X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
              display_on_line(s, DISPLAY_UPDATE, 5, "%3X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
              display_on_line(s, DISPLAY_UPDATE, 6, "               ");

            }
          else
            {
              // Display (Rc) and (Rd)
              // 
              display_line_2(s, DISPLAY_UPDATE);
              display_two_any_size_register_on_line(s, DISPLAY_UPDATE, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
            }

          break;

        default:
          display_line_2(s, display);
          clear_lines_3_to_6(s, display);

          display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
          //display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
          break;
        }
      // display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
      // display_any_size_register_on_line(s, display, 3, s->reginst_rd, CONTENTS);
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      switch(s->inst_digit_b)
	{
	case 0:
	case 1:
	case 2:
	  display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->inst_aa, display_store_word(load_from_store(s, s->inst_aa)));
	  display_any_size_register_on_line(s, display, 4, 0, CONTENTS);
          display_any_size_register_on_line(s, display, 5, 1, CONTENTS);
          break;
          
        case 4:
        case 5:
        case 6:
        case 7:
	  display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->inst_aa, display_store_word(load_from_store(s, s->inst_aa)));
          break;

          // Input instruction, in extracode mode needs to display all three TARs
        case 8:
        case 9:
          if( IS_EXTRACODE )
            {
              display_line_2(s, display);
              clear_lines_3_to_6(s, display);
              display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
              display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
              display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
            }
          else
            {
              display_line_2(s, display);
              clear_lines_3_to_6(s, display);
              
              display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
            }
          break;
        }

      break;
      
      // Absolute addressing
    case 7:
      display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
      display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
      display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
      break;
      
      // Relative
    case 8:
      display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
      display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
      display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));

      //display_any_size_register_on_line(s, display, 4, 3, CONTENTS);
      //display_any_size_register_on_line(s, display, 5, 4, CONTENTS);
      //display_any_size_register_on_line(s, display, 6, 5, CONTENTS);
      break;
      
      // Indirect
    case 9:
      display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
      display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
      display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
      break;
    }

  FN_EXIT;
}

////////////////////////////////////////////////////////////////////////////////
//
// Display for stage C
//
////////////////////////////////////////////////////////////////////////////////

void stage_c_display(ESC_STATE *s, int display, int a)
{
  // Common Items

  clear_display(s, display);
  
  // Top line clear
  clear_line(s, display, 1);

  display_line_2(s, display);
  clear_line(s, display, 3);
  
  // A different display if we have just exited an extracode or we are in an extracode subroutine 
  // AUX IAR is used to find the instruction, as that works for the extracodes.
  // the previous stages are from the extracode subroutine instructions for extracodes.
  SINGLE_WORD extracode_inst = load_from_store(s, s->aux_iar.address);
  
  // If stepping extracode then we want to see the subroutine instructions
  
  if( s->exiting_extracode || (IS_EXTRACODE && !setup_step_extracode) )
    {
      // Force inst_digit_a to use the extracode instruction digit a, not the last subroutine
      // instruction digit a
      a = INST_A_FIELD(extracode_inst);

#if DEBUG_EXTRACODE
      printf("\nInst digit A set to %d", a);
#endif
    }

  switch(a)
    {
      // Register instructions
    case 0:
      switch(s->inst_digit_b)
        {
        case 5:
	  display_line_2(s, display);
	  clear_lines_3_to_6(s, display);
	  display_any_size_register_on_line(s, display, 3, s->reginst_rc,  CONTENTS);
	  display_on_line(s, display, 6, "CL            %d", s->control_latch);
          break;

        default:
          display_line_2(s, display);
          clear_lines_3_to_6(s, display);
          display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
          break;
        }
      
      break;
          
    case 1:
      switch(s->inst_digit_b)
        {
        case 9:
                    if( IS_EXTRACODE )
            {
              //
              // Display X, Y, Z
              // 
              display_line_2(s, DISPLAY_UPDATE);
              display_on_line(s, DISPLAY_UPDATE, 3, "%3X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
              display_on_line(s, DISPLAY_UPDATE, 4, "%3X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
              display_on_line(s, DISPLAY_UPDATE, 5, "%3X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
              display_on_line(s, DISPLAY_UPDATE, 6, "               ");

            }
          else
            {
              // Display (Rc) and (Rd)
              // 
              display_line_2(s, DISPLAY_UPDATE);
              display_two_any_size_register_on_line(s, DISPLAY_UPDATE, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
            }

          break;

        default:
          display_line_2(s, display);
          clear_lines_3_to_6(s, display);

          display_two_any_size_register_on_line(s, DISPLAY_UPDATE, 3, s->reginst_rc, s->reginst_rd, CONTENTS);
          //display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
          break;
        }
      // display_any_size_register_on_line(s, display, 3, s->reginst_rc, CONTENTS);
      // display_any_size_register_on_line(s, display, 3, s->reginst_rd, CONTENTS);
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      switch(s->inst_digit_b)
	{
	case 0:
	case 1:
	case 2:
	  display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->inst_aa, display_store_word(load_from_store(s, s->inst_aa)));
	  display_any_size_register_on_line(s, display, 4, 0, CONTENTS);
          display_any_size_register_on_line(s, display, 5, 1, CONTENTS);
          break;
          
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
	  display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->inst_aa, display_store_word(load_from_store(s, s->inst_aa)));
          break;
        }

      break;
      
      // Three address instructions have the three TARs in upper store (100..102) which
      // hold the address of the values X, Y and Z.
      // We can use those to display the results after stage C
#if 0
    case 7:

      //display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
      //display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
      //display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
      //break;
      
      // Relative
    case 8:
      //display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
      //display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
      //display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));

      //display_any_size_register_on_line(s, display, 4, 3, CONTENTS);
      //display_any_size_register_on_line(s, display, 5, 4, CONTENTS);
      //display_any_size_register_on_line(s, display, 6, 5, CONTENTS);
      //break;
      
      // Indirect
    case 9:
      //display_on_line(s, display, 4, "%2X    %s", s->Aa1, display_store_word(load_from_store(s, s->Aa1)));
      //display_on_line(s, display, 5, "%2X    %s", s->Aa2, display_store_word(load_from_store(s, s->Aa2)));
      //display_on_line(s, display, 6, "%2X    %s", s->Aa3, display_store_word(load_from_store(s, s->Aa3)));
      break;
#endif
    case 7:
    case 8:
    case 9:
      display_on_line(s, display, 4, "%2X    %s", load_from_store(s, TAR1), display_store_word(load_from_store(s, load_from_store(s, TAR1))));
      display_on_line(s, display, 5, "%2X    %s", load_from_store(s, TAR2), display_store_word(load_from_store(s, load_from_store(s, TAR2))));
      display_on_line(s, display, 6, "%2X    %s", load_from_store(s, TAR3), display_store_word(load_from_store(s, load_from_store(s, TAR3))));
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Fetch instruction
// Decode presumptive addresses
//
////////////////////////////////////////////////////////////////////////////////

void stage_a_decode(ESC_STATE *s, int display)
{
  FN_ENTRY_DISPLAY;
  
#if DEBUG_FN_CALL
  printf("\n%s:display=%d", __FUNCTION__, display);
#endif
  
#if DEBUG_STAGES
  printf("\n [Stage A: AUXIAR:%03X%s IAR:%03X%s] ", s->aux_iar.address, s->aux_iar.a_flag?"A":" ", s->iar.address, s->iar.a_flag?"A":" ");
#endif
  
  if( s->iar.a_flag )
    {
      s->inst_digit_a = INST_E_FIELD(s->instruction_register);
      s->inst_digit_b = INST_F_FIELD(s->instruction_register);
      s->inst_digit_c = INST_G_FIELD(s->instruction_register);
      s->inst_digit_d = INST_H_FIELD(s->instruction_register);
    }
  else
    {
      s->inst_digit_a = INST_A_FIELD(s->instruction_register);
      s->inst_digit_b = INST_B_FIELD(s->instruction_register);
      s->inst_digit_c = INST_C_FIELD(s->instruction_register);
      s->inst_digit_d = INST_D_FIELD(s->instruction_register);
    }


#if DEBUG_A_DECODE
  printf("\n***  abcd = %d%d%d%d  ***  ",
	 s->inst_digit_a,
	 s->inst_digit_b,
	 s->inst_digit_c,
	 s->inst_digit_d);
#endif

  // Check here for a value in the store location, i.e. a sign
  // Error if we see one
  switch(s->inst_digit_a)
    {
    case WORD_SIGN_PLUS:
    case WORD_SIGN_MINUS:
    case WORD_SIGN_NONE:       // No sign is still a bad value for an instruction
      enter_error_state(s, "Expecting instruction");
      break;
    }
  
  // Pre-calculate the Ap field of the instruction for some later
  // decoding

  s->inst_ap = (s->inst_digit_c) * 16 + (s->inst_digit_d);

  // If an extracode then we refer to the upper half of store
  if( IS_EXTRACODE )
    {
      s->inst_ap += 0x100;
    }

#if DEBUG_A_DECODE
  printf("   inst_ap: %04X\n", s->inst_ap);
#endif

  // Calculate the presumptive addresses
  
  switch(s->inst_digit_a)
    {
    case 0:
      switch(s->inst_digit_b)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	  s->reginst_rc = s->inst_digit_c;
	  s->reginst_literal = s->inst_digit_d;

	  display_line_2(s, display);
	  clear_lines_3_to_6(s, display);
	  display_any_size_register_on_line(s, display, 3, s->reginst_rc, NO_CONTENTS);
	  
	  break;
	  
	case 4:
	  // Not used
          enter_error_state(s, "04xx not used");
	  break;
	  
	case 5:
	  // TEST
	  // Performed in stage C
	  s->reginst_rc = s->inst_digit_c;

	  display_line_2(s, display);
	  clear_lines_3_to_6(s, display);
	  display_any_size_register_on_line(s, display, 3, s->reginst_rc, NO_CONTENTS);
	  break;

	case 6:
	case 7:
	  s->reginst_rc = s->inst_digit_c;
	  s->reginst_rd = s->inst_digit_d;
	  s->reginst_literal = s->inst_digit_d;	  

	  display_line_2(s, display);
	  clear_lines_3_to_6(s, display);
	  display_any_size_register_on_line(s, display, 3, s->reginst_rc, NO_CONTENTS);
	  break;
          
        case 8:
          enter_error_state(s, "08xx not used");
          break;

        case 9:
          enter_error_state(s, "09xx not used");
          break;
	}
      break;
      
    case 1:
      switch(s->inst_digit_b)
	{
	case 5:
          enter_error_state(s, "15xx not used");    
          break;
          
        case 8:
          enter_error_state(s, "18xx not used");
          break;
          
        default:
          
          // Register instructions
          s->reginst_rc = s->inst_digit_c;
          s->reginst_rd = s->inst_digit_d;
          
          display_line_2(s, display);
          clear_lines_3_to_6(s, display);
          clear_lines_3_to_6(s, display);
          display_two_any_size_register_on_line(s, display, 3, s->reginst_rc, s->reginst_rd, NO_CONTENTS);
          
          break;
        }
      break;
      
    case 2:
      switch(s->inst_digit_b)
        {
        case 3:
          break;

        default:
          // Absolute addressing
          s->inst_aa = s->inst_ap;
          s->inst_aa = BOUND_ADDRESS(s->inst_aa);
          
          s->reginst_rc = s->inst_digit_c;
          s->reginst_rd = s->inst_digit_d;
          
          display_line_2(s, display);
          display_on_line(s, display, 3, "               ");
          display_on_line(s, display, 4, "%02X", s->inst_aa);
          display_on_line(s, display, 5, "               ");
          display_on_line(s, display, 6, "               ");
          break;
        }
      break;

      // relative addressing
    case 3:
      switch(s->inst_digit_b)
        {
        case 3:
          break;
          
        default:
          s->inst_aa = REMOVED_SW_SIGN(bcd_sw_addition(s, SET_SW_SIGN(s->inst_ap, WORD_SIGN_PLUS), read_register(s, 3)));
          s->inst_aa = BOUND_ADDRESS(s->inst_aa);
          
          s->reginst_rc = s->inst_digit_c;
          s->reginst_rd = s->inst_digit_d;
          
          display_line_2(s, display);
          display_on_line(s, display, 3, "%02X", s->inst_ap);
          display_on_line(s, display, 4, "               ");
          display_on_line(s, display, 5, "               ");
          display_on_line(s, display, 6, "               ");
          break;
        }
      break;
      
    case 4:
      switch(s->inst_digit_b)
        {
        case 3:
          break;
          
        default:
          s->inst_aa = REMOVED_SW_SIGN(bcd_sw_addition(s, SET_SW_SIGN(s->inst_ap, WORD_SIGN_PLUS), read_register(s,4)));
          s->inst_aa = BOUND_ADDRESS(s->inst_aa);
                    
          display_line_2(s, display);
          display_on_line(s, display, 3, "               ");
          display_on_line(s, display, 4, "%02X", s->inst_aa);
          display_on_line(s, display, 5, "               ");
          display_on_line(s, display, 6, "               ");
          break;
        }
      break;
      
    case 5:
      switch(s->inst_digit_b)
        {
        case 3:
          break;
          
        default:
          s->inst_aa = REMOVED_SW_SIGN(bcd_sw_addition(s, SET_SW_SIGN(s->inst_ap, WORD_SIGN_PLUS), read_register(s, 5)));
          s->inst_aa = BOUND_ADDRESS(s->inst_aa);
                    
          display_line_2(s, display);
          display_on_line(s, display, 3, "%02X", s->inst_aa);
          display_on_line(s, display, 4, "               ");
          display_on_line(s, display, 5, "               ");
          display_on_line(s, display, 6, "               ");
          break;
        }
      break;
      
    case 6:
      switch(s->inst_digit_b)
        {
        case 3:
          break;
          
        default:
          // Indirect addressing
          // Indirect addressing uses the hundreds digit of the IAR. This is to ensure that
          // the addressing works correctly when used in an extracode instruction.
#if DEBUG_ADDR_MODES
          printf("\nap:%08X", s->inst_ap);
#endif
          
          s->inst_aa = load_from_store(s, REMOVED_SW_SIGN(s->inst_ap));
          s->inst_aa = BOUND_ADDRESS(s->inst_aa);
          
#if DEBUG_ADDR_MODES
          printf("\naa:%08X", s->inst_aa);
#endif
          display_line_2(s, display);
          display_on_line(s, display, 3, "%02X", s->inst_aa);
          display_on_line(s, display, 4, "               ");
          display_on_line(s, display, 5, "               ");
          display_on_line(s, display, 6, "               ");
          break;
        }
      break;
      
      // 3 address instructions
      
    case 7:
    case 8:
    case 9:

      // Three address instructions can be executed as extracode,
      // or coded instructions
      
      if (s->inst_digit_a == 7 )
        {
          
          // Absolute
          s->Ap1 = INST_3_ADDR_1(s->instruction_register);
          s->Ap2 = INST_3_ADDR_2(s->instruction_register);
          s->Ap3 = INST_3_ADDR_3(s->instruction_register);
          
          s->Aa1 = convert_store_to_address(s->Ap1);
          s->Aa2 = convert_store_to_address(s->Ap2);
          s->Aa3 = convert_store_to_address(s->Ap3);

          
          display_line_2(s, display);
          display_on_line(s, display, 3, "%2X", s->Ap1);
          display_on_line(s, display, 4, "%2X", s->Ap2);
          display_on_line(s, display, 5, "%2X", s->Ap3);
          display_on_line(s, display, 6, "     YY        ");
        }
      
      if( s->inst_digit_a == 8)
        {
          // Relative
          s->Ap1 = INST_3_ADDR_1(s->instruction_register);
          s->Ap2 = INST_3_ADDR_2(s->instruction_register);
          s->Ap3 = INST_3_ADDR_3(s->instruction_register);

          // ** TODO Look at this addition, shoulkd it be BCD?
#if 0
          s->Aa1 = convert_store_to_address(s->Ap1 + read_register(s, 3)s->R3); 
          s->Aa2 = convert_store_to_address(s->Ap2 + s->R4); 
          s->Aa3 = convert_store_to_address(s->Ap3 + s->R5); 
#endif
          s->Aa1 = convert_store_to_address(s->Ap1 + read_register(s, 3)); 
          s->Aa2 = convert_store_to_address(s->Ap2 + read_register(s, 4)); 
          s->Aa3 = convert_store_to_address(s->Ap3 + read_register(s, 5)); 

          display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->Ap1, display_store_word(s->Ap1));
          display_on_line(s, display, 4, "%2X    %s", s->Ap2, display_store_word(s->Ap2));
          display_on_line(s, display, 5, "%2X    %s", s->Ap3, display_store_word(s->Ap3));
          display_on_line(s, display, 6, "               ");
        }
      

      if( s->inst_digit_a == 9)
        {
          // Indirect
          s->Ap1 = INST_3_ADDR_1(s->instruction_register);
          s->Ap2 = INST_3_ADDR_2(s->instruction_register);
          s->Ap3 = INST_3_ADDR_3(s->instruction_register);
          
          s->Aa1 = convert_store_to_address(load_from_store(s, s->Ap1));
          s->Aa2 = convert_store_to_address(load_from_store(s, s->Ap2));
          s->Aa3 = convert_store_to_address(load_from_store(s, s->Ap3));
#if 0
          display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %s", s->Ap1, display_store_word(s->Ap1));
          display_on_line(s, display, 4, "%2X    %s", s->Ap2, display_store_word(s->Ap2));
          display_on_line(s, display, 5, "%2X    %s", s->Ap3, display_store_word(s->Ap3));
          display_on_line(s, display, 6, "               ");
#endif
#if 1
          display_line_2(s, display);
          display_on_line(s, display, 3, "%2X    %02X", s->Ap1, s->Aa1);
          display_on_line(s, display, 4, "%2X    %02X", s->Ap2, s->Aa2);
          display_on_line(s, display, 5, "%2X    %02X", s->Ap3, s->Aa3);
          display_on_line(s, display, 6, "               ");
#endif
        }
      break;
    }

  // Display stage A information
  stage_a_display(s, display, s->inst_digit_a);
  FN_EXIT;
}

////////////////////////////////////////////////////////////////////////////////

// Execution
// Basic instructions are stored two to a word
// The two instructions are processed, in turn, digit position 1-4
// then digit position 5-8
// Three-address instructions are stored one to a word
//
// Instruction to be processed is in instruction register
// If this is a KI reset then the IAR should not be updated
//
// Display is updated on a per-instruction basis as the display format
// is different for each instruction

void run_stage_a(ESC_STATE *s, int display)
{
  // Set instruction stage
  s->stage = 'A';

  if( !(s->ki_reset_flag) )
    {
      // load aux iar
      if( !IS_EXTRACODE || setup_step_extracode )
        {
          // Only load during non-extracode execution, as the stage display of extracodes requires this
          // to show the extracode IAR not the subroutine instruction values.
          s->aux_iar = s->iar;
        }
    }
    
  // Decode instruction
  stage_a_decode(s, display);

  //s->update_display = display;
}

//
// Stage B
//
// The addresses have been calculated, now the instruction is executed
//

void run_stage_b(ESC_STATE *s, int display)
{
  // Set instruction stage
  s->stage = 'B';

  //s->update_display = display;
  stage_b_decode(s, display);
}

void run_stage_c(ESC_STATE *s, int display)
{
  // Set instruction stage
  s->stage = 'C';
  //s->update_display = display;
  
  stage_c_decode(s, display);
}

////////////////////////////////////////////////////////////////////////////////
//
// State machines
//
////////////////////////////////////////////////////////////////////////////////

void null_entry_fn(FSM_DATA *s, TOKEN tok)
{
}

void null_every_fn(FSM_DATA *s, TOKEN tok)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// Load ADDR from KBD
// Address needs to be limited to valid range
//
////////////////////////////////////////////////////////////////////////////////

void state_esc_load_addr(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;

  s = (ESC_STATE *)fs;

  s->address_register = BOUND_ADDRESS(s->keyboard_register);

  clear_keyboard_register(s);
  
  display_on_line(s, DISPLAY_UPDATE, 6, "%s   %s", display_address(s->address_register), display_store_word(load_from_store(s, s->address_register)));

  s->update_display = 1;
}

// Load store from KBD
void state_esc_load_store(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;

  s = (ESC_STATE *)fs;

  write_sw_to_store(s, s->address_register, s->keyboard_register);

  clear_keyboard_register(s);
    
  display_on_line(s, DISPLAY_UPDATE, 6, "%s   %s", display_address(s->address_register), display_store_word(load_from_store(s, s->address_register)));
  
  s->update_display = 1;
}

void state_esc_incr_addr(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;
  REGISTER_SINGLE_WORD one;
  
  s = (ESC_STATE *)fs;

  one = 1;
  one = SET_SW_SIGN(one, WORD_SIGN_PLUS);

  s->address_register = BOUND_ADDRESS(bcd_sw_addition(s, s->address_register, one));

  display_on_line(s, DISPLAY_UPDATE, 6, "%s   %s", display_address(s->address_register), display_store_word(load_from_store(s, s->address_register)));
  
  s->update_display = 1;
}

void state_esc_decr_addr(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;
  REGISTER_SINGLE_WORD minus_1;
  
  s = (ESC_STATE *)fs;

  minus_1 = 1;
  minus_1 = SET_SW_SIGN(minus_1, WORD_SIGN_MINUS);

  s->address_register = (BOUND_ADDRESS(bcd_sw_addition(s, s->address_register, minus_1)));

  display_on_line(s, DISPLAY_UPDATE, 6, "%s   %s", display_address(s->address_register), display_store_word(load_from_store(s, s->address_register)));
  
  s->update_display = 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// Load IAR from KBD
// This is a load so we don't preserve the upper digit. If extracode execution is required then it will
// work.

void state_esc_load_iar(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;

  s = (ESC_STATE *)fs;

  s->iar.address = (ADDRESS)s->keyboard_register;
  s->iar.a_flag = 0;

  clear_keyboard_register(s);

  
  display_on_line(s, DISPLAY_UPDATE, 1, "%02s           ", display_iar(s, SPEC_FORCE_IAR));

  s->update_display = 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// Clear keyboard register and associated flags
//
////////////////////////////////////////////////////////////////////////////////

void clear_keyboard_register(ESC_STATE *s)
{
  s->keyboard_register = 0;
  s->dot_entered = 0;
  
  display_on_line(s, DISPLAY_UPDATE, 1, "%02s           ", display_iar(s, SPEC_IAR));
}

////////////////////////////////////////////////////////////////////////////////
//
// Add digit to the keybord register
//
////////////////////////////////////////////////////////////////////////////////

// Preserve sign

void state_esc_numeric(FSM_DATA *fd, TOKEN tok)
{
  int num = tok - TOK_KEY_0;
  
  ESC_STATE *s;
  s = (ESC_STATE *)fd;
  
  SINGLE_WORD kbr = s->keyboard_register;
  SINGLE_WORD digits, old_digits;
  int sign;
  int exp;

  digits = STORE_GET_DIGITS(kbr);
  exp    = STORE_GET_EXPONENT(kbr);
  sign   = STORE_GET_SIGN(kbr);

  // If there is a sign then update as a floating point number otherwise as an integer
  if( (sign == WORD_SIGN_PLUS) || (sign == WORD_SIGN_MINUS) || (s->dot_entered) )
    {
      // Update the digits, leaving the exponent and the sign unchanged
      old_digits = digits;
      digits *= 16;
      digits += num;

      //@printf("\ndigits=%08X", digits);
      
      // If we have a non zero exponent then we increment it as we have added a fractional digit
      if( (exp > 0) || (s->dot_entered) )
	{
	  exp++;
	  if( exp == 7)
	    {
	      digits = old_digits;
	      exp = 6;
	    }
	}
      
      // Rebuild the register contents
      kbr = STORE_SET_SIGN    (kbr,sign);
      kbr = STORE_SET_EXPONENT(kbr,exp);
      kbr = STORE_SET_DIGITS  (kbr,digits);
    }
  else
    {
      // Instruction (integer)
      kbr *= 16;
      kbr += num;
    }
      
  s->keyboard_register = kbr;
  
  display_on_line(s, DISPLAY_UPDATE, 1, "%02s %8s", display_iar(s, SPEC_IAR), display_store_word(s->keyboard_register));
  
  s->update_display = 1;
}

//------------------------------------------------------------------------------
//
// Dot pressed, so set the exponent digit
//
// Set sign to positive if it isn't set
//

void state_esc_dot(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;
  char line[20];
  
  s = (ESC_STATE *)fs;

  // Set dot entered flag
  s->dot_entered = 1;

  // Set exponent
  s->keyboard_register = STORE_SET_EXPONENT(s->keyboard_register, 0);

  // Sign set to plus
  // Minus key will over-ride at the end of entry
  switch(SW_SIGN(s->keyboard_register))
    {
    case WORD_SIGN_PLUS:
    case WORD_SIGN_MINUS:
      break;

    default:
      s->keyboard_register = STORE_SET_SIGN(s->keyboard_register, WORD_SIGN_PLUS);
      break;
    }

  sprintf(line, "%02s %8s", display_iar(s, SPEC_IAR), display_store_word(s->keyboard_register));
  display_on_line(s, 1, DISPLAY_UPDATE, line);

  s->update_display = 1;
}

// Force sign negative

void state_esc_minus(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;
  char line[20];
  
  s = (ESC_STATE *)fs;

  s->keyboard_register = SET_SW_SIGN(s->keyboard_register, WORD_SIGN_MINUS);

  sprintf(line, "%02s %8s", display_iar(s, SPEC_IAR), display_store_word(s->keyboard_register));
  display_on_line(s, 1, DISPLAY_UPDATE, line);

  s->update_display = 1;

}


////////////////////////////////////////////////////////////////////////////////

void state_esc_clear(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;

  s = (ESC_STATE *)fs;

  clear_keyboard_register(s);
  
  // Re-display
  s->update_display = 1;
}

void state_esc_normal_reset(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;
  char line[20];
  
  s = (ESC_STATE *)fs;

  // Everything cleared except IAR

  s->stage = ' ';
  clear_keyboard_register(s);

  
  s->extracode         = 0;
  s->exiting_extracode = 0;
  
  s->ki_reset_flag     = 0;
  s->error             = 0;

#if 0
  s->address_register0 = EMPTY_ADDRESS;
  s->address_register1 = EMPTY_ADDRESS;
  s->address_register2 = EMPTY_ADDRESS;
#endif
  
#if OLED_ON
  oled_clear_display(&oled0);
#endif
  
  display_on_line(s, DISPLAY_UPDATE, 1, "%02s             ", display_iar(s, SPEC_FORCE_IAR));
  display_on_line(s, DISPLAY_UPDATE, 2, "               ");
  display_on_line(s, DISPLAY_UPDATE, 3, "               ");
  display_on_line(s, DISPLAY_UPDATE, 4, "               ");
  display_on_line(s, DISPLAY_UPDATE, 5, "               ");
  display_on_line(s, DISPLAY_UPDATE, 6, "               ");
  
  s->reginst_rc = NO_VALUE;
  s->reginst_rd = NO_VALUE;
  s->reginst_literal = NO_VALUE;
  
  // Re-display
  s->update_display = 1;
}

void state_esc_ki_reset(FSM_DATA *fs, TOKEN tok)
{
  ESC_STATE *s;

  s = (ESC_STATE *)fs;

  // Everything cleared except IAR

  // Unreachable address
  s->breakpoint.address = 0xFFF;
  s->breakpoint.a_flag  = 0;

  s->stage = ' ';
  s->keyboard_register = 0x00;
  s->dot_entered = 0;
  
  s->ki_reset_flag = 1;
#if 0
  s->address_register0 = EMPTY_ADDRESS;
  s->address_register1 = EMPTY_ADDRESS;
  s->address_register2 = EMPTY_ADDRESS;
#endif
  
  s->reginst_rc = NO_VALUE;
  s->reginst_rd = NO_VALUE;
  s->reginst_literal = NO_VALUE;

  // Keyboard reset executes upper instruction first
  s->aux_iar.a_flag = 0;
  s->iar.a_flag = 0;
  
  display_on_line(s, DISPLAY_UPDATE, 1, "%02s             ", display_iar(s, SPEC_IAR));
  display_on_line(s, DISPLAY_UPDATE, 2, "K              ");
  display_on_line(s, DISPLAY_UPDATE, 3, "               ");
  display_on_line(s, DISPLAY_UPDATE, 4, "               ");
  display_on_line(s, DISPLAY_UPDATE, 5, "               ");
  display_on_line(s, DISPLAY_UPDATE, 6, "               ");

  // Re-display
  s->update_display = 1;
}

//
// Set up the instruction register ready to execute the instrution in it
// KI reset handles things differently as the instructionm comes from there,
// not where the IAR is pointing
//

void prepare_instruction(ESC_STATE *s)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif

#if DEBUG_PREPARE
  printf("\n%s:ki_reset:%d", __FUNCTION__, s->ki_reset_flag);
#endif
  
  if( s->ki_reset_flag && !IS_EXTRACODE )
    {
      s->instruction_register = s->keyboard_register;
    }
  else
    {
      // Load instruction into instruction register
      s->instruction_register = load_from_store(s, s->iar.address);
#if DEBUG_PREPARE
      printf("\n%s:iar:%08X", __FUNCTION__, s->iar.address);
      //cli_dump_store();
#endif

    }

#if DEBUG_PREPARE
  printf("\n%s:instruction register:%08X", __FUNCTION__, s->instruction_register);
#endif

}

//------------------------------------------------------------------------------

void state_esc_a_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif

  s = (ESC_STATE *)es;

  prepare_instruction(s);
      
  switch(s->stage)
    {
    case ' ':
      run_stage_a(s, display_flag);
      break;

    case 'A':
      break;
      
    case 'B':
      break;

      // If in stage C then next instruction is ready to go
    case 'C':
      prepare_instruction(s);
      run_stage_a(s, display_flag);
      break;
    }
}


////////////////////////////////////////////////////////////////////////////////
//
// Run stage a
//
// Called from A key
//

void state_esc_a_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif
  state_esc_a_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_a_no_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif

  // The extracode instructions do not display their stage data, by default.
  // This can be tuirned on if the setup wants stepping of subroutine instructions
  
  state_esc_a_core(es, tok, DISPLAY_NO_UPDATE);
}

//------------------------------------------------------------------------------

void state_esc_b_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif

  s = (ESC_STATE *)es;
  //s->update_display = display_flag;
  
  switch(s->stage)
    {
    case ' ':
      prepare_instruction(s);
      run_stage_a(s, display_flag);
      run_stage_b(s, display_flag);
      break;

    case 'A':
      run_stage_b(s, display_flag);
      break;

    case 'B':
      break;

    case 'C':
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Run stage b
//
// Called from B key
//

void state_esc_b_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif
  state_esc_b_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_b_no_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif

  state_esc_b_core(es, tok, DISPLAY_NO_UPDATE);
}

//------------------------------------------------------------------------------

void state_esc_c_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;

#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif

  s = (ESC_STATE *)es;

  //  s->update_display = display_flag;

  s->stop = 0;
  
  switch(s->stage)
    {
    case ' ':
      prepare_instruction(s);
      
      run_stage_a(s, display_flag);

      if( !s->stop )
        {
          run_stage_b(s, display_flag);
        }
      
      if( !s->stop )
        {
          run_stage_c(s, display_flag);
        }
      break;

    case 'A':
      run_stage_b(s, display_flag);
      run_stage_c(s, display_flag);
      break;

    case 'B':
      run_stage_c(s, display_flag);
      break;

    case 'C':
      prepare_instruction(s);
      run_stage_a(s, display_flag);

      if( !s->stop )
        {
          run_stage_b(s, display_flag);
        }
      if( !s->stop )
        {
          run_stage_c(s, display_flag);
        }
      break;
    }
}

void state_esc_next_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;

#if DEBUG_FN_CALL
  printf("\n%s display=%d", __FUNCTION__, display_flag);
#endif

  s = (ESC_STATE *)es;

  switch(s->stage)
    {
    case ' ':
      prepare_instruction(s);
      
      run_stage_a(s, display_flag);
      //      run_stage_b(s, display_flag);
      //run_stage_c(s, display_flag);
      break;

    case 'A':
      run_stage_b(s, display_flag);
      //run_stage_c(s, display_flag);
      break;

    case 'B':
      run_stage_c(s, display_flag);
      break;

    case 'C':
      prepare_instruction(s);
      run_stage_a(s, display_flag);

      //run_stage_b(s, display_flag);
      //run_stage_c(s, display_flag);
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Run stage c
//
// Called from C key
//

void state_esc_c_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif
  state_esc_c_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_c_no_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif
  state_esc_c_core(es, tok, DISPLAY_NO_UPDATE);
}

void state_esc_next_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif
  state_esc_next_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_next_no_disp(FSM_DATA *es, TOKEN tok)
{
#if DEBUG_FN_CALL
  printf("\n%s", __FUNCTION__);
#endif
  state_esc_next_core(es, tok, DISPLAY_NO_UPDATE);
}

//------------------------------------------------------------------------------

void state_esc_run(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  FN_ENTRY;

  s->extracode_run = 0;
  s->run           = 1;
  s->stop          = 0;

  FN_EXIT;
}

void state_esc_stop(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  FN_ENTRY;
  
  s->stop = 1;

  FN_EXIT;
}

////////////////////////////////////////////////////////////////////////////////
//
// Run at full speed
//
////////////////////////////////////////////////////////////////////////////////
//
// This is called when no keypresses are detectd, so it is an idle loop
//
//

void state_esc_execute(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  // If just started running check for input loads
  if( s->run && (!s->last_run) )
    {
#if DEBUG_RESTART
      printf("\n%s:", __FUNCTION__);
#endif
    }

  s->last_run = s->run;
  
  // If not running then exit, nothing to do
  // Being in the extracode is treated as running, so we execute the extracode subroutine as
  // as block of code. This could be extended to allow stepping of the extracode as a feature, later
  
  if( (s->run || s->extracode_run) || (IS_EXTRACODE && !setup_step_extracode) )
    {
#if DEBUG_EXECUTE
      printf("  EXEC:RUN(%s%c)", display_iar(s, SPEC_IAR), s->stage);
#endif
#if 1
      // Check for breakpoint
      if( s->breakpoint_on && (s->breakpoint.address == s->iar.address) && (s->breakpoint.a_flag == s->iar.a_flag) )
        {
          if( (s->stop != 1) && (s->run != 0) )
            {
              printf("\n**** Breakpoint at %s ****", display_iar(s, SPEC_BREAKPOINT));
              s->stop = 1;
              s->run = 0;
            }
        }
#endif 
      // Check for stop
      if( s->stop )
	{
#if DEBUG_EXECUTE
	  printf("\nEXEC:STOP");
#endif
	  // Turn off the execution

	  // Has the program stopped? A test may be waiting for this
	  if( test_waiting_for_stop )
	    {
#if DEBUG_TEST_SEQ
	      printf("\n**Test continuing due to program STOP and test was waiting for this");
#endif
	      test_waiting_for_stop = 0;

	      // Reset counter so test will continue on next time slot.
	      test_loop_count = 0;
	    }

	  // As we run to stage C for each instruction this stops at the end
	  // of stage C for the current instruction.
	  s->run = 0;
	  //s->stop = 0;
	  return;
	}

#if 0
      // Run to end of stage C repeatedly
      state_esc_c_no_disp(s, tok);
#endif

      // Run next stage in sequence
#if 1
      state_esc_next_no_disp(s, tok);
#endif
    }
  else
    {
      // Not running
    }

}

////////////////////////////////////////////////////////////////////////////////
//
// Scans the directory and works out the next file number to use
//
// Leaves max_filenum with that value
//
////////////////////////////////////////////////////////////////////////////////

#define ESC_FILE_NAME_PRINT_FMT   "state%05d.esc"
#define ESC_FILE_NAME_SCAN_FMT    "state%d.esc"
#define ESC_FILE_NAME_GLOB        "state*.esc"

// Write a state file to the next available file name

void state_esc_dump(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  char filename[MAX_LINE];
  
  // Get next file number
  find_next_file_number(ESC_DIR, ESC_FILE_NAME_SCAN_FMT, ESC_FILE_NAME_PRINT_FMT, ESC_FILE_NAME_GLOB);
  sprintf(filename, ESC_FILE_NAME_PRINT_FMT, max_filenum+1);
  
  write_state_to_file(s, filename);

  printf("\nWritten state to '%s", filename);
}

////////////////////////////////////////////////////////////////////////////////
//
// Reload a state file.
// Display all state files and allow the user to pick one.
//
//------------------------------------------------------------------------------

void state_esc_reload(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  
  s->reload_file_first = 0;
  s->reload_display = 1;
  s->update_display = 1;
  s->setup_display  = 0;
}

void state_reload_incr(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  
  s->reload_file_first++;
  s->update_display = 1;
}

void state_reload_decr(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  
  if( s->reload_file_first > 0 )
    {
      s->reload_file_first--;
    }
  
  s->update_display = 1;
}

void state_reload_clear(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  s->reload_display = 0;
  s->update_display = 1;
  s->setup_display  = 0;
}

void state_reload_delete(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  s->delete_display = 1;
  s->reload_display = 0;
  s->update_display = 1;
  s->setup_display  = 0;
}

void state_reload_reload(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  read_file_into_state(&(file_list_data[0][0]), s);
  
#if OLED_ON
  oled_clear_display(&oled0);
#endif

  s->delete_display = 0;
  s->reload_display = 0;
  s->update_display = 1;
  s->setup_display  = 0;
}

void state_delete_do_delete(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  
  delete_file( ESC_DIR, &(file_list_data[0][0]) );	\

  s->delete_display = 0;
  s->reload_display = 1;
  s->update_display = 1;
  s->setup_display  = 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Setup Menu
// As the CHECK key isn't really needed, it is used for a setup menu
//
//------------------------------------------------------------------------------

typedef struct {
  char *title;
  int *value_var;
  char *val0;
  char *val1;
} SETUP_ENTRY;

int suppressed_display = 0;
int setup_step_extracode = 0;

SETUP_ENTRY setup_entries[] =
  {
    {"Numbers  : ", &suppressed_display,   "Orig", "Full"},
    {"Extracode: ", &setup_step_extracode, "Step", "Run "},
  };

#define NUM_SETUP_ENTRIES (sizeof(setup_entries)/sizeof(SETUP_ENTRY))

void state_esc_setup(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  
  s->reload_file_first = 0;
  s->reload_display = 0;
  s->update_display = 1;
  s->setup_display  = 1;
}

void state_setup_val0(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  *(setup_entries[s->reload_file_first].value_var) = 0;
  s->update_display = 1;
}

void state_setup_val1(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  *(setup_entries[s->reload_file_first].value_var) = 1;
  s->update_display = 1;
}

void state_setup_incr(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  if( s->reload_file_first < (NUM_SETUP_ENTRIES - 1 ) )
    {
      s->reload_file_first++;
    }
  s->update_display = 1;
}

void state_setup_decr(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;
  
  if( s->reload_file_first > 0 )
    {
      s->reload_file_first--;
    }
  
  s->update_display = 1;
}

void state_setup_clear(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  s->reload_display = 0;
  s->setup_display  = 0;
  s->update_display = 1;
}

// The variables are updated when they are changed in the setup display
// so this is just an exit.

void state_setup_select(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  s->delete_display = 0;
  s->reload_display = 0;
  s->setup_display  = 0;
  s->update_display = 1;

#if OLED_ON
  oled_clear_display(&oled0);
#endif

}

////////////////////////////////////////////////////////////////////////////////
//
// 
//
////////////////////////////////////////////////////////////////////////////////

STATE esc_table[ ] =
  {
    {
      _STATE(STATE_ESC_INIT),
      null_entry_fn,
      null_every_fn,
      {
        {CTOK_NUMERIC,         STATE_ESC_INIT,   state_esc_numeric},

        {TOK_KEY_DOT,          STATE_ESC_INIT,   state_esc_dot},
        {TOK_KEY_MINUS,        STATE_ESC_INIT,   state_esc_minus},
     
        {TOK_KEY_NORMAL_RESET, STATE_ESC_INIT,   state_esc_normal_reset},
        {TOK_KEY_KI_RESET,     STATE_ESC_INIT,   state_esc_ki_reset},
        {TOK_KEY_LOAD_IAR,     STATE_ESC_INIT,   state_esc_load_iar},
        {TOK_KEY_LOAD_ADDR,    STATE_ESC_INIT,   state_esc_load_addr},
        {TOK_KEY_INCR_ADDR,    STATE_ESC_INIT,   state_esc_incr_addr},
        {TOK_KEY_DECR_ADDR,    STATE_ESC_INIT,   state_esc_decr_addr},
        {TOK_KEY_LOAD_STORE,   STATE_ESC_INIT,   state_esc_load_store},
        {TOK_KEY_A,            STATE_ESC_INIT,   state_esc_a_disp},
        {TOK_KEY_B,            STATE_ESC_INIT,   state_esc_b_disp},
        {TOK_KEY_C,            STATE_ESC_INIT,   state_esc_c_disp},
        {TOK_KEY_RUN,          STATE_ESC_INIT,   state_esc_run},
        {TOK_KEY_STOP,         STATE_ESC_INIT,   state_esc_stop},
        {TOK_KEY_DUMP,         STATE_ESC_INIT,   state_esc_dump},
        {TOK_KEY_RELOAD,       STATE_ESC_RELOAD, state_esc_reload},
        {TOK_KEY_SETUP,        STATE_ESC_SETUP,  state_esc_setup},
        {TOK_KEY_CLEAR,        STATE_ESC_INIT,   state_esc_clear},
     
        // Execute code at full speed
        {TOK_NO_TOK,           STATE_ESC_INIT,  state_esc_execute},
        {CTOK_ERROR,           STATE_ESC_INIT,  NULL},
        {CTOK_END,             STATE_NULL,      NULL},
      }
    },
    {
      _STATE(STATE_ESC_RELOAD),
      null_entry_fn,
      null_every_fn,
      {
        {TOK_KEY_INCR_ADDR,    STATE_ESC_RELOAD,  state_reload_incr},
        {TOK_KEY_DECR_ADDR,    STATE_ESC_RELOAD,  state_reload_decr},
        {TOK_KEY_CLEAR,        STATE_ESC_DELETE,  state_reload_delete},
        {TOK_KEY_RELOAD,       STATE_ESC_INIT,    state_reload_reload},
        {TOK_KEY_KI_RESET,     STATE_ESC_INIT,    state_reload_clear},
        {CTOK_END,             STATE_NULL,        NULL},
      }
    },
    {
      _STATE(STATE_ESC_SETUP),
      null_entry_fn,
      null_every_fn,
      {
        {TOK_KEY_INCR_ADDR,    STATE_ESC_SETUP,   state_setup_incr},
        {TOK_KEY_DECR_ADDR,    STATE_ESC_SETUP,   state_setup_decr},
        {TOK_KEY_0,            STATE_ESC_SETUP,   state_setup_val0},
        {TOK_KEY_1,            STATE_ESC_SETUP,   state_setup_val1},
        {TOK_KEY_SETUP,        STATE_ESC_INIT,    state_setup_select},
        //     {TOK_KEY_KI_RESET,     STATE_ESC_INIT,    state_setup_clear},
        {CTOK_END,             STATE_NULL,        NULL},
      }
    },
    {
      _STATE(STATE_ESC_DELETE),
      null_entry_fn,
      null_every_fn,
      {
        {TOK_KEY_NORMAL_RESET, STATE_ESC_RELOAD,  state_delete_do_delete},
        {TOK_KEY_CLEAR,        STATE_ESC_RELOAD,  state_reload_clear},
        {TOK_KEY_KI_RESET,     STATE_ESC_RELOAD,  state_reload_clear},
        {CTOK_END,             STATE_NULL,        NULL},
      }
    },
  };

FSM esc_fsm =
  {
    "ESC FSM",
    STATE_ESC_INIT,
    esc_table,
  };

////////////////////////////////////////////////////////////////////////////////
//

// Drives FSMs

void drive_fsms(void)
{
  while( !token_queue_empty() )
    {
      int tok = unqueue_token();
      
      do_fsm(&esc_fsm, &esc_state, tok);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// QT Touch Debug
//
//
// On schematic Y3 is Y0
//              Y2 is Y1
//              Y1 is Y2
//              Y0 is Y3
//
//  Key codes
//  =========
//
//  Key        X   Y   Code
//  -----------------------
//
//  KI RESET   0   4    32
//  A          0   0    0      ****               Was  3   0
//  B          2   0    2
//  C          1   4    33
//  0          0   1    8      (((((               Was 4   1 
//  1          0   2   16     ====                Was 6   2
//  2          6   2   22     ====
//  3          6   1
//  4          2   2          !!!!                Was 5 2
//  5          5   2          !!!!
//  6          5   1
//  7          1   2   10        $$$$$              Was 4  2
//  8          4   2           $$$$$
//  9          4   1           (((((((
//  RUN        3   0    3        ****
//  STOP       2   4   36
//  RELOAD     1   5
//  CHECK      2   5
//  DUMP       3   5
//  LOAD IAR   0   3   24  +++++                    Was 7  3
//  LOAD ADDR  7   3       +++++
//  LOAD STORE 6   3
//  DECR ADDR  4   3        ------                  Was 5   3
//  INCR ADDR  5   3        ------
//  NORM RESET 0   3
//  CLEAR      3   2    19   &&&&&                    Was 7   2
//  DOT        7   1
//  MINUS      7   2       &&&&&
//
////////////////////////////////////////////////////////////////////////////////

typedef struct _QT_KEYS
{
  char *name;
  int x;
  int y;
  int esc_token;
} QT_KEYS;

// Note that on the PCB Y0-Y3 are reversed and so are actually
// Y3-Y0

#define Y0 3
#define Y1 2
#define Y2 1
#define Y3 0
#define Y4 4
#define Y5 5

QT_KEYS qt_keycode[] =
  {
#if 1
    {"KI RESET   ", 0,   Y4, TOK_KEY_KI_RESET},
    {"A          ", 0,   Y0, TOK_KEY_A},
    {"B          ", 2,   Y0, TOK_KEY_B},
    {"C          ", 1,   Y4, TOK_KEY_C},
    {"0          ", 0,   Y1, TOK_KEY_0},
    {"1          ", 0,   Y2, TOK_KEY_1},
    {"2          ", 6,   Y2, TOK_KEY_2},
    {"3          ", 6,   Y1, TOK_KEY_3},
    {"4          ", 2,   Y2, TOK_KEY_4},
    {"5          ", 5,   Y2, TOK_KEY_5},
    {"6          ", 5,   Y1, TOK_KEY_6},
    {"7          ", 1,   Y2, TOK_KEY_7},
    {"8          ", 4,   Y2, TOK_KEY_8},
    {"9          ", 4,   Y1, TOK_KEY_9},
    {"RUN        ", 3,   Y0, TOK_KEY_RUN},
    {"STOP       ", 2,   Y4, TOK_KEY_STOP},
    {"RELOAD     ", 1,   Y5, TOK_KEY_RELOAD},
    {"CHECK      ", 2,   Y5, TOK_KEY_CHECK},
    {"DUMP       ", 3,   Y5, TOK_KEY_DUMP},
    {"LOAD IAR   ", 0,   Y3, TOK_KEY_LOAD_IAR},
    {"LOAD ADDR  ", 7,   Y3, TOK_KEY_LOAD_ADDR},
    {"LOAD STORE ", 6,   Y3, TOK_KEY_LOAD_STORE},
    {"DECR ADDR  ", 4,   Y3, TOK_KEY_DECR_ADDR},
    {"INCR ADDR  ", 5,   Y3, TOK_KEY_INCR_ADDR},
    {"NORM RESET ", 2,   Y3, TOK_KEY_NORMAL_RESET},
    {"CLEAR      ", 3,   Y2, TOK_KEY_CLEAR},
    {"DOT        ", 7,   Y1, TOK_KEY_DOT},
    {"MINUS      ", 7,   Y2, TOK_KEY_MINUS},
#endif

#if 0
    {"KI RESET   ", 0,   4, TOK_KEY_KI_RESET},
    {"A          ", 0,   3, TOK_KEY_A},
    {"B          ", 2,   3, TOK_KEY_B},
    {"C          ", 1,   4, TOK_KEY_C},
    {"0          ", 0,   2, TOK_KEY_0},
    {"1          ", 0,   1, TOK_KEY_1},
    {"2          ", 6,   1, TOK_KEY_2},
    {"3          ", 6,   2, TOK_KEY_3},
    {"4          ", 2,   1, TOK_KEY_4},
    {"5          ", 5,   1, TOK_KEY_5},
    {"6          ", 5,   2, TOK_KEY_6},
    {"7          ", 1,   1, TOK_KEY_7},
    {"8          ", 4,   1, TOK_KEY_8},
    {"9          ", 4,   2, TOK_KEY_9},
    {"RUN        ", 3,   3, TOK_KEY_RUN},
    {"STOP       ", 2,   4, TOK_KEY_STOP},
    {"RELOAD     ", 1,   5, TOK_KEY_RELOAD},
    {"CHECK      ", 2,   5, TOK_KEY_CHECK},
    {"DUMP       ", 3,   5, TOK_KEY_DUMP},
    {"LOAD IAR   ", 0,   0, TOK_KEY_LOAD_IAR},
    {"LOAD ADDR  ", 7,   0, TOK_KEY_LOAD_ADDR},
    {"LOAD STORE ", 6,   0, TOK_KEY_LOAD_STORE},
    {"DECR ADDR  ", 4,   0, TOK_KEY_DECR_ADDR},
    {"INCR ADDR  ", 5,   0, TOK_KEY_INCR_ADDR},
    {"NORM RESET ", 2,   0, TOK_KEY_NORMAL_RESET},
    {"CLEAR      ", 3,   1, TOK_KEY_CLEAR},
    {"DOT        ", 7,   2, TOK_KEY_DOT},
    {"MINUS      ", 7,   1, TOK_KEY_MINUS},
#endif   
  };

#define NUM_QT_KEYS (sizeof(qt_keycode)/sizeof(QT_KEYS))
#define UNKNOWN_NAME "????"

char *qt_key_name(int code)
{
  for(int i=0; i<NUM_QT_KEYS;i++)
    {
      //printf("\n%d: %d  %s", i, ((qt_keycode[i].x)+8*(qt_keycode[i].y)), qt_keycode[i].name);
      if( ((qt_keycode[i].x)+8*(qt_keycode[i].y)) == code )
        {
          return(qt_keycode[i].name);
        }	
    }

  return(UNKNOWN_NAME);
}

int qt_esc_code(int code)
{
  for(int i=0; i<NUM_QT_KEYS;i++)
    {
      if( ((qt_keycode[i].x)+8*(qt_keycode[i].y)) == code )
	{
	  return(qt_keycode[i].esc_token);
        }	
    }

  return(TOK_NONE);
}

int qt_key_used(int code)
{
  char *name = qt_key_name(code);

  if( strcmp(name, UNKNOWN_NAME)==0 )
    {
      return(0);
    }

  return(1);
}

// 16 bits crc calculation. Initial crc entry value must be 0.
// The message is not augmented with 'zero' bits.
// polynomial = X16 + X12 + X5 + 1
// data is an 8 bit number, unsigned
// crc is a 16 bit number, unsigned
// repeat this function for each data block byte, folding the result
// back into the call parameter crc

uint16_t sixteen_bit_crc(unsigned long crc, unsigned char data)
{
  unsigned char index;  // shift counter
  crc ^= (uint16_t)(data) << 8;
  index = 8;

  do
    // loop 8 times
    {
      if(crc & 0x8000)
	{
	  crc= (crc << 1) ^ 0x1021;
	}
      else
	{
	  crc = crc << 1;
	}
    }
  while(--index);
  
  return crc;
}

int get_byte(void)
{
  while( !uart_is_readable(uart0) )
    {
    }
  
  return(uart_getc(uart0));
}

#define SETUPS_BLOCK_DATA_LEN   350-2
#define SETUPS_BLOCK_LEN 350

int setups_block[SETUPS_BLOCK_LEN];

void dump_setups_block(void)
{
  uint16_t crc = 0L;
  
  for(int i=0; i<SETUPS_BLOCK_LEN; i++)
    {
      setups_block[i] = get_byte();      
    }

  for(int i=0; i<SETUPS_BLOCK_LEN; i++)
    {
      if( (i %16) == 0 )
	{
	  printf("\n%04X: ", i);
	}
      
      printf("%02X ", setups_block[i]);

      if( i < (SETUPS_BLOCK_LEN - 2))
	{
	  crc = sixteen_bit_crc(crc, setups_block[i]);
	}
    }

  printf("\nCRC= %04lX\n", crc);
}

void wait_for_uart_byte(int byte)
{
  printf("\nWaiting for QT byte");
  
  while( !uart_is_readable(uart0) )
    {
    }
  
  char c = uart_getc(uart0);
  
  printf("\nReceived QT RX:%d 0x%02X", c, c);
  
}

// Reset the touch controller. This should write the setups to internal EEPROM

void do_qt_reset(void)
{
  // Write the data
  uart_putc(uart0, 0x04);
  sleep_ms(CHAR_MS_DELAY);
  uart_putc(uart0, 0x04);
  sleep_ms(CHAR_MS_DELAY);

  wait_for_uart_byte(0xFB);
}

// Setups block based on default, altered to match our keyboard

uint8_t setups_block_esc[SETUPS_BLOCK_LEN] =
  {
    0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26,
    0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26,
    0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26, 0x26,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
    0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52,
    0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52,
    0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
    0x09, 0x01, 0x07, 0x07, 0x04, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07, 0x06, 0x05, 0x00,
    0x04, 0x00, 0x00, 0x00, 0x07, 0x04, 0x05, 0x00, 0x08, 0x00, 0x00, 0x00, 0x0E, 0x07, 0x08, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x12, 0x04, 0x0A, 0x0D, 0x03, 0x03, 0x03, 0x00, 0x10, 0x00, 0x00, 0x00, 0x09, 0x08, 0x0A, 0x00,
    0x08, 0x00, 0x00, 0x00, 0x0E, 0x08, 0x0C, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x1A, 0x0F, 0x0F, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x64, 0x70, 0x20, 0x0F, 0x00, 0x18, 0x1E, 0x24, 0x30, 0x00, 0x11, 0x50,
  };

void write_setups_block(uint8_t *block)
{
  printf("\nCopying setup data...");
  
  // Copy the setups block to the buffer
  for(int i=0; i<SETUPS_BLOCK_LEN; i++)
    {
      setups_block[i] = *(block++);
    }

  // Remove unused keys
  printf("\nRemoving unused keys...");
  for(int i = 96; i<96+48; i++)
    {
      if( !qt_key_used(i-96) )
	{
	  // Clear lower nibble
	  setups_block[i] &= 0xF0;
	}
    }
  
  // Calculate crc
  uint16_t crc = 0;

  printf("\nCalculating CRC...");
  for(int i=0; i<SETUPS_BLOCK_LEN-2; i++)
    {
      crc = sixteen_bit_crc(crc, setups_block[i]);
    }

  setups_block[SETUPS_BLOCK_LEN-1] = crc >> 8;
  setups_block[SETUPS_BLOCK_LEN-2] = crc &  0xFF;

  // Write the data
  printf("\nWriting command...");
    
  uart_putc(uart0, 0x01);
  sleep_ms(CHAR_MS_DELAY);
  uart_putc(uart0, 0x01);
  sleep_ms(CHAR_MS_DELAY);

  printf("\nWriting data...");
  for(int i=0; i<SETUPS_BLOCK_LEN; i++)
    {
      uart_putc(uart0, setups_block[i]);
      sleep_ms(CHAR_MS_DELAY);
    }

  // Wait for two 0xFE bytes from the keyboard
  printf("\nSending write command");
  //  uart_putc(uart0, 0x02);
  //uart_putc(uart0, 0x02);
  printf("\nWaiting for 0xFE twice...");
  wait_for_uart_byte(0xFE);
  printf("\nReceived 0xFE");
  wait_for_uart_byte(0xFE);
  printf("\nReceived 0xFE");

  printf("\nCommand completed");

}

void check_status(void)
{
  uart_putc(uart0, 0x06);
  printf("\n");
  
  for(int i=0; i<7; i++)
    {
      status[i] = get_byte();
      printf("%02X ", status[i]);
    }
}

//Send Cal All command twice then wait for an 0xFC
void do_calibrate_all(void)
{
  printf("\nCal All");
  uart_putc(uart0, 0x03);
  sleep_ms(CHAR_MS_DELAY);
  uart_putc(uart0, 0x03);
  sleep_ms(CHAR_MS_DELAY);

  wait_for_uart_byte(0xFC);
}

void cli_qt_debug(void)
{
  int key;
  int contread = 0;
  int contflag = 0;
  
  int keymap[8];
      
  printf("\nQT Touch");
  printf("\n");
  printf("\nC: Cal All");
  printf("\nL: Low Level Cal and Offset");
  printf("\nO: Overview");
  printf("\nr: Report All Keys");
  printf("\n-: Get Last Command");
  printf("\ns: Setups CRC Check");
  printf("\nd: Dump Setups Block");
  printf("\n*: Toggle continuous read");
  printf("\nW: Write setups Block");
  printf("\nR: Reset");
  printf("\n");
  
  while(1)
    {
      if( contflag )
	{
	  contread++;
	}
      
      if( contread>100)
	{
	  contread = 0;
	  
	  // Send command and read 8 bytes back
	  uart_putc(uart0, 0x07);
#if 1	  
	  for(int i = 0; i<8; i++)
	    {
	      while( !uart_is_readable(uart0) )
		{
		}
	      
	      keymap[i] = uart_getc(uart0);
	      sleep_ms(1);
	    }

	  // Any keys pressed?
	  int t = 0;
	  for(int i = 0; i<6; i++)
	    {
	      t += keymap[i];
	    }

	  if( t > 0 )
	    {
	      printf("\n");
	      
	      // See what keys are pressed
	      for(int i = 0; i<6; i++)
		{
		  for(int j=0; j<8; j++)
		    {
		      if( keymap[i] & (1<<j) )
			{
			  printf("%s (%d)  ", qt_key_name(i*8+j), i*8+j);
			}
		    }
		}
	    }
#endif	  
	  sleep_ms(50);
	}
      
      // If anything has come back from the QT, display it
      if( uart_is_readable(uart0) )
	{
	  char c = uart_getc(uart0);

	  printf("\nQT RX:%d 0x%02X", c, c);
	}
      
      if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
	{
	  switch(key)
	    {
	    case '*':
	      contflag = !contflag;
	      break;
	      
	    case 'C':
	      // Cal All
	      printf("\nCal All");
	      uart_putc(uart0, 0x03);
	      uart_putc(uart0, 0x03);
	      do
		{
		  check_status();
		}
	      while(status[0] & 0x2);
	      break;

	    case 'L':
	      // Low Level Cal and Offset
	      printf("\nLow Level Cal and Offset");
	      uart_putc(uart0, 0x02);
	      uart_putc(uart0, 0x02);
	      check_status();
	      break;

	    case 'O':
	      // Send Overview
	      printf("\nOverview");
	      uart_putc(uart0, 0x06);
	      break;

	    case '-':
	      // Get last command
	      printf("\nGet Last Command");
	      uart_putc(uart0, 0x0F);
	      break;

	    case 's':
	      // Setups CRC Check
	      printf("\nSetups CRC Check");
	      uart_putc(uart0, 0x0E);
	      break;

	    case 'd':
	      printf("\nDump Setups Block");
	      uart_putc(uart0, 0x0D);
	      dump_setups_block();
	      
	      break;

	    case 'W':
	      printf("\nWrite setups Block");
	      write_setups_block(&(setups_block_esc[0]));
	      break;

	    case 'R':
	      printf("\nReset");
	      do_qt_reset();
	      break;

	    case 'r':
	      // report all keys
	      printf("\nReport All Keys");
	      uart_putc(uart0, 0x07);
	      break;
	      
	    default:
	      return;
	      break;
	    }
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// QT Touch keyboard keycode determination
//

int get_qt_key_code(void)
{
  int keymap[8];
  
  // Send command and read 8 bytes back
  uart_putc(uart0, 0x07);
  
  for(int i = 0; i<8; i++)
    {
      while( !uart_is_readable(uart0) )
	{
	}
      
      keymap[i] = uart_getc(uart0);
      sleep_ms(1);
    }
  
  // Any keys pressed?
  int t = 0;
  for(int i = 0; i<6; i++)
    {
      t += keymap[i];
    }
  
  if( t > 0 )
    {

      // See what keys are pressed
      for(int i = 0; i<6; i++)
	{
	  for(int j=0; j<8; j++)
	    {
	      if( keymap[i] & (1<<j) )
		{
		  // We now know what the QT  key code is, now look that up
		  // and find the appropriate ESc code
		  return(qt_esc_code(i*8+j));
#if DEBUG_QT_KB
		  printf("%s (%d)  ", qt_key_name(i*8+j), i*8+j);
#endif		  
		}
	    }
	}
    }
  return(0);
}
  
////////////////////////////////////////////////////////////////////////////////

void cli_boot_mass(void)
{
  reset_usb_boot(0,0);
}

void cli_update_display(void)
{
  ESC_STATE *s = &esc_state;

  s->update_display = 1;
}

char str_state[600];

char *get_string_state(void)
{
  char line[80];
  ESC_STATE *s = &esc_state;
  
  sprintf(str_state, "\nIAR: %s", display_iar(s, SPEC_FORCE_IAR));
  sprintf(line,      "   Aux IAR: %s", display_iar(s, SPEC_FORCE_AUX_IAR));
  strcat(str_state, line);
  sprintf(line,      "\nBreakpoint: %s %s", s->breakpoint_on?"ON ":"OFF", display_iar(s, SPEC_BREAKPOINT));
  strcat(str_state, line);
  sprintf(line,      "\nKI            : %s", display_register_double_word(s->keyboard_register));
  strcat(str_state, line);
  sprintf(line,      "\nInst Reg      : %08X", s->instruction_register);
  strcat(str_state, line);
  sprintf(line,     "\nLink register  : %02X", s->link_register);
  strcat(str_state, line);
  sprintf(line,     "\nControl latch  : %d",
          s->control_latch);
  strcat(str_state, line);
  sprintf(line,     "\nRun            : %d   Extracode Run:%d  Stop:%d",
          s->run,
          s->extracode_run,
          s->stop);
  strcat(str_state, line);
#if 0  
  sprintf(line,      "\nAddr R0       : %s", display_address(s->address_register0));
  strcat(str_state, line);
  sprintf(line,      "\nAddr R1       : %s", display_address(s->address_register1));
  strcat(str_state, line);
  sprintf(line,      "\nAddr R2       : %s", display_address(s->address_register2));
  strcat(str_state, line);
#endif  
  return(str_state);
}

// Dump state info
void cli_dump_state(void)
{
  ESC_STATE *s = &esc_state;

  printf("\n");
  printf("\n/=========\\");
  printf("\n|  State  |");
  printf("\n\\=========/");
  
  printf("\n%s", get_string_state());
  
  for(int i=0; i<NUM_WORD_REGISTERS; i++)
    {
      if( (i % 4) == 0 )
        {
          printf("\n");
        }
      printf("  R%d: %s", i, display_register_single_word(read_register(s, i)));
    }

  printf("\n");
  for(int i=0; i<NUM_DBL_WORD_REGISTERS; i++)
    {
      printf("  R%d: %s", i+NUM_WORD_REGISTERS, display_register_double_word(read_register(s, i+NUM_WORD_REGISTERS)));
    }
  
  printf("\n");
  printf("\nInternal");
  printf("\n========");

  printf("\nStage   :'%c'",       s->stage);
  printf("\nabcd    : %d%d%d%d   rc=%d rd=%d",  s->inst_digit_a,  s->inst_digit_b,  s->inst_digit_c,  s->inst_digit_d, s->reginst_rc, s->reginst_rd);
  printf("\nAa      : %02X ap:  %02X ",                s->inst_aa, s->inst_ap);
  printf("\nAp1..3  : %03X %03X %03X",                 s->Ap1, s->Ap2, s->Ap3);
  printf("\nAa1..3  : %03X %03X %03X",                 s->Aa1, s->Aa2, s->Aa3);
  printf("\nReg Inst Lit      : %d",                   s->reginst_literal);
  printf("\nKI Reset          : %d",                   s->ki_reset_flag);
  printf("\nError             : %d",                   s->error);
  printf("\nExtracode         : %s    Exiting : %d",   IS_EXTRACODE?"Yes":"No ", s->exiting_extracode);
  printf("\n");

  printf("\nTest:  running: %s", test_running?"Yes":"No ");
  printf("  number: %d", test_number);
  printf("  loop count: %d", test_loop_count);
  printf("  waiting for stop: %d", test_waiting_for_stop);
  printf("\n");
}

////////////////////////////////////////////////////////////////////////////////
//
// Dump Store
//

char *csum_desc(uint32_t csum)
{
  for(int i=0; i<NUM_EXTRACODE_IDS; i++)
    {
      if( extracode_info[i].csum == csum )
        {
          return(extracode_info[i].desc);
        }
    }
  
  return("");
}

char *display_store_checksum(ESC_STATE *s, char *title, int first, int last)
{
  uint32_t csum = checksum_store(s, first, last);
  printf("\n%s checksum:%08X %s", title, csum, csum_desc(csum));
}

  
void cli_dump_store(void)
{
  ESC_STATE *s = &esc_state;

  for(int i=0; i<STORE_SIZE; i++)
    {
      if( (i % 5) == 0 )
	{
	  printf("\n");
	}

      printf("  %03d:%08X (%s)", i, s->store[i], display_store_word(s->store[i]));
    }
  
  printf("\n");
  display_store_checksum(s, "Lower store", LOWER_STORE);
  display_store_checksum(s, "Upper store", UPPER_STORE);
  display_store_checksum(s, "Extracode  ", EXTRACODE_CSUM_RANGE);
  display_store_checksum(s, "All   store", ALL_STORE);
  printf("\n");

}

//------------------------------------------------------------------------------
//
// Checksum the store.
//
// This can be used to used to identify the extracodes that are currently stored
// or check the user program
//

uint32_t checksum_store(ESC_STATE *s, int first, int last)
{
  uint32_t csum = 0;
  
  for(int i=first; i<=last; i++)
    {
      csum += s->store[i];
    }
  
  return(csum);	   
}

////////////////////////////////////////////////////////////////////////////////
//
// Another digit pressed, update the parameter variable
//

void cli_digit(void)
{
  printf("\n%d", keypress);
  queue_token(TOK_KEY_0 + keypress - '0');
}

void cli_dot(void)
{
  queue_token(TOK_KEY_DOT);
}

void cli_minus(void)
{
  queue_token(TOK_KEY_MINUS);
}

void cli_key_a(void)
{
  queue_token(TOK_KEY_A);
}

void cli_key_b(void)
{
  queue_token(TOK_KEY_B);
}

void cli_key_c(void)
{
  queue_token(TOK_KEY_C);
}

void cli_normal_reset(void)
{
  queue_token(TOK_KEY_NORMAL_RESET);
}

void cli_ki_reset(void)
{
  queue_token(TOK_KEY_KI_RESET);
}

void cli_load_iar(void)
{
  queue_token(TOK_KEY_LOAD_IAR);
}

void cli_incr_addr(void)
{
  queue_token(TOK_KEY_INCR_ADDR);
}

void cli_decr_addr(void)
{
  queue_token(TOK_KEY_DECR_ADDR);
}

void cli_load_addr(void)
{
  queue_token(TOK_KEY_LOAD_ADDR);
}

void cli_load_store(void)
{
  queue_token(TOK_KEY_LOAD_STORE);
}

void cli_dump(void)
{
  queue_token(TOK_KEY_DUMP);
}

void cli_reload(void)
{
  queue_token(TOK_KEY_RELOAD);
}

void cli_check(void)
{
  queue_token(TOK_KEY_CHECK);
}

//------------------------------------------------------------------------------
//
// Set the breakpoint
//
// If parameter ends in A then that is a set a_flag, otherwise set flag to 0
//

void cli_breakpoint_old(void)
{
  ESC_STATE *s = &esc_state;
  
  if( parameter_a_flag )
    {
      s->breakpoint.a_flag = 1;
    }
  else
    {
      s->breakpoint.a_flag = 0;
    }
  
  s->breakpoint.address = parameter;
}

////////////////////////////////////////////////////////////////////////////////
//
// Set breakpoint address
//

void cli_breakpoint(void)
{
  int  key;
  int done = 0;
  ESC_STATE *s = &esc_state;
  
  printf("\nEnter breakpoint address: (ESC or <RETURN> to exit)");
  
  parameter = 0;
  parameter_a_flag = 0;
  
  while(!done)
    {
      if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
	{
	  switch(key)
	    {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	      parameter *= 16;
	      parameter += (key - '0');
	      prompt_breakpoint();
	      break;

              // A digit for IAR entry of breakpoint
            case 'A':
            case 'a':
              parameter_a_flag = 1;
              prompt_breakpoint();
              break;

#if 1
            case '-':
	      parameter *= 16;
	      parameter += 0xD;
              prompt_breakpoint();
              break;

            case '+':
	      parameter *= 16;
	      parameter += 0xC;
              prompt_breakpoint();
              break;
#endif         
	    case 27:
	    case 13:
	    case 10:
              // Set the breakpoint
              s->breakpoint.address = parameter;
              s->breakpoint.a_flag  = parameter_a_flag;            
	      done = 1;
	      break;
	      
	    default:
	      break;
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
}

void cli_breakpoint_toggle(void)
{
  ESC_STATE *s = &esc_state;

  s->breakpoint_on = !s->breakpoint_on;
  
  if( s->breakpoint_on )
    {
      printf("\nBreakpoint ON");    
    }
  else
    {
      printf("\nBreakpoint OFF");    
    }
  
}


//------------------------------------------------------------------------------

void cli_load_test_code(void)
{
  ESC_STATE *s = &esc_state;

  int i = 0;
  
  // R5 and 6 have 2 and 3 in them
  s->store[i++] = 0x03520363;

  // Add R5 and R6 and add 6
  s->store[i++] = 0x10560056;

  // Test R5
  s->store[i++] = 0x05530550;
  s->store[i++] = 0x05510552;
  s->store[i++] = 0x05530554;

  // Shift R5 left 1 place and back again
  s->store[i++] = 0x06510751;

  // Branch to zero
  s->store[i++] = 0x24002400;
  
  // Stop Executing
  s->store[i++] = 0x06510751;

}

void test_sw_add(int a, int b, int r)
{
  int c;
  
  c = bcd_sw_addition(&esc_state, a, b);
  
  printf("\n%08X + %08X = %08X : %s (r=%08X)", a, b, c, (c ==r)?"PASS":"FAIL", r);
}

typedef struct _SW_BCD_TEST
{
  int a;
  int b;
  int r;
} SW_BCD_TEST;

SW_BCD_TEST sw_bcd_test[] =
  {
    {0xC0000010, 0xC0000020, 0xC0000030},
    {0xC0000020, 0xC0000010, 0xC0000030},
    {0xC0000007, 0xC0000005, 0xC0000012},
    {0xD0000020, 0xD0000010, 0xD0000030},
    {0xC0123456, 0xC0654321, 0xC0777777},
    {0xD0123456, 0xD0654321, 0xD0777777},
    {0xD0077777, 0xD0777777, 0xD0855554},
    {0xC0002345, 0xD0000045, 0xC0002300},
    {0xC0000045, 0xD0002345, 0xD0002300},

    {0xC0999900, 0xC0000099, 0xC0999999},
    {0xC0999900, 0xC0000100, 0xC0000000},

    {0xD0999900, 0xD0000099, 0xD0999999},
    {0xD0999900, 0xD0000100, 0xD0000000},

   
  };

#define NUM_SW_BCD_TEST (sizeof(sw_bcd_test)/sizeof(SW_BCD_TEST))

void test_dw_add(uint64_t a, uint64_t b, uint64_t r)
{
  uint64_t c;
  
  c = bcd_dw_addition(a, b);
  
  printf("\n%016llX + %016llX = %016llX : %s (r=%016llX)", a, b, c, (c ==r)?"PASS":"FAIL", r);
}

typedef struct _DW_BCD_TEST
{
  uint64_t a;
  uint64_t b;
  uint64_t r;
} DW_BCD_TEST;

DW_BCD_TEST dw_bcd_test[] =
  {
    {0xC000000000000010L, 0xC000000000000020L, 0xC000000000000030L},
    {0xC000000000000020L, 0xC000000000000010L, 0xC000000000000030L},
    {0xC000000000000007L, 0xC000000000000005L, 0xC000000000000012L},
    {0xD000000000000020L, 0xD000000000000010L, 0xD000000000000030L},
    {0xC000000000123456L, 0xC000000000654321L, 0xC000000000777777L},
    {0xD000000000123456L, 0xD000000000654321L, 0xD000000000777777L},
    {0xD000000000077777L, 0xD000000000777777L, 0xD000000000855554L},
    {0xC000000000002345L, 0xD000000000000045L, 0xC000000000002300L},
    {0xC000000000000045L, 0xD000000000002345L, 0xD000000000002300L},

    {0xC000000000999900L, 0xC000000000000099L, 0xC000000000999999L},
    {0xC000000000999900L, 0xC000000000000100L, 0xC000000000000000L},

    {0xD000000000999900L, 0xD000000000000099L, 0xD000000000999999L},
    {0xD000000000999900L, 0xD000000000000100L, 0xD000000000000000L},



   
  };

#define NUM_DW_BCD_TEST (sizeof(sw_bcd_test)/sizeof(SW_BCD_TEST))

void cli_test_bcd(void)
{
  int a, b, c;

  printf("\n\nSingle word tests\n");
  
  // Perform some arithmetic
  for(int i=0; i<NUM_SW_BCD_TEST; i++)
    {
      test_sw_add(sw_bcd_test[i].a, sw_bcd_test[i].b, sw_bcd_test[i].r);
    }

  printf("\n\nDouble word tests\n");
    
  // Perform some arithmetic
  for(int i=0; i<NUM_DW_BCD_TEST; i++)
    {
      test_dw_add(dw_bcd_test[i].a, dw_bcd_test[i].b, dw_bcd_test[i].r);
    }
  
  printf("\n");
}

void cli_load_test_code_2(void)
{
  ESC_STATE *s = &esc_state;
  int i = 0;

#if 0  
  // R5 and 6 have 2 and 1 in them
  s->store[i++] = 0x03520361;

  // Add R5 and R6 and add 6
  s->store[i++] = 0x10560000;

  // Branch to 1 to count forever
  s->store[i++] = 0x24012401;
  
  // Stop Executing
  s->store[i++] = 0x06510751;

  // Floating point
  s->store[i++] = 0xD5314159;
#endif

#if 1
  // R5 and 6 have 2 and 1 in them
  // test copying of six and RH six digits from one register to another

  // Set registers up
  write_register(s, 0, 0x00123456);
  write_register(s, 0, 0x00876543008765432L);
  write_register(s, 1, 0x00000000);

  // Copy d into c: 0 into 1
  s->store[i++] = 0x14101418;

  printf("\nSetup Test 2");
  printf("\nSet up R[8] = %016llX", read_register(s, 0));
#endif
  
}

// We have two flags:
//
// Run: If set then program is executed
// Stop: If set then execution is stopped after next Stage C

void cli_run(void)
{
  ESC_STATE *s = &esc_state;

  s->update_display = 0;

  queue_token(TOK_KEY_RUN);
}

void cli_stop(void)
{
  ESC_STATE *s = &esc_state;

  s->update_display = 0;

  queue_token(TOK_KEY_STOP);
}

#if ESC_TYPE_SMALL

void cli_key_key_test(void)
{
  int key;
  
  printf("\nKey test");
      
  while(1)
    {
      if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
	{
	  switch(key)
	    {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	      for(int i=0; i<NUM_KBD_DRV; i++)
		{
		  gpio_put(kbd_drv_gpios[i], 0);
		}
	      
	      gpio_put(kbd_drv_gpios[key-'0'], 1);
	      break;
	      
	    default:
	      return;
	      break;
	    }
	}
      
      printf("\n%d", kbd_read_sense());
    }
}

#endif

void cli_file_list(void)
{
  if( !file_list(ESC_DIR) )
    {
      printf("\n\nError %s", sd_error);
    }

  // Write state
  write_state_to_file(&esc_state, "state.esc");

  cat_file("state.esc");
}

void cli_file_read_state(void)
{
  read_file_into_state("state.esc", &esc_state);

}


////////////////////////////////////////////////////////////////////////////////
//
// Test 0
//
// Keyboard input
//

INIT_INFO test_init_0[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    0x123456},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    0x987654321},
    {IC_END,          0},
  };

TOKEN test_seq_0[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_2,
    TOK_KEY_2,
    TOK_KEY_3,
    TOK_KEY_3,
    TOK_KEY_5,
    TOK_NONE,
  };


TEST_INFO test_res_0[] =
  {
    {TC_REG_N,   0},
    {TC_MUST_BE, 0x123456},
    {TC_REG_N,   8},
    {TC_MUST_BE, 0x987654321L},
    {TC_END,     0},

  };

TEST_LOAD_STORE test_0_store =
  {
    {0x12345678, 0x112233, -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 1
//
// Register instructions
//
// 

INIT_INFO test_init_1[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x123456)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x40)},
    {IC_SET_REG_N,    2},
    {IC_SET_REG_V,    SW_PLUS(0x5)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_MINUS(0x987654321)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_PLUS(0x000000000010)},
    {IC_END,          0},
  };

// Run just one instruction at 00

TOKEN test_seq_1[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,
    TOK_KEY_C,                 // copy R0 to R1
    TOK_TEST_CHECK_RES,
   
    TOK_KEY_C,                 // One added to R1
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                // r1-2
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 // r2 = 8-r2
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 // r1 = 2
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 // r1 = r1 -9
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 // r1 = r2
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 // r1 = r1 + r2
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 // R1 = 5
    TOK_KEY_C,                 // r1 = r1 -r2
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 //  r0 = r0 -r1
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,

    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 //  load r1 with 2
    TOK_KEY_C,                 //  r1 = r1 - r9
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 //  10: 
    TOK_KEY_C,                 //  r1 = r9 - r1
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,                 //  11: 
    TOK_KEY_C,                 // 
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_1[] =
  {
    // Original register contents must be unchanged
    {TC_REG_N,   0},
    {TC_MUST_BE, 0xc0123456},
    {TC_REG_N,   8},
    {TC_MUST_BE, 0xd000000987654321L},

    // Copied value must be there
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0123456},
    {TC_END_SECTION, 0},   

    // One added to R1
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0123457},
    {TC_END_SECTION, 0},   

    // Two subtracted from R1
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0123455},
    {TC_END_SECTION, 0},   

    // Two subtracted from R1
    {TC_REG_N,   2},
    {TC_MUST_BE, 0xc0000003},
    {TC_END_SECTION, 0},   

    // Load R1 with 2
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0000002},
    {TC_END_SECTION, 0},   

    // Subtract 9 from R1
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xd0000007},
    {TC_END_SECTION, 0},   

    // Assign R1 and R2
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0000003},
    {TC_END_SECTION, 0},   

    // Add R1 and R2
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0000006},
    {TC_END_SECTION, 0},   

    // Subtract R2 from R1 after loading R2 with 5
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0000001},
    {TC_END_SECTION, 0},   

    //Subtract R1 from R0
    {TC_REG_N,   0},
    {TC_MUST_BE, 0xd0123455},
    {TC_END_SECTION, 0},

    // After small sequence, R5 = 1
    {TC_REG_N,   5},
    {TC_MUST_BE, 0xc0000001},
    {TC_END_SECTION, 0},

    // R1 = 30
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0000008},
    {TC_END_SECTION, 0},

    // 10: Load R1 with 2, Subtract R9 from R1
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xD0000008},
    {TC_END_SECTION, 0},
    
    // 11: R8 = 8, R9 = 9
    {TC_REG_N,   8},
    {TC_MUST_BE, 0xC000000000000008},
    {TC_REG_N,   9},
    {TC_MUST_BE, 0xC000000000000009},

    {TC_END,     0},

  };

TEST_LOAD_STORE test_1_store =
  {
    {
      0x13100011,      // 00  Copy R0 to R1, Add 1 to R1
      0x01120228,      // 01  Subtract 2 from R1, subtract R2 from 8
      0x03120119,      // 02  Load R1 with 2, subtract 9 from R1
      0x13121012,      // 03  Assign R1, R2,  Add R1 and R2
      0x03251112,      // 04  load R1 with 5, subtract R2 from R1
      0x12010000,      // 05  Subtract R0 from R1
      0x03510000,      // 06  R5 = 1
      0x01590000,      // 07  R5 = R5 – 9  {R5 now -8}
      0x00590000,      // 08  R5 = R5 + 9  {R5 now -1}
      0x03121219,      // 09 Load R1 with 2, Subtract R1 from R9
      0x03121119,      // 10 Load R1 with 2, Subtract R9 from R1

      // Double word register instructions
      0x03880399,      // 11: R8 = 8, R9 = 9
      
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 2
//
// Register instructions
//

INIT_INFO test_init_2[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    0xc0123456},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    0x987654321},
    {IC_END,          0},
  };

TOKEN test_seq_2[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_2,
    TOK_KEY_2,
    TOK_KEY_3,
    TOK_KEY_3,
    TOK_KEY_5,
    TOK_NONE,
  };


TEST_INFO test_res_2[] =
  {
    // Original register contents must be unchanged
    {TC_REG_N,   0},
    {TC_MUST_BE, 0xc0123456},
    {TC_REG_N,   8},
    {TC_MUST_BE, 0x00000987654321L},

    {TC_END,     0},

  };

TEST_LOAD_STORE test_2_store =
  {
    {0x12345678, 0x112233, -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 3
//
// Register instructions
//

INIT_INFO test_init_3[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    0x123456},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    0x987654321},
    {IC_END,          0},
  };

TOKEN test_seq_3[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_9,
    TOK_KEY_8,
    TOK_KEY_DOT,
    TOK_KEY_LOAD_ADDR,
    TOK_TEST_CHECK_RES,

    TOK_KEY_INCR_ADDR,
    TOK_TEST_CHECK_RES,

    TOK_KEY_INCR_ADDR,
    TOK_TEST_CHECK_RES,

    TOK_KEY_INCR_ADDR,
    TOK_TEST_CHECK_RES,

    TOK_KEY_NORMAL_RESET,
    TOK_KEY_2,
    TOK_KEY_DOT,
    TOK_KEY_LOAD_ADDR,
    TOK_TEST_CHECK_RES,
   
    TOK_KEY_DECR_ADDR,
    TOK_TEST_CHECK_RES,

    TOK_KEY_DECR_ADDR,
    TOK_TEST_CHECK_RES,
    TOK_NONE,
  };


TEST_INFO test_res_3[] =
  {
    // Original register contents must be unchanged
    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000098},
    {TC_END_SECTION, 0},   

    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000099},
    {TC_END_SECTION, 0},   

    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000100},
    {TC_END_SECTION, 0},   

    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000101},
    {TC_END_SECTION, 0},   

    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000002},
    {TC_END_SECTION, 0},   

    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000001},
    {TC_END_SECTION, 0},   

    {TC_REG_ADDR,    0},
    {TC_MUST_BE,     0x00000000},

    {TC_END,         0},

  };

TEST_LOAD_STORE test_3_store =
  {
    {0x12345678, 0x112233, -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 4
//
// Copy RH 6 digits instructions
//
// 

INIT_INFO test_init_4[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x123456)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    2},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_PLUS(0x0D000000)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_MINUS(0x03000001)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0x0000000987654321L)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_MINUS(0x01CD112233445566L)},
    {IC_END,          0},
  };

TOKEN test_seq_4[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_4[] =
  {
    // Original register contents must be unchanged
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xC0123456},
    {TC_END_SECTION, 0},
   
    {TC_REG_N,   2},
    {TC_MUST_BE, 0xC0654321},
    {TC_END_SECTION, 0},   

    {TC_REG_N,   3},
    {TC_MUST_BE, 0xD0445566},
    {TC_END_SECTION, 0},   

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xC000000987123456L},
    {TC_END_SECTION, 0},
    
    {TC_REG_N,   9},
    {TC_MUST_BE, 0xC1CD112233123456L},
    {TC_END_SECTION, 0},

    // Check digits other than lower 6 aren't affected
    {TC_REG_N,   4},
    {TC_MUST_BE, 0xCD123456},

    {TC_END,     0},

  };

TEST_LOAD_STORE test_4_store =
  {
    {
      0x14101428,      // RH 6 dig of R0 into R1, RH 6 dig of R8 into R2
      0x14391480,      // RH 6 dig of R9 into R3, RH 6 dig of R0 into R8
      0x14901440,      // RH 6 dig of R0 into R9, RH 6 dig of R0 into R4
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 5
//
// TEST instructions
//
// 

INIT_INFO test_init_5[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    2},
    {IC_SET_REG_V,    SW_MINUS(0x1)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x00011111)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_PLUS(0x00111110)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0xC000056987654321)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_MINUS(0xC000112233445560)},
    {IC_END,          0},
  };

TOKEN test_seq_5[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_5[] =
  {
    // Test control latch after TEST instructions
    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     1},
    {TC_END_SECTION, 0},

    {TC_CL,          0},
    {TC_MUST_BE,     0},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_5_store =
  {
    {
      0x05000510,      // TEST R0=0, TEST R1=0
      0x05110501,      // TEST R1>0, TEST R0>0
      0x05220502,      // TEST R1<0, TEST R0<0
      0x05330543,      // TEST LH digit R3=0, TEST LH digit R4=0
      0x05440534,      // TEST RH digit R4=0, TEST RH digit R3=0
      0x05830593,      // TEST LH digit R8=0, TEST LH digit R9=0
      0x05940584,      // TEST RH digit R9=0, TEST RH digit R8=0
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 6
//
// Shift left instructions
//
// 

INIT_INFO test_init_6[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x2)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x00123456)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_PLUS(0x00123456)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0xC000123456789012)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_PLUS (0xC000123456789012)},
    {IC_END,          0},
  };

TOKEN test_seq_6[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_6[] =
  {
   
    {TC_REG_N,   3},
    {TC_MUST_BE, 0xc0234560},
    {TC_END_SECTION, 0},

    {TC_REG_N,   3},
    {TC_MUST_BE, 0xc0456000},
    {TC_END_SECTION, 0},

    {TC_REG_N,   4},
    {TC_MUST_BE, 0xc0234560},
    {TC_END_SECTION, 0},

    {TC_REG_N,   4},
    {TC_MUST_BE, 0xc0456000},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xc000234567890120},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xc000456789012000},
    {TC_END_SECTION, 0},

    {TC_REG_N,   9},
    {TC_MUST_BE, 0xc000234567890120},
    {TC_END_SECTION, 0},

    {TC_REG_N,   9},
    {TC_MUST_BE, 0xc000456789012000},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_6_store =
  {
    {
      0x06310632,
      0x16401641,
      0x06810682,
      0x16901691,
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 7
//
// Shift right instructions
//
// 

INIT_INFO test_init_7[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x2)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x00123456)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_PLUS(0x00123456)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0xC000123456789012)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_PLUS (0xC000123456789012)},
    {IC_END,          0},
  };

TOKEN test_seq_7[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_7[] =
  {
   
    {TC_REG_N,   3},
    {TC_MUST_BE, 0xc0012345},
    {TC_END_SECTION, 0},

    {TC_REG_N,   3},
    {TC_MUST_BE, 0xc0000123},
    {TC_END_SECTION, 0},

    {TC_REG_N,   4},
    {TC_MUST_BE, 0xc0012345},
    {TC_END_SECTION, 0},

    {TC_REG_N,   4},
    {TC_MUST_BE, 0xc0000123},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xc000012345678901},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xc000000123456789},
    {TC_END_SECTION, 0},

    {TC_REG_N,   9},
    {TC_MUST_BE, 0xc000012345678901},
    {TC_END_SECTION, 0},

    {TC_REG_N,   9},
    {TC_MUST_BE, 0xc000000123456789},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_7_store =
  {
    {
      0x07310732,
      0x17401741,
      0x07810782,
      0x17901791,
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 8
//
// Instructions [2-6]0
// 
// 

INIT_INFO test_init_8[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_END,          0},
  };

TOKEN test_seq_8[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_1,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_8[] =
  {
   
    {TC_REG_N,   0},
    {TC_MUST_BE, 0xc0000005},
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0310732},
    {TC_END_SECTION, 0},

    {TC_REG_N,   0},
    {TC_MUST_BE, 0xc0001234},
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xc0005678},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x3},
    {TC_MUST_BE, 0xc5310732},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,   0x4},
    {TC_MUST_BE, 0x12345678},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_8_store =
  {
    {
      SW_PLUS(0x05310732),
      0x12345678,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x20002001,
      0x20002103,
      0x20012204,
      -1},
  };


////////////////////////////////////////////////////////////////////////////////
//
// Test 9
//
// Single address instrcution branches
// 
// 

INIT_INFO test_init_9[] =
  {
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_MINUS(0x02)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_PLUS(0x20)},

    {IC_END,          0},
  };

TOKEN test_seq_9[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,             // Execute NOP
    TOK_KEY_C,             
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_9[] =
  {
   
    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000002},
    {TC_REG_LINK,   0},
    {TC_MUST_BE, 0x00000001},
    {TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000001},
    {TC_REG_LINK,   0},
    {TC_MUST_BE, 0x00000003},
    {TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000030},
    {TC_REG_LINK,   0},
    {TC_MUST_BE, 0x00000002},
    {TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000018},
    {TC_REG_LINK,   0},
    {TC_MUST_BE, 0x00000031},
    {TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000024},
    {TC_REG_LINK,   0},
    {TC_MUST_BE, 0x00000019},
    {TC_END_SECTION, 0},
    
    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000016},
    {TC_REG_LINK,   0},
    {TC_MUST_BE, 0x00000025},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_9_store =
  {
    {
      0x24020000,    // 00  Branch to 02
      0x54100000,    // 01  Branch to 10+(R5) R5=20, so branch to 30
      0x34000000,    // 02  Branch to 00+(R3) R3=01, so branch to 01
      0x00000000,    // 03
      0x00000000,    // 04
      0x00000000,    // 05
      0xC0000006,    // 06
      0x00000024,    // 07
      0x00000000,    // 08
      0x00000000,    // 09
      0x00000000,    // 10
      0x00000000,    // 11
      0x00000000,    // 12
      0x00000000,    // 13
      0x00000000,    // 14
      0x00000000,    // 15
      0x00000000,    // 16
      0x00000000,    // 17
      0x64070000,    // 18 Branch to (07) = 24
      0x00000000,    // 19
      0x00000000,    // 20
      0x00000000,    // 21
      0x00000000,    // 22
      0x00000000,    // 23
      0x00002416,    // 24 NOP , Branch to 16
      0x00000000,    // 25
      0x00000000,    // 26
      0x00000000,    // 27
      0x00000000,    // 28
      0x00000000,    // 29
      0x44200000,    // 30 Branch to 20+(R4) R4=-2, so branch to 18
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 10
//
// Branches
// 
// 

INIT_INFO test_init_10[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_MINUS(0x02)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_PLUS(0x20)},

    {IC_END,          0},
  };

TOKEN test_seq_10[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_10[] =
  {
   
    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000010},
    {TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000011},
    {TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000015},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_10_store =
  {
    {
      0x05002510,    // 00
      0x00000000,    // 01
      0x00000000,    // 02
      0x00000000,    // 03
      0x00000000,    // 04
      0x00000000,    // 05
      0x00000000,    // 06
      0x00000000,    // 07
      0x00000000,    // 08
      0x00000000,    // 09
      0x05102520,    // 10
      0x05102615,    // 11
      0x00000000,    // 12
      0x00000000,    // 13
      0x00000000,    // 14
      0x00000000,    // 15
      0x00000000,    // 16
      0x00000000,    // 17
      0x64070000,    // 18
      0x00000000,    // 19
      0x00000000,    // 20
      0x00000000,    // 21
      0x00000000,    // 22
      0x00000000,    // 23
      0x00000000,    // 24
      0x00000000,    // 25
      0x00000000,    // 26
      0x00000000,    // 27
      0x00000000,    // 28
      0x00000000,    // 29
      0x44200000,    // 30
      -1},
  };
////////////////////////////////////////////////////////////////////////////////
//
// Test 11
//
// FP addition
// 
// 

INIT_INFO test_init_11[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_MINUS(0x02)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_PLUS(0x20)},

    {IC_END,          0},
  };

TOKEN test_seq_11[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    //TOK_KEY_C,
    //TOK_TEST_CHECK_RES,

    //TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

#if EXTRACODE_CODED
TEST_INFO test_res_11[] =
  {
    {TC_STORE_N,   0x21},
    {TC_MUST_BE, 0xC2011320},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x20},
    {TC_MUST_BE, 0xC2008820},

    {TC_END,     0},
  };
#endif

#if EXTRACODE_FRAMEWORK
TEST_INFO test_res_11[] =
  {
    {TC_STORE_N,   0x21},
    {TC_MUST_BE, 0xC3113201},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x20},
    {TC_MUST_BE, 0xC3088201},

    {TC_END,     0},
  };
#endif

TEST_LOAD_STORE test_11_store =
  {
    {
      0x70212223,    // 00  (21) = (22) + (23)
      0x70202324,    // 01  (20) = (23) + (24)
      0x19200000,    // 02  DISPLAY (STOP)
      0x00000000,    // 03
      0x00000000,    // 04
      0x00000000,    // 05
      0x00000000,    // 06
      0x00000000,    // 07
      0x00000000,    // 08
      0x00000000,    // 09
      0x00000000,    // 10
      0x00000000,    // 11
      0x00000000,    // 12
      0x00000000,    // 13
      0x00000000,    // 14
      0x00000000,    // 15
      0x00000000,    // 16
      0x00000000,    // 17
      0x00000000,    // 18
      0x00000000,    // 19
      0x00000000,    // 20
      0x00000000,    // 21
      0xC1000125,    // 22
      0xC3100701,    // 23
      0xD1000125,    // 24
      0x00000000,    // 25
      0x00000000,    // 26
      0x00000000,    // 27
      0x00000000,    // 28
      0x00000000,    // 29
      0x44200000,    // 30
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 12
//
// FP addition
// 
// 

INIT_INFO test_init_12[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_MINUS(0x02)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_PLUS(0x20)},

    {IC_END,          0},
  };

TOKEN test_seq_12[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,
   
    // Run until a stop and then check results
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,

    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
   
#if 0
    TOK_KEY_C,   
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif
   
    TOK_NONE,
  };

TEST_INFO test_res_12[] =
  {
    {TC_STORE_N,     0x21},
    {TC_MUST_BE,     0xD3088201},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x20},
    {TC_MUST_BE,     0xC3113201},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_12_store =
  {
    {
      0x71212223,    // 00
      0x19210000,    // 01
      0x71202324,    // 02
      0x19200000,    // 03
      0x00000000,    // 04
      0x00000000,    // 05
      0x00000000,    // 06
      0x00000000,    // 07
      0x00000000,    // 08
      0x00000000,    // 09
      0x00000000,    // 10
      0x00000000,    // 11
      0x00000000,    // 12
      0x00000000,    // 13
      0x00000000,    // 14
      0x00000000,    // 15
      0x00000000,    // 16
      0x00000000,    // 17
      0x00000000,    // 18
      0x00000000,    // 19
      0x00000000,    // 20
      0x00000000,    // 21
      0xC1000125,    // 22
      0xC3100701,    // 23
      0xD1000125,    // 24
      0x00000000,    // 25
      0x00000000,    // 26
      0x00000000,    // 27
      0x00000000,    // 28
      0x00000000,    // 29
      0x44200000,    // 30
      -1},
  };
////////////////////////////////////////////////////////////////////////////////
//
// Test 13
//
// FP multiply
// 
// 

INIT_INFO test_init_13[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_MINUS(0x02)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_PLUS(0x20)},

    {IC_END,          0},
  };

TOKEN test_seq_13[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif

    TOK_KEY_RUN,

    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
      
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,

    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_13[] =
  {
    {TC_STORE_N,     0x21},
    {TC_MUST_BE,     0xC1000045},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,     0x20},
    {TC_MUST_BE,     0xD2001875},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,     0x10},
    {TC_MUST_BE,     0xC5986959},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_13_store =
  {
    {
      0x72212223,    // 00 (21) = (22) * (23)  +4.5
      0x19210000,    // 01
      0x72202324,    // 02 (20) = (23) * (24)  -18.75
      0x19210000,    // 03
      0x72101112,    // 04 (10) = (11) * (12)  +9.869589 
      0x19210000,    // 05
      0x00000000,    // 06
      0x00000000,    // 07
      0x00000000,    // 08
      0x00000000,    // 09
      0x00000000,    // 10
      0xC5314159,    // 11  3.14159
      0xC5314159,    // 12  3.14159
      0x00000000,    // 13
      0x00000000,    // 14
      0x00000000,    // 15
      0x00000000,    // 16
      0x00000000,    // 17
      0x00000000,    // 18
      0x00000000,    // 19
      0x00000000,    // 20
      0x00000000,    // 21
      0xC0000003,    // 22    +3.0
      0xC1000015,    // 23    +1.5
      0xD1000125,    // 24    -12.5
      0x00000000,    // 25
      0x00000000,    // 26
      0x00000000,    // 27
      0x00000000,    // 28
      0x00000000,    // 29
      0x44200000,    // 30
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 14
//
// FP divide
// 
// 

INIT_INFO test_init_14[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0x1)},
    {IC_SET_REG_N,    4},
    {IC_SET_REG_V,    SW_MINUS(0x02)},
    {IC_SET_REG_N,    5},
    {IC_SET_REG_V,    SW_PLUS(0x20)},

    {IC_END,          0},
  };

TOKEN test_seq_14[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_14[] =
  {
    {TC_STORE_N,     0x21},
    {TC_MUST_BE,     0xC0000005},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,     0x35},
    {TC_MUST_BE,     0xD0000010},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x13},
    {TC_MUST_BE,     0xC0000010},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x16},
    {TC_MUST_BE,     0xC5314286},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,     0x26},
    {TC_MUST_BE,     0xC6318182},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x31},
    {TC_MUST_BE,     0xC3000032},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_14_store =
  {
    {
      0x73212223,    // 00 (21) = (22) / (23)   +5.0
      0x19210000,    // 01
      0x73353412,    // 02 (35) = (34) / (12)   -10
      0x19210000,    // 03
      0x73131415,    // 04 (13) = (14) / (15)
      0x19210000,    // 05
      0x73161718,    // 06 (16) = (17) / (18)
      0x19210000,    // 07
      0x93282930,    // 08 ((28)) = ((29)) / ((30)) or: (26) = (18) / (17)
      0x19210000,    // 09
      0x73313233,    // 10 (31) = (32)/ (33)
      0x19210000,    // 11

      0xD0000010,    // 12    -10.0
      0x00000000,    // 13
      0xC2010000,    // 14    +100.00
      0xC1000100,    // 15    +10.0
      0x00000000,    // 16
      0xC4220000,    // 17    +22.00
      0xC0000007,    // 18    +7
      0x00000000,    // 19 
      0xC0000004,    // 20    +4
      0xC0000125,    // 21    +125
      0xC0000015,    // 22    +15
      0xC1000030,    // 23    +3.0
      0xD0000125,    // 24    -125
      0xD1000050,    // 25    -5.0
      0xC0000022,    // 26    +22
      0x00000000,    // 27
      0xC0000026,    // 28    +26
      0xC0000018,    // 29    +18
      0xC0000017,    // 30    +17
      0x00000000,    // 31
      0xC0000004,    // 32    +4
      0xC0000125,    // 33    +125
      0xC2010000,    // 34    +100.00
      0x00000000,    // 35
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test SAV
//
// Surface area and volume of a sphere
// 
// 

INIT_INFO test_init_15[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_15[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_1,
    TOK_KEY_1,
    TOK_KEY_LOAD_IAR,

    // Skip the entry instruction, nothing changes
    //   TOK_KEY_C,

    // Calculate r * r

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif

   
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_15[] =
  {
    {TC_STORE_N,     0x03},
    {TC_MUST_BE,     0xC2000625},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,     0x50},
    {TC_MUST_BE,     0xC4196349},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x51},
    {TC_MUST_BE,     0xC4785396},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x52},
    {TC_MUST_BE,     0xC3196349},
    {TC_END_SECTION, 0},
   
    {TC_STORE_N,     0x53},
    {TC_MUST_BE,     0xC4654497},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_15_store =
  {
    {
      0x00000000,    // 00
      0x00000000,    // 01
      0xC1000025,    // 02  Set up r
      0x00000000,    // 03
      0x00000000,    // 04
      0xC5314159,    // 05  Set up Pi
      0xC0000004,    // 06  Constant 4
      0xC0000003,    // 07  Constant 3
      0x00000000,    // 08
      0x00000000,    // 09
      0x78020507,    // 10  Input r; display 3.14159 and 3 (not executed)
      0x72030202,    // 11  a <- r * r
      0x19030000,    // 12    Stop for result check (2.5 * 2.5 = 6.25)
      0x72500305,    // 13  a <- a * 3.14159
      0x19030000,    // 14    Stop (a = 19.6349)
      0x72515006,    // 15  a <- a * 4
      0x19030000,    // 16    Stop (a = 78.5398)
      0x72525102,    // 17  b <- a * r
      0x19040000,    // 18    Stop (a = 196.349)
      0x73535207,    // 15  b = b / 3
      0x19040000,    //       Stop (b = 65.497) 65.44792
      0x79020304,    // 16  Display r, a and b
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 16
//
// Floating point branch
// 
// 
////////////////////////////////////////////////////////////////////////////////

INIT_INFO test_init_16[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_16[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif
   
    // Run until a stop and then check results
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_16[] =
  {
    {TC_REG_IAR,   0},
    //{TC_MUST_BE, 0x00000010},
    //{TC_END_SECTION, 0},

    {TC_REG_IAR,   0},
    {TC_MUST_BE, 0x00000011},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_16_store =
  {
    {
      0x74101516,    // 00 branch to Aa1 if (Aa2) = (Aa3) 
      0x19200000,    // 01   Stop on error branch
      0x00000000,    // 02
      0x00000000,    // 03
      0x00000000,    // 04
      0x19200000,    // 05   Stop on erroneous branch
      0x00000000,    // 06
      0x00000000,    // 07
      0x00000000,    // 08
      0x00000000,    // 09
      0x74051415,    // 10  Branch to 05 if (14) = (15)
      0x19200000,    // 11
      0x00000000,    // 12
      0x00000000,    // 13
      0x00000000,    // 14
      0xC2000500,    // 15
      0xC1000050,    // 16
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 17
//
// Instructions 1[0-3]
// 
// 

INIT_INFO test_init_17[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x112233)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0xC600000000314159L)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_PLUS (0xC600012345314159L)},

    {IC_END,          0},
  };

TOKEN test_seq_17[] =
  {
    TOK_KEY_NORMAL_RESET,
    TOK_KEY_1,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif

    // Run until a stop and then check results
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_17[] =
  {
   
    {TC_REG_N,   8},
    {TC_MUST_BE, 0xC000000000314160},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xC000000000314162L},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xC000000000314171L},
    {TC_END_SECTION, 0},

    {TC_REG_N,   8},
    {TC_MUST_BE, 0xC600012345314159L},
    {TC_END_SECTION, 0},

    {TC_REG_N,   9},
    {TC_MUST_BE, 0xC000000000112233L},
    {TC_END_SECTION, 0},

    {TC_REG_N,   1},
    {TC_MUST_BE, 0xC0314159},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_17_store =
  {
    {
      SW_PLUS(0x05310732),  // 00
      0x12345678,           // 01
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x0,
      0x00811900,          // 10  
      0x00821900,          // 10  
      0x00891900,
      0x13891900,
      0x13901900,
      0x13181900,
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 18
//
// Quick FP test
// 
// 

INIT_INFO test_init_18[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x112233)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0xC600000000314159L)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_PLUS (0xC600012345314159L)},

    {IC_END,          0},
  };

TOKEN test_seq_18[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_18[] =
  {
    {TC_STORE_N,     0x10},
    {TC_MUST_BE,     0xC6142857},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x13},
    {TC_MUST_BE,     0xC6142857},
    {TC_END_SECTION, 0},

    {TC_STORE_N,     0x07},
    {TC_MUST_BE,     0xD6000353},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_18_store =
  {
    {
      0x73101112,     //00 (10) = (11) / (12)
      0x19200000,     //01   Stop for results check
      0x73131415,     //02 (13) = (14) / (15)
      0x19200000,     //03   Stop for results check
      0x73070809,     //04 (07) = (08) / (09)
      0x19200000,     //05   Stop for results check
      0x00000000,     //06
      0x00000000,     //07
      0xC6001410,     //08
      0xD5400000,     //09
      0x00000000,     //10
      0xC5100000,     //11
      0xC5700000,     //12
      0x00000000,
      0xC0000001,     //14
      0xC0000007,     //15
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 19
//
// Stop (19)
// 
// 

INIT_INFO test_init_19[] =
  {
    {IC_SET_REG_N,    0},
    {IC_SET_REG_V,    SW_PLUS(0x112233)},
    {IC_SET_REG_N,    1},
    {IC_SET_REG_V,    SW_PLUS(0x0)},
    {IC_SET_REG_N,    8},
    {IC_SET_REG_V,    DW_PLUS (0xC600000000314159L)},
    {IC_SET_REG_N,    9},
    {IC_SET_REG_V,    DW_PLUS (0xC600012345314159L)},

    {IC_END,          0},
  };

TOKEN test_seq_19[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

#if 0   
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,             // 0000
   
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,             // 0000
   
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif
   
    TOK_NONE,
  };

TEST_INFO test_res_19[] =
  {
   
    {TC_REG_N,   0},
    {TC_MUST_BE, 0xC0000004},
    //{TC_END_SECTION, 0},

    {TC_REG_N,   1},
    {TC_MUST_BE, 0xC0000005},
    {TC_END_SECTION, 0},

    //{TC_MUST_BE_STOPPED, 0},
    //{TC_END_SECTION, 0},

    {TC_REG_N,   0},
    {TC_MUST_BE, 0xC0000007},
    //{TC_END_SECTION, 0},

    {TC_REG_N,   1},
    {TC_MUST_BE, 0xC0000008},
    {TC_END_SECTION, 0},

    //{TC_MUST_BE_STOPPED, 0},
    //{TC_END_SECTION, 0},

    {TC_REG_N,   0},
    {TC_MUST_BE, 0xC0000001},
    {TC_END_SECTION, 0},

#if 0
    {TC_REG_N,   1},
    {TC_MUST_BE, 0xC0000002},
    {TC_MUST_BE_NOT_STOPPED, 0},
#endif
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_19_store =
  {
    {
      0x03040315,    //00  R0 <-4, R1 <- 5
      0x19010000,    //01  Stop and display R0 and R1, NOP
      0x03070318,    //02  R0<- 7, R1 <- 8
      0x19010000,    //03  Stop and display R0 and R1, NOP
      0x03010312,    //04  R0 <- 1, R1 <- 2
      0x19010000,    //05  Stop
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,     //10
      0xC4062500,     //11
      0xC5314159,     //12
      0x00000000,     //13
      0xC0000001,
      0xC0000007,
      -1},
  };
////////////////////////////////////////////////////////////////////////////////
//
// Test 20
//
// Solution of two linear simultaneous equations
// 
// 

INIT_INFO test_init_20[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_20[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_2,
    TOK_KEY_1,
    TOK_KEY_LOAD_IAR,

    // Run program to end
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

#if 0
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
#endif
   
    TOK_NONE,
  };

TEST_INFO test_res_20[] =
  {
    {TC_STORE_N,   0x09},
    {TC_MUST_BE, 0xC6326471},

    {TC_STORE_N,   0x10},
    {TC_MUST_BE, 0xD5003621},

    {TC_END,     0},
  };

// ad-bc = 125.4392
// dp-bq = 40.952168
// aq-cp = -4.542101
// x = 0.32647025
// y = -0.03620958

TEST_LOAD_STORE test_20_store =
  {
    {
      0x00000000,    // 00 spare
      0x00000000,    // 01 spare
      0x00000000,    // 02 spare
      0xC0000010,    // 03 a     10
      0xC1000034,    // 04 b     3.4
      0xC5314159,    // 05 p     3.14159
      0xC3007812,    // 06 c     7.812
      0xC1000152,    // 07 d     15.2
      0xC0000002,    // 08 q     2
      0x00000000,    // 09 r     
      0x00000000,    // 10 s
      0x00000000,    // 11 u
      0x00000000,    // 12 v
      0x00000000,    // 13 spare
      0x00000000,    // 14 spare
      0x78030303,    // 15
      0x78040303,    // 16
      0x78050403,    // 17
      0x78060504,    // 18
      0x78070605,    // 19
      0x78080706,    // 20
      0x72090705,    // 21
      0x72100408,    // 22
      0x71090910,    // 23
      0x72100307,    // 24
      0x72110406,    // 25
      0x71101011,    // 26
      0x73090910,    // 27
      0x72110308,    // 28
      0x72120605,    // 29
      0x71111112,    // 30
      0x73101110,    // 31
      0x79091010,    // 32
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 21
//
// Square root
// 
// 

INIT_INFO test_init_21[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_21[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_9,
    TOK_KEY_LOAD_IAR,

#if 0
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
    TOK_KEY_C,
#endif

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_21[] =
  {
    {TC_STORE_N,   0x3},
    {TC_MUST_BE, 0xC5141422},
    {TC_END,     0},
  };

TEST_LOAD_STORE test_21_store =
  {
    {
      0x00000000,    // 00 spare
      0x00000000,    // 01 spare
      0xC0000002,    // 02 v
      0x00000000,    // 03 q
      0x00000000,    // 04 p
      0xC0000002,    // 05 2
      0xC0000000,    // 06 0
      0x00000000,    // 07 spare
      0x78020506,    // 08 
      0x73030205,    // 09 
      0x70040306,    // 10 
      0x73030204,    // 11 
      0x70030304,    // 12 
      0x73030305,    // 13 
      0x74160304,    // 14 
      0x74100505,    // 15
      0x79020304,    // 16
      0x00000000,    // 31
      0x00000000,    // 32
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 22
//
// Sin(x)
// 
// 

INIT_INFO test_init_22[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_22[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_1,
    TOK_KEY_1,
    TOK_KEY_LOAD_IAR,

    //TOK_KEY_RUN,
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,
   
    TOK_NONE,
  };

TEST_INFO test_res_22[] =
  {
    {TC_STORE_N,   0x3},
    {TC_MUST_BE, 0xC6841471},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_22_store =
  {
    {
      0x00000000,    // 00 spare
      0x00000000,    // 01 spare
      0xC0000001,    // 02 x
      0x00000000,    // 03 s
      0x00000000,    // 04 a
      0xC0000002,    // 05 b
      0xC0000000,    // 06 t
      0x00000000,    // 07 0
      0xC0000001,    // 08 1
      0x00000000,    // 09 spare 
      0x78020708,    // 10 
      0x70030707,    // 11 
      0x70040807,    // 12 
      0x72050202,    // 13 
      0x71050705,    // 14 
      0x70060207,    // 15
      0x74220808,    // 16
      0x72060605,    // 17
      0x70040408,    // 18
      0x73060604,    // 19
      0x70040408,    // 20
      0x73060604,    // 21
      0x70030306,    // 22
      0x76170607,    // 23
      0x79020307,    // 24
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 23
//
// FP tests
// 
// 

INIT_INFO test_init_23[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_23[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

#if 0
    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,

    TOK_KEY_C,
    TOK_TEST_CHECK_RES,
#endif

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_23[] =
  {
   
    {TC_STORE_N,   0x10},
    {TC_MUST_BE, 0xC6142857},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x13},
    {TC_MUST_BE, 0xC6142857},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x7},
    {TC_MUST_BE, 0xD6000353},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x16},
    {TC_MUST_BE, 0xC6000014},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x19},
    {TC_MUST_BE, 0xC0000002},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x22},
    {TC_MUST_BE, 0xC0000017},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_23_store =
  {
    {
      0x73101112,    //00
      0x73131415,    //01
      0x73070809,    //02  (07) = (08) / (09)  
      0x73161718,    //03  (16) = (17) / (18)
      0x70192021,    //04  (19) = (20) + (21)
      0x70222324,
      0x19100000,     //06 Stop for result checking
      0x00000000,     //07
      0xC6001410,     //08
      0xD5400000,     //09
      0x00000000,     //10
      0xC5100000,     //11
      0xC5700000,     //12
      0x00000000,     //13
      0xC0000001,     //14
      0xC0000007,     //15
      0x00000000,     //16
      0xC4000001,     //17
      0xC5700000,     //18
      0x00000000,     //19
      0xC5100000,     //20
      0xC5100000,     //21
      0x00000000,     //22
      0xC5900000,     //23
      0xC5800000,     //24
      0x00000000,     //25
      0xC0000001,     //26
      0xC0000007,     //27
      0x00000000,     //28
      0xC0000001,     //29
      0xC0000007,     //30
    
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 24
//
// Set up data for Fig 10 IEC document example
// Run instruction
// 

INIT_INFO test_init_24[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_24[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_9,
    TOK_KEY_5,
    TOK_KEY_LOAD_ADDR,

    TOK_KEY_DOT,
    TOK_KEY_0,
    TOK_KEY_2,
    TOK_KEY_6,
    TOK_KEY_4,
    TOK_KEY_1,
    TOK_KEY_5,
    TOK_KEY_MINUS,
   
    TOK_KEY_LOAD_STORE,
    TOK_TEST_CHECK_RES,
   
    TOK_KEY_2,
    TOK_KEY_5,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,

    TOK_TEST_WAIT_FOR_STOP,
    TOK_TEST_CHECK_RES,

#if 0
    TOK_KEY_A,
    TOK_KEY_B,
    TOK_KEY_C,
#endif
   
    TOK_NONE,
  };

TEST_INFO test_res_24[] =
  {
   
    {TC_STORE_N,   0x95},
    {TC_MUST_BE, 0xD6026415},
    {TC_END_SECTION, 0},

    {TC_STORE_N,   0x03},
    {TC_MUST_BE, 0xD1003552},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_24_store =
  {
    {
      0x00000000,    //00
      0x00000000,    //01
      0x00000000,
      0x00000000,    //03
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,     //07
      0x00000000,     //08
      0x00000000,     //09
      0x00000000,     //10
      0xC5938261,     //11
      0x00000000,     //12
      0x00000000,     //13
      0x00000000,     //14
      0x00000000,     //15
      0x00000000,     //16
      0x00000000,     //17
      0x00000000,     //18
      0x00000000,     //19
      0xC0000003,     //20
      0x00000000,     //21
      0xC0000095,     //22
      0x00000000,     //23
      0xC0000011,     //24
      0x93202422,     //25
      0x19200000,     //26   Stop and check results
      0x00000000,     //27
      0x00000000,     //28
      0x00000000,     //29
      0x00000000,     //30
    
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 25
//
// Set up data for Fig 11 IEC document example
// Run instruction
// 

INIT_INFO test_init_25[] =
  {
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0xC0000071)},

    {IC_END,          0},
  };

TOKEN test_seq_25[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_6,
    TOK_KEY_8,
    TOK_KEY_LOAD_ADDR,

    TOK_KEY_8,
    TOK_KEY_2,
    TOK_KEY_1,
    TOK_KEY_DOT,
    TOK_KEY_3,
    TOK_KEY_4,
    TOK_KEY_6,
    TOK_KEY_MINUS,
   
    TOK_KEY_LOAD_STORE,
    TOK_TEST_CHECK_RES,
   
    TOK_KEY_1,
    TOK_KEY_7,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_A,
    TOK_KEY_B,
    TOK_KEY_C,

    TOK_TEST_CHECK_RES,

    TOK_KEY_1,
    TOK_KEY_8,

    TOK_KEY_LOAD_IAR,

    TOK_KEY_A,
    TOK_KEY_B,
    TOK_KEY_C,

    TOK_TEST_CHECK_RES,

   
    TOK_NONE,
  };

TEST_INFO test_res_25[] =
  {
   
    {TC_STORE_N,   0x68},
    {TC_MUST_BE, 0xD3821346},
    {TC_END_SECTION, 0},

    {TC_REG_N,   0x00},
    {TC_MUST_BE, 0xC0000003},

    {TC_REG_N,   0x01},
    {TC_MUST_BE, 0xD0821346},

    {TC_END_SECTION, 0},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_25_store =
  {
    {
      0x00000000,    //00
      0x00000000,    //01
      0x00000000,
      0x00000000,    //03
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,     //07
      0x00000000,     //08
      0x00000000,     //09
      0x00000000,     //10
      0x00000000,     //11
      0x00000000,     //12
      0x00000000,     //13
      0x00000000,     //14
      0x00000000,     //15
      0x00000000,     //16
      0x20680000,     //17
      0x05321089,     //18
      0x38640000,     //19
      0x00000000,     //20
      0x00000000,     //21
      0x00000000,     //22
      0x00000000,     //23
      0x00000000,     //24
      0x00000000,     //25
      0x00000000,     //26
      0x00000000,     //27
      0x00000000,     //28
      0x00000000,     //29
      0x00000000,     //30
    
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 26
//
// Set up the store so this program can be run as a sequence.
// Designed to test the input and display/stop instructions.
// 

INIT_INFO test_init_26[] =
  {
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0xC0000071)},

    {IC_END,          0},
  };

TOKEN test_seq_26[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_1,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_NONE,
  };

TEST_INFO test_res_26[] =
  {
   
    {TC_STORE_N,   0x68},
    {TC_MUST_BE, 0xD3821346},
    {TC_END_SECTION, 0},

    {TC_REG_N,   0x00},
    {TC_MUST_BE, 0xC0000003},

    {TC_REG_N,   0x01},
    {TC_MUST_BE, 0xD0821346},

    {TC_END_SECTION, 0},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_26_store =
  {
    {
      0x00000000,    //00
      0x00000000,    //01
      0x00000000,
      0x00000000,    //03
      0x00000000,
      0x00000000,
      0x00000000,
      0x00000000,     //07
      0x00000000,     //08
      0x00000000,     //09
      0x78010203,     //10
      0x70010101,     //11
      0x79010910,     //12
      0x00000000,     //13
      0x00000000,     //14
      0x00000000,     //15
      0x00000000,     //16
      0x00000000,     //17
      0x00000000,     //18
      0x00000000,     //19
      0x00000000,     //20
      0x00000000,     //21
      0x00000000,     //22
      0x00000000,     //23
      0x00000000,     //24
      0x00000000,     //25
      0x00000000,     //26
      0x00000000,     //27
      0x00000000,     //28
      0x00000000,     //29
      0x00000000,     //30
    
      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 27
//
// Log N program from the flowchart document
// This can't be run as it uses the 74xxxxxx square root jump instruction
//
// Set up the store so this program can be run as a sequence.
//
// 

INIT_INFO test_init_27[] =
  {
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0xC0000071)},

    {IC_END,          0},
  };

TOKEN test_seq_27[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_1,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_NONE,
  };

TEST_INFO test_res_27[] =
  {
   
    {TC_STORE_N,   0x68},
    {TC_MUST_BE, 0xD3821346},
    {TC_END_SECTION, 0},

    {TC_REG_N,   0x00},
    {TC_MUST_BE, 0xC0000003},

    {TC_REG_N,   0x01},
    {TC_MUST_BE, 0xD0821346},

    {TC_END_SECTION, 0},
   
    {TC_END,     0},
  };

TEST_LOAD_STORE test_27_store =
  {
    {
      //----------------------------------------
      // Program for L <- Logb N
      //
      0x00000000,     //00          L
      0x00000000,     //01          J
      0x00000000,     //02          N
      0x00000000,     //03          b
      0x00000000,     //04          x, P
      0x00000000,     //05          2 E6  |
      0x00000000,     //06          2     | Constants (load with program)
      0x00000000,     //07          Q     |
      0x03000310,     //08  START   R0 <- 0    R1<-0
      0x21000311,     //09          L <- 0     E1<-1
      0x21010000,     //10          J <- 1
      0x28022803,     //11          Input N     Input b
      0x20022104,     //12          x <- N
      0x71290403,     //13  A       \ * Branch to B if b>x
      0x74292918,     //14          /   (i.e. if x-b<0)
      0x73040403,     //15          x <- x / b
      0x20000011,     //16          L <- L + 1
      0x21002413,     //17                      Branh to A
      0x73010106,     //18  B       J <- J / 2
      0x71290105,     //19          \  * Branch to C if
      0x74292927,     //20          /  J - (2 E -6) < 0
      0x72040404,     //21          x <- x * x
      0x71290403,     //22          \  * Branch to B if
      0x74292918,     //23          /    x - b < 0
      0x73040403,     //24          x <- x / b
      0x70000001,     //25          L <- L + J
      0x24180000,     //26          Branch to B
      0x79020300,     //27  C       Display N, b, L
      0x28080000,     //28          Branch to start
      0x00000000,     //29           Working

      // * Branches employ the square root subroutine
      //
      //----------------------------------------
      //
      // Program for N <- b ^ L
      //
      0xC0000001,     //30          1    Constant (load with program)
      0x73023030,     //31  START   N <- 1
      0x28032800,     //32          Input b    Input L
      0x20032101,     //33          J <- b
      0x20001710,     //34          (R0R1) <- L
      0x03002104,     //35          R0<-0   P<-[L]
      0x71070004,     //36          Q <- L-P
      0x20040510,     //37  A       | * Branch to B if P = 0
      0x25420000,     //38          |
      0x01112104,     //39          P <- P - 1
    
      0x72020203,     //40          N <- N * b
      0x24370000,     //41          Branch to A
      0x74010130,     //42  B       J <- sqrt(J)
      0x20010111,     //43          |  * Branch to C if J = 1
      0x05102551,     //44          | 
      0x70070707,     //45          Q <- Q + Q
      0x71290730,     //46          | Branch to B if Q - 1 < 0
      0x74292942,     //47          |
      0x71070730,     //48          Q <- Q - 1
      0x72020201,     //49          N <- N * J
      0x24420000,     //50          Branch to B
      0x79030002,     //51  C       Display b, L, N
      0x24310000,     //52          Branch to start

      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 28
//
// Set up the store so this program can be run as a sequence.
//
// 

INIT_INFO test_init_28[] =
  {
    {IC_SET_REG_N,    3},
    {IC_SET_REG_V,    SW_PLUS(0xC0000071)},

    {IC_END,          0},
  };

TOKEN test_seq_28[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_1,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,

    TOK_TEST_WAIT_FOR_STOP,
    //TOK_KEY_A,
    //TOK_KEY_B,
    //TOK_KEY_C,

    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_28[] =
  {
   
    {TC_STORE_N,   0x20},
    {TC_MUST_BE, 0xC0200000},
    {TC_END_SECTION, 0},

    {TC_END,     0},
  };

TEST_LOAD_STORE test_28_store =
  {
    {
      //----------------------------------------
      // Program for L <- Logb N
      //
      0x00000000,  // 00
      0x00000000,  // 01
      0x00000000,  // 02
      0x00000000,  // 03
      0x00000000,  // 04
      0x00000000,  // 05
      0x00000000,  // 06
      0x00000000,  // 07
      0x03000310,  // 08
      0x21000311,  // 09
      0x70202122,  // 10
      0x28022803,  // 11
      0x20022104,  // 12
      0x71290403,  // 13
      0x74292918,  // 14
      0x73040403,  // 15
      0x20000011,  // 16
      0x21002413,  // 17
      0x73010106,  // 18
      0x71290105,  // 19
      0xC0222222,  // 20
      0xC0100000,  // 21
      0xC0100000,  // 22
      0x74292918,  // 
      0x73040403,  // 
      0x70000001,  // 
      0x24180000,  // 
      0x79020300,  // 
      0x28080000,  // 
      0x00000000,  

      -1},
  };

////////////////////////////////////////////////////////////////////////////////
//
// Test 29
//
// Stop, enter a
// Stop, enter b
// Divide a / b -> c
// Stop, display a, b, c
// 

INIT_INFO test_init_29[] =
  {
    {IC_END,          0},
  };

TOKEN test_seq_29[] =
  {
    TOK_KEY_NORMAL_RESET,

    TOK_KEY_1,
    TOK_KEY_0,
    TOK_KEY_LOAD_IAR,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,

    TOK_KEY_1,
    TOK_KEY_DOT,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,

    TOK_KEY_7,
    TOK_KEY_DOT,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,

    // result 1
    TOK_TEST_CHECK_RES,


    // Enter values using three address instructions
    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,

    TOK_KEY_9,
    TOK_KEY_5,
    TOK_KEY_DOT,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,

    TOK_KEY_2,
    TOK_KEY_DOT,

    TOK_KEY_RUN,
    TOK_TEST_WAIT_FOR_STOP,

    // Result 2
    TOK_TEST_CHECK_RES,

    TOK_NONE,
  };

TEST_INFO test_res_29[] =
  {
    {TC_STORE_N,   0x00},
    {TC_MUST_BE,   0xC6142857},
    {TC_END_SECTION, 0},
    
    {TC_STORE_N,   0x00},
    {TC_MUST_BE,   0xC1000475},
    {TC_END,       0},
  };

TEST_LOAD_STORE test_29_store =
  {
    {
      0x00000000,    // 00 c
      0x00000000,    // 01 a
      0x00000000,    // 02 b
      0x00000000,    // 03 
      0x00000000,    // 04 
      0x00000000,    // 05 
      0x00000000,    // 06 
      0x00000000,    // 07
      0x00000000,    // 08 
      0x00000000,    // 09 
      0x28012802,    // 10  input a    input b 
      0x73000102,    // 11  divide
      0x79000102,    // 12  display and stop
      0x78010202,    // 13  stop and input a
      0x78020102,    // 14  stop and input b
      0x73000102,    // 15  divide
      0x79000102,    // 16  display and stop
      0x00000000,    // 17
      0x00000000,    // 18
      -1},
  };

////////////////////////////////////////////////////////////////////////////////

ESC_TEST_INFO tests[] =
  {
    {"KB Input",                test_init_0,  test_seq_0,  test_res_0,  0, &test_0_store,  ""},
    {"Reg Inst 0[0-3],1[0-3]",  test_init_1,  test_seq_1,  test_res_1,  0, &test_1_store,  ""},
    {"Test 2",                  test_init_2,  test_seq_2,  test_res_2,  0, &test_2_store,  ""},
    {"ADDR inc/dec",            test_init_3,  test_seq_3,  test_res_3,  0, &test_3_store,  ""},
    {"RH 6 Digits",             test_init_4,  test_seq_4,  test_res_4,  0, &test_4_store,  ""},
    {"TEST",                    test_init_5,  test_seq_5,  test_res_5,  0, &test_5_store,  ""},
    {"Left Shift",              test_init_6,  test_seq_6,  test_res_6,  0, &test_6_store,  ""},
    {"Right Shift",             test_init_7,  test_seq_7,  test_res_7,  0, &test_7_store,  ""},
    {"Inst 20",                 test_init_8,  test_seq_8,  test_res_8,  0, &test_8_store,  ""},
    {"Branches",                test_init_9,  test_seq_9,  test_res_9,  0, &test_9_store,  ""},
    {"CL=1,0 Branches",         test_init_10, test_seq_10, test_res_10, 0, &test_10_store, ""},
    {"Fp Addition",             test_init_11, test_seq_11, test_res_11, 0, &test_11_store, ""},
    {"Fp Subtraction",          test_init_12, test_seq_12, test_res_12, 0, &test_12_store, ""},
    {"Fp Multiply",             test_init_13, test_seq_13, test_res_13, 0, &test_13_store, ""},
    {"Fp Divide",               test_init_14, test_seq_14, test_res_14, 0, &test_14_store, ""},
    {"Surface & volume",        test_init_15, test_seq_15, test_res_15, 0, &test_15_store, ""},
    {"Fp Branches",             test_init_16, test_seq_16, test_res_16, 0, &test_16_store, ""},
    {"Inst [0-1][0-3] DW",      test_init_17, test_seq_17, test_res_17, 0, &test_17_store, ""},
    {"FP test",                 test_init_18, test_seq_18, test_res_18, 0, &test_18_store, ""},
    {"Stop (19)",               test_init_19, test_seq_19, test_res_19, 0, &test_19_store, ""},
    {"Simultaneous Eq",         test_init_20, test_seq_20, test_res_20, 0, &test_20_store, ""},
    {"Square root",             test_init_21, test_seq_21, test_res_21, 0, &test_21_store, ""},
    {"sin(x)",                  test_init_22, test_seq_22, test_res_22, 0, &test_22_store, ""},
    {"FP tests",                test_init_23, test_seq_23, test_res_23, 0, &test_23_store, ""},
    {"Fig 10",                  test_init_24, test_seq_24, test_res_24, 0, &test_24_store, ""},
    {"Fig 11",                  test_init_25, test_seq_25, test_res_25, 0, &test_25_store, ""},
    {"Load Prog",               test_init_26, test_seq_26, test_res_26, 0, &test_26_store, ""},
    {"Load Log N",              test_init_27, test_seq_27, test_res_27, 0, &test_27_store, ""},
    {"Double 100000",           test_init_28, test_seq_28, test_res_28, 0, &test_28_store, ""},
    {"Input/Disp",              test_init_29, test_seq_29, test_res_29, 0, &test_29_store, ""},
   
    {"--END--",                 test_init_1,  test_seq_1,  test_res_1,  0, &test_1_store,  ""},
  };
  
////////////////////////////////////////////////////////////////////////////////
//
// Run tests
//
//
// Each test is a sequence of inputs to the state machine
//
// The store and registers can be loaded before the test starts.
// The store and registers can be tested after the test has ended.
// The store and registers can be tested at any point in the test run.
//

void cli_load_test_code_into_store(void)
{
  ESC_STATE *s = &esc_state;
    
  load_store_from_test(s, parameter);
}

void cli_run_single_test(void)
{
  
  test_number = parameter;

  // Assume passed
  tests[test_number].passed = 1;
  tests[test_number].fail_text[0] = '\0';
  
  test_run_single_test = 1;
  test_running   = 1;
  test_done_init = 0;

  
  clear_test_fail_buffer();
}

void cli_run_tests(void)
{
  // Set test number to first test
  test_number = 0;

  int done = 0;
  int t = 0;
  
  while(!done)
    {
      if( strcmp( tests[t].desc, "--END--") == 0 )
	{
	  done = 1;
	  continue;
	}

      tests[test_number].fail_text[t] = '\0';
      tests[t].passed = 1;
      t++;
    }
  
  test_run_single_test = 0;
  test_running   = 1;
  test_done_init = 0;
  clear_test_fail_buffer();
}

void cli_test_results(void)
{
  printf("\n");
  printf("\nTest results");
  printf("\n============\n");
  
  if( test_running )
    {
      printf("\nRunning test:%d", test_number);
    }
  else
    {
      printf("\nNo tests running");
    }

  printf("\n");
  
  for(int i=0; i<NUM_TESTS; i++)
    {
      if( strcmp(tests[i].desc, "--END--") != 0 )
	{
	  printf("\n%03d: %-30s   %-10s %s", i, tests[i].desc, tests[i].passed?"Passed":"Failed", tests[i].fail_text);
	}
    }

  printf("\n");

}

////////////////////////////////////////////////////////////////////////////////
//
//
// We allow digits to enter a new value for the parameter

void cli_enter_parameter()
{
  int  key;
  int done = 0;

  printf("\nEnter parameter: (ESC or <RETURN> to exit)");
  
  parameter = 0;
  parameter_a_flag = 0;
  
  while(!done)
    {
      if( ((key = getchar_timeout_us(1000)) != PICO_ERROR_TIMEOUT))
	{
	  switch(key)
	    {
	    case '0':
	    case '1':
	    case '2':
	    case '3':
	    case '4':
	    case '5':
	    case '6':
	    case '7':
	    case '8':
	    case '9':
	      parameter *= 10;
	      parameter += (key - '0');
	      prompt();
	      break;

              // A digit for IAR entry of breakpoint
            case 'A':
            case 'a':
              parameter_a_flag = 1;
              break;

#if 0
            case '-':
	      parameter *= 10;
	      parameter += 0xD;
	      prompt();
              break;

            case '+':
	      parameter *= 10;
	      parameter += 0xC;
	      prompt();
              break;
#endif         
	    case 27:
	    case 13:
	    case 10:
	      done = 1;
	      break;
	      
	    default:
	      break;
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
}

////////////////////////////////////////////////////////////////////////////////
//
// Reads and prints a file
//

int cat_file(char *fn)
{
  char line[MAX_FILE_LINE];
  char fileline[MAX_FILE_LINE];

  mount_sd();
  
  if( !cd_to_dir(ESC_DIR) )
    {
      unmount_sd();
      return(0);
    }
  
  sprintf("Reading '%s'", fn);

  FF_FILE *fp = ff_fopen(fn, "r");

  if (fp == NULL)
    {
      printf("Failed to open:%s", fn);
      unmount_sd();
      return(0);
    }
  
  // Get lines from the file
  while( ff_fgets(&(fileline[0]), sizeof(fileline)-1, fp) != NULL )
    {
      printf("%s", fileline);
    }
  
  ff_fclose(fp);
  unmount_sd();
  return(1);
}

////////////////////////////////////////////////////////////////////////////////
//
// Reads a state file and loads an ES state structure with that data
//

FIELD_INFO  field_info[] =
  {
    {"*IAR_ADDRESS:",            wfn_iar_address},
    {"*IAR_A_FLAG:",             wfn_iar_a_flag},
    {"*BREAKPOINT_ADDRESS:",     wfn_breakpoint_address},
    {"*BREAKPOINT_A_FLAG:",      wfn_breakpoint_a_flag},
    {"*KB_REGISTER:",            wfn_kb_register},
#if 0
    {"*ADDRESS_REGISTER_%*d:",   wfn_address_register},
    {"*ADDRESS_REGISTER_%*d:",   wfn_address_register},
    {"*ADDRESS_REGISTER_%*d:",   wfn_address_register},
#endif
    {"*INSTRUCTION_REGISTER:",   wfn_instruction_register},
    {"*LINK_REGISTER:",          wfn_link_register},
    {"*LINK_REGISTER:",          wfn_store},
    {"*LINK_REGISTER:",          wfn_store_data},
    {"*SUPPRESSED_DISPLAY",      wfn_suppressed_display},
    {"*STEP_EXTRACODE",          wfn_step_extracode},
  };

#define NUM_FIELD_INFO (sizeof(field_info)/sizeof(FIELD_INFO))

int wfn_iar_address(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*IAR_ADDRESS:%X", &value) == 1 )
    {
      es->iar.address = (ADDRESS)value;
      
      printf("\nIAR address now:%08X", es->iar.address);
      return(1);
    }
  
  return(0);
}

int wfn_breakpoint_address(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*BREAKPOINT_ADDRESS:%X", &value) == 1 )
    {
      es->breakpoint.address = (ADDRESS)value;
      
      printf("\nBreakpoint now:%03X", es->breakpoint.address);
      return(1);
    }
  
  return(0);
}

int wfn_breakpoint_a_flag(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*BREAKPOINT_A_FLAG:%X", &value) == 1 )
    {
      es->breakpoint.a_flag = (BOOLEAN) value;
      printf("\nIAR a_flag now:%08X", es->breakpoint.a_flag);
      return(1);
    }
  
  return(0);
}


int wfn_iar_a_flag(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*IAR_A_FLAG:%X", &value) == 1 )
    {
      es->iar.a_flag = (BOOLEAN) value;
      printf("\nIAR a_flag now:%08X", es->iar.a_flag);
      return(1);
    }
  
  return(0);
}

int wfn_kb_register(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value = 0;
  
  if( sscanf(line, "*KB_REGISTER:%X", &value) == 1 )
    {
      es->keyboard_register = (SINGLE_WORD)value;
      
      printf("\nKEYBOARD REGISTER now:%08X", es->keyboard_register);
      return(1);
    }

  return(0);
}

#if 0
int wfn_address_register(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int index;
  int value;
  
  if( sscanf(line, "*ADDRESS_REGISTER_%d:%X", &index, &value) == 2 )
    {
      
      switch(index)
	{
	case 0:
	  es->address_register0 = (ADDRESS)value;
	  break;
	  
	case 1:
	  es->address_register1 = (ADDRESS)value;
	  break;
	  
	case 2:
	  es->address_register2 = (ADDRESS)value;
	  break;
	}
      
      printf("\nADDRESS_REGISTER_%d now:%08X", index, value);
      return(1);
    }

  return(0);
}
#endif

int wfn_instruction_register(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*INSTRUCTION_REGISTER:%X", &value) == 1 )
    {
      es->instruction_register = (SINGLE_WORD) value;
      printf("\nINSTRUCTION REGISTER now:%08X", es->instruction_register);
      return(1);
    }

  return(0);
}

int wfn_link_register(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*LINK_REGISTER:%X", &value) == 1 )
    {
      es->link_register = (ADDRESS)value;
      
      printf("\nLINK REGISTER now:%08X", es->link_register);
      return(1);
    }

  return(0);
}

int reading_store_data = 0;
int reading_store_index = 0;

int wfn_store(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( strncmp(line, "*STORE:", strlen("*STORE:")) == 0 )
    {
      // Now reading store data, set flag so store data function
      // knows data is for it
      reading_store_data = 1;
      printf("\nReading store data");

      return(1);
    }

  return(0);
}

int wfn_store_data(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  int d[STORE_DATA_FILE_CHUNK_SIZE];
  
  if( reading_store_data )
    {
      // If line is in correct format then we read it, otherwise turn off
      // reading of data
      if( sscanf(line, "*%X %X %X %X %X %X %X %X",
		 &(d[0]), &(d[1]), &(d[2]), &(d[3]),
		 &(d[4]), &(d[5]), &(d[6]), &(d[7])
		 ) == 8 )
	{
	  for( int i=0; i<STORE_DATA_FILE_CHUNK_SIZE; i++)
	    {
	      es->store[reading_store_index++] = (SINGLE_WORD) d[i];
	    }
	  
	  return(1);
	}
      else
	{
	  // Not correct format, data must have ended
	  reading_store_data = 0;
	  reading_store_index = 0;

	  return(0);
	}
    }

  return(0);
}

int wfn_suppressed_display(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*SUPPRESSED_DISPLAY:%X", &value) == 1 )
    {
      suppressed_display = (ADDRESS)value;
      
      printf("\nSuppressed_display now:%08X", suppressed_display);
      return(1);
    }
  
  return(0);
}

int wfn_step_extracode(ESC_STATE *es, void *fi, char *line)
{
  FIELD_INFO *info = (FIELD_INFO *) fi;
  int value;
  
  if( sscanf(line, "*STEP_EXTRACODE:%X", &value) == 1 )
    {
      suppressed_display = (ADDRESS)value;
      
      printf("\nStep_extracode now:%08X", setup_step_extracode);
      return(1);
    }
  
  return(0);
}

//------------------------------------------------------------------------------

void read_state_field(char *line, ESC_STATE *es)
{
  char fieldname[MAX_FILE_LINE];
  
  // Ignore lines that don't start with a '*'
  if( line[0] != '*' )
    {
      return;
    }

  // Check each line against known statements
  for(int i=0; i<NUM_FIELD_INFO; i++)
    {
      if( (*field_info[i].fn)(&esc_state, &(field_info[i]), line ) )
	{
	  break;
	}
    }
}
  
int read_file_into_state(char *fn, ESC_STATE *es)
{
  char line[MAX_FILE_LINE];
  char fileline[MAX_FILE_LINE];

  mount_sd();
  
  if( !cd_to_dir(ESC_DIR) )
    {
      unmount_sd();
      return(0);
    }
  
  sprintf("Reading '%s'", fn);

  FF_FILE *fp = ff_fopen(fn, "r");

  if (fp == NULL)
    {
      printf("Failed to open:%s", fn);
      unmount_sd();
      return(0);
    }
  
  // Get lines from the file
  while( ff_fgets(&(fileline[0]), sizeof(fileline)-1, fp) != NULL )
    {
      read_state_field(fileline, es);
    }
  
  ff_fclose(fp);
  unmount_sd();
  return(1);
}


////////////////////////////////////////////////////////////////////////////////
//
// Writes the machine state to a file
//
// All files are written in the ESC directory

char *write_store_checksum(FIL *fp, ESC_STATE *s, char *title, int first, int last)
{
  uint32_t csum = checksum_store(s, first, last);
  f_printf(fp, "\n# %s checksum:%08X %s", title, csum, csum_desc(csum));
}

int write_state_to_file(ESC_STATE *es, char *fn)
{
  char filename[20];
  int i;
  FRESULT fr;
  FIL fp;
  int bytes_written = 0;
  char line[40];

  sprintf(filename, "%s", fn);
  
  printf("\nWriting %s", filename);

  mount_sd();
  
  if( !cd_to_dir(ESC_DIR) )
    {
      unmount_sd();
      return(0);
    }
  
  // Open file for writing
  fr = f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE);

  if (FR_OK != fr /*(&& FR_EXIST != fr)*/)
    {
      printf("Open error");
      unmount_sd();
      return(0);
    }

  f_printf(&fp, "# Experimental Schools Computer State File");

  f_printf(&fp, "\n*IAR_ADDRESS:%08X",          es->iar.address);
  f_printf(&fp, "\n*IAR_A_FLAG:%08X",           es->iar.a_flag);
  f_printf(&fp, "\n*BREAKPOINT_ADDRESS:%03X",   es->breakpoint.address);
  f_printf(&fp, "\n*BREAKPOINT_A_FLAG:%03X",    es->breakpoint.a_flag);
  f_printf(&fp, "\n*KB_REGISTER:%08X",          es->keyboard_register);
#if 0
  f_printf(&fp, "\n*ADDRESS_REGISTER_0:%08X",   es->address_register0);
  f_printf(&fp, "\n*ADDRESS_REGISTER_1:%08X",   es->address_register1);
  f_printf(&fp, "\n*ADDRESS_REGISTER_2:%08X",   es->address_register2);
#endif
  f_printf(&fp, "\n*INSTRUCTION_REGISTER:%08X", es->instruction_register);
  f_printf(&fp, "\n*LINK_REGISTER:%08X",        es->link_register);

  f_printf(&fp, "\n*SUPPRESSED_DISPLAY:%08X",   suppressed_display);
  f_printf(&fp, "\n*STEP_EXTRACODE:%08X",       setup_step_extracode  );
    
  for(int i=0;i<NUM_WORD_REGISTERS; i++)
    {
      f_printf(&fp, "\n*R%d:%08X", i, read_register(es, i));
    }

  for(int i=0;i<NUM_DBL_WORD_REGISTERS; i++)
    {
      f_printf(&fp, "\n*R%d:%016lX", i+NUM_WORD_REGISTERS, read_register(es, i));
    }

  f_printf(&fp, "\n*STORE:");
  
  for(int i=0; i<STORE_SIZE; i++)
    {
      if( (i % STORE_DATA_FILE_CHUNK_SIZE) == 0 )
	{
	  f_printf(&fp, "\n*");
	}
      f_printf(&fp, "%08X ", es->store[i]);
    }

  f_printf(&fp, "\n");
  write_store_checksum(&fp, es, "Lower store", LOWER_STORE);
  write_store_checksum(&fp, es, "Upper store", EXTRACODE_CSUM_RANGE);
  write_store_checksum(&fp, es, "All   store", ALL_STORE);
  f_printf(&fp, "\n");
  
  fr = f_close(&fp);

  if (FR_OK != fr)
    {
      printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
      unmount_sd();
      return(0);
    }

  unmount_sd();
  
  return(1);
}


////////////////////////////////////////////////////////////////////////////////

char *paddesc[100] = {
  "00",
  "01",
  "02",
  "03",
  "04",
  "05",
  "06",
  "07",
  "08",
  "09",
  "10",
  "11",
  "12",
  "13",
  "14",
  "15",
  "16",
  "17",
  "18",
  "19",
  "20",
  "21",
  "22",
  "23",
  "24",
  "25",
  "26",
  "27",
  "28",
  "29",
  "30",
  "31",
		      
};

void cli_dump_touch_key_data(void)
{
  char binstr[100];
  int padcnt[32];
  int pressed[32];
  
  for(int i=0; i<32; i++)
    {
      padcnt[i] = 0;
      pressed[i] = 0;
    }
  
  printf("\nTouch Keyboard\n");

  while( getchar_timeout_us(1000) == PICO_ERROR_TIMEOUT)
    {
      uint32_t x = touch_key_raw & 0x0FFFFFFF;
      
      binstr[0] = '\0';

      for(int i=0; i<32; i++)
	{
	  if( x & (1<<i) )
	    {
	      strcat(binstr, "1");
	      if( padcnt[i] < 20 )
		{
		  padcnt[i]+=5;

		  if( padcnt[i] >= 8 )
		    {
		      if( !pressed[i] )
			{
			  pressed[i] = 1;

			  printf("\n-%d-", i);
			}
		      else
			{

			}
		    }
		}
	    }
	  else
	    {
	      if( padcnt[i] > 0 )
		{
		  padcnt[i]--;

		  if( padcnt[i] == 0 )
		    {
		      if( pressed[i] )
			{
			  pressed[i] = 0;
			}
		    }
		  else
		    {
		    }
		}
	      
	      strcat(binstr, " ");
	    }
	}

#if 0      
      printf("\nRaw:%08X   %s\n", touch_key_raw, binstr);
      for(int i=0; i<32; i++)
	{
	  printf(" %02d ", padcnt[i]);
	}

      for(int i=0; i<32; i++)
	{
	  if( padcnt[i] > 0 )
	    {
	      printf(" %s ", paddesc[i]);
	    }
	}
#endif
    }
  
}

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

SERIAL_COMMAND serial_cmds[] =
  {
    {
      '?',
      "Serial command help",
      serial_help,
    },
    {
      '!',
      "Boot to mass storage",
      cli_boot_mass,
    },
    {
      'z',
      "Enter parameter value",
      cli_enter_parameter,
    },
    {
      '^',
      "Update Display",
      cli_update_display,
    },
    {
      '$',
      "Test BCD",
      cli_test_bcd,
    },
    {
      'v',
      "Version",
      cli_version,
    },
    {
      '*',
      "Dump State",
      cli_dump_state,
    },
    {
      '&',
      "Dump Store",
      cli_dump_store,
    },
    {
      'T',
      "Load Test Code",
      cli_load_test_code,
    },
    {
      'U',
      "Load Test Code 2",
      cli_load_test_code_2,
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
      '-',
      "*Digit",
      cli_minus,
    },
    {
      '.',
      "*Digit",
      cli_dot,
    },
    {
      'r',
      "Run",
      cli_run,
    },
    {
      's',
      "Stop",
      cli_stop,
    },
    {
      'N',
      "Normal Reset",
      cli_normal_reset,
    },
    {
      'K',
      "KI Reset",
      cli_ki_reset,
    },
    {
      'x',
      "Dump Extracode Information",
      print_extracode_info,
    },
    {
      'I',
      "Load IAR",
      cli_load_iar,
    },
    {
      'B',
      "Breakpoint",
      cli_breakpoint,
    },
    {
      '~',
      "Breakpoint toggle",
      cli_breakpoint_toggle,
    },
    {
      'A',
      "Load ADDR",
      cli_load_addr,
    },
    {
      'i',
      "Incr ADDR",
      cli_incr_addr,
    },
    {
      'd',
      "Decr ADDR",
      cli_decr_addr,
    },
    {
      'S',
      "Load STORE",
      cli_load_store,
    },
    {
      'a',
      "A",
      cli_key_a,
    },
    {
      'b',
      "B",
      cli_key_b,
    },
    {
      'c',
      "C",
      cli_key_c,
    },
#if ESC_TYPE_SMALL   
    {
      '%',
      "Key test",
      cli_key_key_test,
    },
#endif   
#if ESC_TYPE_SMALL   
    {
      '|',
      "KBD Dump",
      cli_kbd_dump,
    },
#endif
   
    {
      'F',
      "File list",
      cli_file_list,
    },
    {
      'R',
      "Read file into state",
      cli_file_read_state,
    },
    {
      '+',
      "Run test",
      cli_run_single_test,
    },
    {
      '=',
      "Run all tests",
      cli_run_tests,
    },
    {
      '[',
      "Load test code into store",
      cli_load_test_code_into_store,
    },
    {
      '@',
      "Test results",
      cli_test_results,
    },
    {
      '<',
      "Dump touch key data",
      cli_dump_touch_key_data,
    },
    {
      'D',
      "DUMP",
      cli_dump,
    },
    {
      'O',
      "RELOAD",
      cli_reload,
    },
    {
      'H',
      "CHECK",
      cli_check,
    },
   
#if ESC_TYPE_DESKTOP
    {
      'Q',
      "Qt Touch Debug",
      cli_qt_debug,
    },
    {
      'L',
      "Desktop display test",
      cli_display_test,
    },
    {
      '/',
      "Desktop display CLI",
      cli_escdd,
    },
#endif   
  };


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
#if DEBUG_SERIAL
	      printf("\nKey:%d (0x%02X)", key, key);
#endif
	      keypress = key;
	      (*serial_cmds[i].fn)();
	      prompt();
	      break;
	    }
	}
    }
  else
    {
      // No key, so inject an 'idle' token into the fsms
      queue_token(TOK_NO_TOK);
      
      // I have found that I need to send something if the serial USB times out
      // otherwise I get lockups on the serial communications.
      // So, if we get a timeout we send a space and backspace it. And
      // flush the stdio, but that didn't fix the problem but seems like a good idea.
      stdio_flush();
      //printf(" \b");
    }
}

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

void prompt_breakpoint(void)
{
  printf("\n\n(Text Parameter:'%s'", text_parameter);
  printf("\n(Breakpoint:%03X%c %c, Address (Slot Num):%d (%04X) %c) >",
	 parameter, parameter_a_flag?'A':' ', auto_increment_parameter?'A':' ',
	 address,   address,   auto_increment_address?  'A':' ');
}


////////////////////////////////////////////////////////////////////////////////

// Spaces if empty or the value (in hex)

char *display_word(SINGLE_WORD w)
{
  static char result[MAX_LINE];
  
  sprintf(result, "%08X", w);
  return(result);
  
}


char *display_register_single_word(REGISTER_SINGLE_WORD x)
{
  static char result[MAX_LINE];
  int s;
  char sc;
  
  if( x == 0xFFFFFFFF )
    {
      return("        ");
    }

  // Get sign
  s = SW_SIGN(x);

  switch(s)
    {
    case WORD_SIGN_MINUS:
      sc = '-';
      break;
      
    case WORD_SIGN_PLUS:
      sc = '+';
      break;
      
    default:
      sc = '?';
      break;
    }
  
  x = REMOVED_SW_SIGN(x);
  if( sc == '?' )
    {
      sprintf(result, "%01X%07X", s, x);
    }
  else
    {
      sprintf(result, "%c%07X", sc, x);
    }
  return(result);
}

char *display_register_double_word(REGISTER_DOUBLE_WORD x)
{
  static char result[MAX_LINE];
  
  if( x == 0xFFFFFFFFFFFFFFFFL )
    {
      return("        ");
    }

  sprintf(result, "%16llX", x);
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// Display the register name and contents
// There are two functions, as R8 and R9 have to be displayed
// across two lines.

char *display_register_and_contents(ESC_STATE *s, int regno)
{
  static char result[MAX_LINE*2];

  result[0] = '\0';
  
  if( IS_SW_REGISTER(regno) )
    {
      sprintf(result, "R%d %s", regno, display_register_single_word(read_register(s, regno)));
    }

  if( IS_DW_REGISTER(regno) )
    {
      snprintf(result, MAX_LINE, "R%d %s", regno, display_register_double_word(read_register(s, regno)));
    }

  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// Display the address and contents of a store location
//
////////////////////////////////////////////////////////////////////////////////

char *display_store_and_contents(ESC_STATE *s, SINGLE_WORD address)
{
  static char result[MAX_LINE*2];

  result[0] = '\0';

  sprintf(result, "%2X %s", address, display_store_word(load_from_store(s, address)));

  return(result);
}

char *display_store_and_contents_from_tar(ESC_STATE *s, int tar_address)
{
  static char result[MAX_LINE*2];

  result[0] = '\0';

  sprintf(result, "%2X %s", load_from_store(s, tar_address), display_store_word(load_from_store(s, load_from_store(s, tar_address))));

  return(result);
}


// Addresses are in the same format as registers, BCD with sign (always positive)
char *display_address(REGISTER_SINGLE_WORD x)
{
  static char result[MAX_LINE];
  
  if( x == 0xFFFFFFFF )
    {
      return("  ");
    }

  // Drop sign
  sprintf(result, "%02X", x & ADDRESS_MASK);
  return(result);
}

char *display_instruction(SINGLE_WORD inst)
{
  static char result[MAX_LINE];
  
  sprintf(result, "%08X", inst);

  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// Modify the displayed number to be in suppressed form.
//
////////////////////////////////////////////////////////////////////////////////

char suppressed[MAX_LINE];

void suppress_output(char *t)
{
  strcpy(suppressed, t);

#if DEBUG_SUPPRESSED
  printf("\n%s:'%s'", __FUNCTION__, suppressed);
#endif
  
  // Remove any plus
  for(int i=0; suppressed[i] != '\0'; i++)
    {
      if( suppressed[i] == '+' )
        {
          suppressed[i] = ' ';
        }

    }

  // Now start at the front and blank any zero that is followed by a zero

  for(int i=0; i<strlen(suppressed); i++)
    {
#if DEBUG_SUPPRESSED
      printf("\n%s:A  i:%d:'%s'", __FUNCTION__, i, suppressed);
#endif

      // If we see a '-' then skip over it
      if( suppressed[i] == '-' )
        {
          continue;
        }

#if DEBUG_SUPPRESSED
      printf("\n%s:B  i:%d:'%s'", __FUNCTION__, i, suppressed);
#endif


      // Skip spaces
      if( suppressed[i] == ' ' )
        {
          continue;
        }

      // If we see anything other than a zero then we stop, as only leading zeroes are being removed.
      if( (suppressed[i] != '0' ) )
        {
          break;
        }

      //------------------  Do not modify above this line
      
#if DEBUG_SUPPRESSED
      printf("\n%s:E  i:%d:'%s'", __FUNCTION__, i, suppressed);
#endif

#if DEBUG_SUPPRESSED
      printf("\n%s:C  i:%d:'%s'", __FUNCTION__, i, suppressed);
#endif

      // Lose leading zero in front of a digit, but only if not last character
      if( (suppressed[i] == '0') )
        {
          switch(suppressed[i+1])
            {
            case'.':
            case '\0':
              break;

            case '0':
            default:
              suppressed[i] = ' ';
              break;
            }
        }
      
#if DEBUG_SUPPRESSED
      printf("\n%s:D  i:%d:'%s'", __FUNCTION__, i, suppressed);
#endif


      if( (suppressed[i] == '0') && (suppressed[i+1] == '0') )
        {
          suppressed[i] = ' ';
        }

#if DEBUG_SUPPRESSED
      printf("\n%s:F  i:%d:'%s'", __FUNCTION__, i, suppressed);
#endif

    }

#if DEBUG_SUPPRESSED
  printf("\n%s:After leading zero removal'%s'", __FUNCTION__, suppressed);
#endif
  
  // Lose any trailing point
  int last = strlen(suppressed)-1;
  
  if( (suppressed[last] == '.') && (suppressed[last+1] == '\0') && (strlen(suppressed) != 2) )
    {
      // Move everything over to cover the point
#if 0 
      suppressed[last] = ' ';
#else
      for(int i = strlen(suppressed)-1; i>0; i--)
        {
          suppressed[i] = suppressed[i-1];
        }
      suppressed[0] = ' ';
#endif      
    }

#if DEBUG_SUPPRESSED
  printf("\n%s:After trailing point removal'%s'", __FUNCTION__, suppressed);
#endif

  // Don't allow a blank display
  if( strlen(suppressed) == 0 )
    {
      strcpy(suppressed, "0");
    }
  
#if DEBUG_SUPPRESSED
  printf("\n%s:'%s'", __FUNCTION__, suppressed);
#endif
  strcpy(t, suppressed);
}

////////////////////////////////////////////////////////////////////////////////
//
// Process and C and D digits
//


void process_signs_output(char *t)
{
  strcpy(suppressed, t);

#if DEBUG_SUPPRESSED
  printf("\n%s:'%s'", __FUNCTION__, suppressed);
#endif
  
  // Remove any plus
  for(int i=0; suppressed[i] != '\0'; i++)
    {
      if( suppressed[i] == 'C' )
        {
          suppressed[i] = ' ';
        }

      if( suppressed[i] == 'D' )
        {
          suppressed[i] = '-';
        }
    }

  strcpy(t, suppressed);
}

#if 0
char *suppress_output_old(char *t)
{
  char *t1 = t;
  int non_zero_seen = 0;
  
  // First, remove plus sign
  for(;(*t)!='\0'; t++)
    {
      switch(*t)
	{
	case '+':
	  *t = ' ';
	  break;

	case '0':
	  if( !non_zero_seen )
	    {
	      for(char *p = t; p>=(t1+1); p--)
		{
		  *p = *(p-1);
		}
	      
	      *t1 = ' ';
	    }
	  break;

	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  non_zero_seen = 1;
	  break;
	}
    }

#if 0
  if( strlen(t1) == 0 )
    {
      strcpy(t1, "0");
    }
  
#else
  int blank = 1;
  
  for(int i=0; i<strlen(output); i++)
    {
      switch(output[i])
        {
        case'0':
        case'1':
        case'2':
        case'3':
        case'4':
        case'5':
        case'6':
        case'7':
        case'8':
        case'9':
          blank = 0;
          break;
          
        case'.':
          //blank = 0;
          break;
        }
    }

  if( blank )
    {
      strcpy(output, "0");
    }
  
#endif
  
  return(t1);
}
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Store can hold floating point or instruction
//
////////////////////////////////////////////////////////////////////////////////

char *display_store_word(SINGLE_WORD w)
{
  static char result[MAX_LINE];
  static char result2[MAX_LINE];

  result2[0] ='\0';
  result2[1] ='\0';
  
  int digit_a = INST_A_FIELD(w);
  int digit_b = INST_B_FIELD(w);
  char sign_char = ' ';
  
  switch(digit_a)
    {
    case WORD_SIGN_PLUS:
      sign_char = '+';
      break;
      
    case WORD_SIGN_MINUS:
      sign_char = '-';
      break;
      
    default:
      break;
    }
  
  switch(digit_a)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      // Instruction
      sprintf(result, "&%08X", w);
      break;

    default:
      // Floating point
      sprintf(result, "%c%08X", sign_char, w);

      sprintf(result2, "&%c", sign_char);
      
      // Insert decimal point
      strncat(result2, result+3, 6-digit_b);
      strcat( result2, ".");
      strncat(result2, result+3+6-digit_b, digit_b);
      strcat( result2, "");
      strcpy( result, result2);

      break;
    }

  if( suppressed_display )
    {
      // Suppressed output has no + symbol or leading zeros. This is closer to the original machine.

      suppress_output(result);
      //      printf("\nSuppressed:'%s'", result);
    }      

  process_signs_output(result);
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// KB register is a double word register but can hold either:
//
// Single word floating point
// Single word integer
// Double word integer
//
// If there's a sign, then floating point, otherwise double word integer
// 
////////////////////////////////////////////////////////////////////////////////

char *display_kb_reg(DOUBLE_WORD w)
{
  static char result[MAX_LINE];
  static char result2[MAX_LINE];

  result2[0] ='\0';
  result2[1] ='\0';

  // Check single word sign digit
  int digit_a = INST_A_FIELD(w);
  int digit_b = INST_B_FIELD(w);
  char sign_char = ' ';
  
  switch(digit_a)
    {
    case WORD_SIGN_PLUS:
      sign_char = '+';
      break;
      
    case WORD_SIGN_MINUS:
      sign_char = '-';
      break;
      
    default:
      break;
    }
  
  switch(digit_a)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      // Instruction
      sprintf(result, "%08X", w);
      break;

    default:
      // Floating point
      sprintf(result, "%c%08X", sign_char, w);
      sprintf(result2, "%c", sign_char);
      
      // Insert decimal point
      strncat(result2, result+3, 6-digit_b);
      strcat( result2, ".");
      strncat(result2, result+3+6-digit_b, digit_b);
      strcat( result2, "");
      strcpy( result, result2);
      break;
    }

  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// Display IAR
//
// Do not display IAR if we are inside an extracode subroutine and aren't
// stepping through the extracode.

char *display_iar(ESC_STATE *s, IAR_SPEC spec_iar)
{
  static char result2[MAX_LINE];
  IAR val;
  int force = 0;
  
  switch(spec_iar)
    {
    case SPEC_IAR:
      val = s->iar;
      force = 0;
      break;

    case SPEC_AUX_IAR:
      val = s->aux_iar;
      force = 0;
      break;

    case SPEC_BREAKPOINT:
      val = s->breakpoint;
      force = 1;
      break;

    case SPEC_FORCE_IAR:
      val = s->iar;
      force = 1;
      break;

    case SPEC_FORCE_AUX_IAR:
      val = s->aux_iar;
      force = 1;
      break;
    }

#if DEBUG_DISPLAY_IAR
  printf("\ns:exiting extracode:%d IS_EXTRACODE:%d setup_step_extracode:%d", s->exiting_extracode, IS_EXTRACODE, setup_step_extracode);
#endif
  
  if( ((s->exiting_extracode || IS_EXTRACODE) && !setup_step_extracode) && !force)
    {
      sprintf(result2, "    ");
    }
  else
    {
      sprintf(result2, "%02X%c", val.address, val.a_flag?'A':' ');
    }

  return(result2);
}

char *display_presumptive_address_1(ESC_STATE *s)
{
  static char result[MAX_LINE];

  if( s->reginst_rc !=NO_VALUE )
    {
      sprintf(result,"%s", display_register_and_contents(s, s->reginst_rc) );
    }

  return(result);
}

char *display_presumptive_address_2(ESC_STATE *s)
{
  static char result[MAX_LINE];

  if( s->reginst_rd !=NO_VALUE )
    {
      sprintf(result,"%s", display_register_and_contents(s, s->reginst_rd) );
    }

  return(result);
}

////////////////////////////////////////////////////////////////////////////////
//
// Display update
//
// The display format is fairly fluid.
// We append to a string to get the display, this allows formatting in the
// functions we call to be shown.
//
// Other display modes can over-ride the computer display, e.g. the reload screen
//
// The top two lines of the screen are fixed, and display the IAR and KB register data.
// The bottom four lines are free-form and information appears there driven by the instruction
// processing. Functions are provided for instructions to put informaton on these lines, previous
// data is deleted, then the information is displayed and left until the next push of data.
//



//------------------------------------------------------------------------------
//
// Display information on one of the four lower lines
//
// Lines are numbered starting at 1.
//
// The text for the lower four lines (and the upper but they aren't used)
// A flag is provided to indicate whether the display should be updated (on OLED etc)
//
// If the string starts with a '&' then the data is a value. That needs special spacing to
// accomodate the decimal poijnt and also match the original.

char display_line[NUM_LINES][MAX_LINE+3];

void display_on_line(ESC_STATE *s, int display, int line_no, char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  if( !display )
    {
      return;
    }
  
#if DEBUG_DISPLAY_ON_LINE
  if( line_no > NUM_LINES )
    {
      printf("\nLine no invalid (%d", line_no);
      printf("\nStopping\n");
      while(1)
	{
	}
    }
#endif
  
  char line[400];
  
  vsnprintf(line, 400, fmt, args);
  if( strlen(line) > MAX_LINE )
    {
      printf("\nfmt='%s'", fmt);
      printf("\nLine too long (%s)", line);
      printf("\nStopping\n");
      while(0)
	{
	}
    }
  
#if DEBUG_DISPLAY
  printf("\n*** %s ***  display=%d", __FUNCTION__, display);
#endif

  vsnprintf(&(display_line[line_no-1][0]), MAX_LINE, fmt, args);

  // Pad the line with spaces to delete any previous contents
  int padlen = 15-strlen(&(display_line[line_no-1][0]));
  
  for(int p=0; p<padlen ; p++)
    {
      strcat(&(display_line[line_no-1][0]), " ");
    }
  
  va_end(args);

  s->update_display = display;
}


int display_any_size_register_on_line(ESC_STATE *s, int display, int lineno, int regno, int contents)
{
  if( IS_SW_REGISTER(regno) )
    {
      if( contents )
	{
      	  display_on_line(s, display, lineno, "%s", display_register_and_contents(s, regno));
	}
      else
	{
	  display_on_line(s, display, lineno, "R%d", regno);
	}

      return(lineno+1);
    }
  
  if( IS_DW_REGISTER(regno) )
    {
      if( contents )
	{
	  display_on_line(s, display, lineno,   "R%d", regno);
	  display_on_line(s, display, lineno+1, "%s", display_register_double_word(regno));
	}
      else
	{
	  display_on_line(s, display, lineno,   "R%d", regno);
	  display_on_line(s, display, lineno+1, "               ");
	}
      return(lineno+2);
    }
  
  return(lineno);
}

int display_two_any_size_register_on_line(ESC_STATE *s, int display, int lineno, int regno1, int regno2, int contents)
{
  int lineno2 = display_any_size_register_on_line(s, display, lineno,  regno1, contents);
  lineno2 = display_any_size_register_on_line(s, display, lineno2, regno2, contents);

  //  display_on_line(s, display, 5, "OUT            ");
  return(lineno2);
}

void clear_display(ESC_STATE *s, int display)
{
  if( display )
    {
#if OLED_ON
      oled_clear_display(&oled0);
#endif
    }
}

void clear_line(ESC_STATE *s, int display, int n)
{
  display_on_line(s, display, n, "               ");
}

void clear_lines_3_to_6(ESC_STATE *s, int display)
{
  display_on_line(s, display, 3, "               ");
  display_on_line(s, display, 4, "               ");
  display_on_line(s, display, 5, "               ");
  display_on_line(s, display, 6, "               ");
}


					    
////////////////////////////////////////////////////////////////////////////////
//
// Updates the display for the schools computer screen
//

void update_computer_display(ESC_STATE *es)
{
  char tmp[1000];
  int oledy = 0;

#if DEBUG_DISPLAY
  printf("\n*** %s ***", __FUNCTION__);

  printf("\n");
  
  printf("\nKeyboard register: %08X   IAR:%8s", es->keyboard_register, display_iar(es, SPEC_IAR));
  printf("\n");
#endif
  
  //
  // Print a representation of the TV display
  //
  
  strcpy(dsp, "   012345678901234\n");

#if 0  
  //------------------------------------------------------------------------------  
  // Line 1
  //------------------------------------------------------------------------------
  
  sprintf(tmp, "1: %02s %8s",
	  display_iar(es, SPEC_IAR),
	  display_store_word(es->keyboard_register));
  strcat(dsp, tmp);
  
#if OLED_ON
  oled_clear_display(&oled0);
  sprintf(tmp, "%02s %8s",
	  display_iar(es, SPEC_IAR),
	  display_store_word(es->keyboard_register));
  
  oled_set_xy(&oled0, 0, oledy);
  oledy+=8;
  oled_display_string(&oled0, tmp);
#endif

  //------------------------------------------------------------------------------
  // Line 2
  //------------------------------------------------------------------------------
  
  if( es->ki_reset_flag )
    {
      sprintf(tmp, "\n2: %c",es->ki_reset_flag?'K':' ');
    }
  else
    {
      sprintf(tmp, "\n2: %2s %8s %c",
	      display_iar(es, SPEC_AUX_IAR),
	      display_instruction(es->instruction_register),
	      es->stage
	      );
    }
  
#if OLED_ON
  if( es->ki_reset_flag )
    {
      sprintf(tmp, "%c",es->ki_reset_flag?'K':' ');
    }
  else
    {
      sprintf(tmp, "%2s %8s %c",
	      display_iar(es, SPEC_AUX_IAR),
	      display_instruction(es->instruction_register),
	      es->stage
	      );
    }
  
  oled_set_xy(&oled0, 0, oledy);
  oledy+=8;
  oled_display_string(&oled0, tmp);
#endif
  
  strcat(dsp, tmp);

#endif

#if ESC_TYPE_DESKTOP
  lcd_cls(0);
  lcd_cls(1);
#endif
  
  // Display the lower four lines that have been set up
  for(int i=1; i<=6; i++)
    {
      
#if OLED_ON
      oledy = 8*(i-1);
      oled_set_xy(&oled0, 0, oledy);

      oled_display_string(&oled0, &(display_line[i-1][0]));

      sprintf(tmp, "\n%d: %s", i, &(display_line[i-1][0]));
      strcat(dsp, tmp);
#endif
      
#if ESC_TYPE_DESKTOP
      escdd_text(0, (i-1)*PIXEL_SCALE*9, &(display_line[i-1][0]), strlen(&(display_line[i-1][0])));

      //      show_char(0, (i-1), &(display_line[i-1][0]));
#endif
    }

#if DEBUG_DISPLAY
  // Now update the display output device(s)
  printf("\n%s\n", dsp);
#endif
}

////////////////////////////////////////////////////////////////////////////////

void update_reload_display(ESC_STATE *es)
{
  int oledy = 0;
  char line[MAX_LINE+2];

#if OLED_ON
  oled_clear_display(&oled0);
#endif

#if ESC_TYPE_DESKTOP
  lcd_cls(0);
  lcd_cls(1);
#endif
  
  file_partial_list(ESC_DIR, es->reload_file_first, 6);
  
  for(int i=0; i<FILE_LIST_DATA_LINES_MAX; i++)
    {
#if OLED_ON
      oled_set_xy(&oled0, 0, oledy);
      sprintf(line, "%c%s", (i==0)?'>':' ', file_list_data[i]);
      oled_display_string(&oled0, line);
#endif
      
#if ESC_TYPE_DESKTOP
      show_char(0, oledy, &(display_line[i-1][0]));
#endif
      
      oledy+=8;
    }
}

void update_delete_display(ESC_STATE *es)
{
  int oledy = 0;
  char line[MAX_LINE+2];
  
  oled_clear_display(&oled0);

  oled_set_xy(&oled0, 0, 8);
  oled_display_string(&oled0, "Delete file?");

  oled_set_xy(&oled0, 0, 16);
  oled_display_string(&oled0, &(file_list_data[0][0]));
  
}

////////////////////////////////////////////////////////////////////////////////

void update_setup_display(ESC_STATE *es)
{
  int oledy = 0;
  char line[MAX_LINE+2];
  int varval;
  
#if OLED_ON
  oled_clear_display(&oled0);
#endif

#if ESC_TYPE_DESKTOP
  lcd_cls(0);
  lcd_cls(1);
#endif
  
  //  file_partial_list(ESC_DIR, es->reload_file_first, 6);
  
  for(int i=0; i<FILE_LIST_DATA_LINES_MAX; i++)
    {
      if( i + es->reload_file_first >= NUM_SETUP_ENTRIES )
        {
          break;
        }

      varval = *(setup_entries[es->reload_file_first+i].value_var);
      
#if OLED_ON
      oled_set_xy(&oled0, 0, oledy);
      sprintf(line, "%s%s", setup_entries[es->reload_file_first+i].title, varval?setup_entries[es->reload_file_first+i].val0:setup_entries[es->reload_file_first+i].val1);
      oled_display_string(&oled0, line);
#endif
      
#if ESC_TYPE_DESKTOP
      show_char(0, oledy, &(display_line[i-1][0]));
#endif
      
      oledy+=8;
    }
}

////////////////////////////////////////////////////////////////////////////////

void update_display(void)
{
  ESC_STATE *s= &esc_state;
  
  if( s->update_display || s->inst_update_display )
    {

#if DEBUG_DISPLAY
      printf("\n*** %s ***", __FUNCTION__);
      printf("\n** update_display is true **");
#endif

      s->update_display = 0;
      s->inst_update_display = 0;
      
      if( s->reload_display )
	{
	  update_reload_display(s);
	}
      else if( s->delete_display )
	{
	  update_delete_display(s);
	}
      else if( s->setup_display )
        {
          update_setup_display(s);
        }
      else
	{
	  update_computer_display(s);
	}
    }
}

////////////////////////////////////////////////////////////////////////////////
#if 0
I2C_PORT_DESC i2c_bus_0 =
  {
    0, 18,
    0, 7,
  };

#endif

#if 0
// Dummy structure
I2C_SLAVE_DESC oled0 =
  {
    &i2c_bus_0,
    0x78 >>1,
  };

#endif

////////////////////////////////////////////////////////////////////////////////
//
//    Core 1 
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Read a 32 bit value from four 165 latches
//
////////////////////////////////////////////////////////////////////////////////

#if ESC_KBD_VOLT_TOUCH
const int PIN_TOUCH_LOAD   = 27;
const int PIN_TOUCH_CLK    = 26;
const int PIN_TOUCH_SERDAT = 28;


uint32_t read_165(void)
{
  uint32_t value = 0;
  
  // Latch the data
  gpio_put(PIN_TOUCH_LOAD, 0);
  gpio_put(PIN_TOUCH_LOAD, 0);
  sleep_us(100);
  
  gpio_put(PIN_TOUCH_LOAD, 1);

  // Clock the data out of the latch
  for(int i=0; i<32; i++)
    {
      sleep_us(1);
      
      // Read data
      value <<= 1;
      if( gpio_get(PIN_TOUCH_SERDAT) )
	{
	  value |= 1;
	}

      sleep_us(1);
      gpio_put(PIN_TOUCH_CLK, 0);
      sleep_us(1);
      gpio_put(PIN_TOUCH_CLK, 1);
    }

#if XP_DEBUG  
  printxy_hex(0,3, value);
#endif
  return(value);
}
#endif

void sense_gpio_out(const int gpio)
{
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_OUT);
}

void sense_gpio_in(const int gpio)
{
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_IN);
  gpio_set_pulls (gpio, 0, 0);
}

#if ESC_KBD_VOLT_TOUCH
void touch_key_setup(void)
{
  sense_gpio_out(PIN_TOUCH_LOAD);
  sense_gpio_out(PIN_TOUCH_CLK);
  sense_gpio_in(PIN_TOUCH_SERDAT);
}


void touch_key_scan(void)
{
  uint32_t data;
  
  // Read the four 165s and get the current levels on all the pins.
  touch_key_raw = read_165();
}

void core1_main(void)
{
  touch_key_setup();
  
  while(1)
    {
      touch_key_scan();
    }
}

#endif

#if ESC_TYPE_DESKTOP
void core1_main(void)
{
  while(1)
    {

    }
}
#endif

#if ESC_TYPE_SMALL
void core1_main(void)
{
  while(1)
    {

    }
}
#endif

////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////


int main(void)
{

  ////////////////////////////////////////////////////////////////////////////////
  //
  // Overclock as needed
  //
  ////////////////////////////////////////////////////////////////////////////////
  
#define OVERCLOCK 135000
  //#define OVERCLOCK 200000
  //#define OVERCLOCK 270000
  //#define OVERCLOCK 360000
  
#if OVERCLOCK > 270000
  /* Above this speed needs increased voltage */
  vreg_set_voltage(VREG_VOLTAGE_1_20);
  sleep_ms(1000);
#endif
  
  /* Overclock */
  set_sys_clock_khz( OVERCLOCK, 1 );

#if ESC_TYPE_DESKTOP
  printf("\nDesktop ESC");
  escdd_display_start();
  
#endif
  
  //stdio_init_all();
  stdio_usb_init();
  
  sleep_ms(3000);

  printf("\n");
  printf("\n                                  ********************************************");
  printf("\n                                  * Experimental Schools Computer Simulation *");
  printf("\n                                  ********************************************");
  printf("\n");
  printf("\nVersion:1.1.%d Compile time:%s %s", VERSION_INC, __DATE__, __TIME__);
  printf("\n");
  
  // Load extracode
  load_extracode_by_id(&esc_state, EXTRACODE_STANDARD);
  
  // Configuration

#if ESC_TYPE_SMALL
  printf("\nSmall ESC");
#endif


#if ESC_KBD_VOLT_TOUCH
  printf("\nVoltage Touch Keyboard");
#endif

#if ESC_TYPE_DESKTOP

  printf("\nQT Touch Keyboard");

  // Set up GPIOs
  set_kbd_gpios();

  // Set up the touch keyboard. This should be stored in the IC
  // but it doesn't, so we set it up and then calibrate every time
  // we start up.

  printf("\nWriting setups block...");
  sleep_ms(3000);
  
  write_setups_block(&(setups_block_esc[0]));

  printf("\nCalibrating all...");
  
  do_calibrate_all();
  
  printf("\nresetting touch controller...");
  do_qt_reset();
  printf("\ndone.");
#endif

  // Set up the checksums of the extracodes we have in the code.
  calculate_extracode_checksums();
  
#if OLED_ON
  // Set up OLED display
  i2c_init(&i2c_bus_0);
  
  oled_setup(&oled0);
#endif
  
  // Sets sd_ok flag for later use
#if SD_ON   

  printf("\nInitialising SD card driver...");

#define SD_CARD 1

  // Initialise SD card driver
  sd_init_driver();

  // Mount and unmount the SD card to set the sd_ok_flag up
  mount_sd();
  unmount_sd();
  
  oled_set_xy(&oled0, 0,21);
  if( sd_ok_flag )
    {
      oled_display_string(&oled0, "SD card OK");
      printf("\nSD card OK");
    }
  else
    {
      oled_display_string(&oled0, "SD card NOT OK");
      printf("\nSD card NOT OK");
    }

  mount_sd();
  if( !cd_to_dir("/ESC") )
    {
      printf("\nFailed to cd to /ESC directory");
      printf("\n%s", sd_error);
    }
  else
    {
      printf("\n/ESC directory found");
    }
  unmount_sd();
  
  sleep_ms(1000);
#endif

#if OLED_ON
  // Overall loop, which contains the polling loop and the menu loop
  oled_clear_display(&oled0);
  
  oled_set_xy(&oled0, 20, 0);
  oled_display_string(&oled0, "Experimental");

  oled_set_xy(&oled0, 30, 8);
  oled_display_string(&oled0, "Schools");

  oled_set_xy(&oled0, 30, 16);
  oled_display_string(&oled0, "Computer");
#endif

#if ESC_TYPE_DESKTOP
  // Overall loop, which contains the polling loop and the menu loop
  lcd_cls(0);
  lcd_cls(1);

  show_char(12, 10, "Experimental");
  show_char(12, 12, "  School's");
  show_char(12, 14, "  Computer");
  

#endif
  
#ifdef ESC_USE_WIFI
  printf("\n** Wifi Enabled **");
	 
  wifi_main();
#else
  printf("\n** Wifi NOT Enabled **");

  // Run the shift register touch key scanning on the second core
  multicore_launch_core1(core1_main);


  // Main loop
#if ESC_TYPE_SMALL
  set_kbd_gpios();
#endif

#if ESC_TYPE_DESKTOP
#endif
  
  while(1)
    {
#if DEBUG_LOOP
      printf("\nLoop");
#endif
      
#if ESC_TYPE_SMALL      
      kbd_scan(&esc_state);
#endif

#if ESC_TYPE_DESKTOP
      kbd_read(&esc_state);
#endif
      
      drive_fsms();
      serial_loop();
      update_display();
    }
  
#endif
}
