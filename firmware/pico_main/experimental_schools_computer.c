////////////////////////////////////////////////////////////////////////////////
//
// Experimental Schools Computer Simulator
//
////////////////////////////////////////////////////////////////////////////////
//
// Simulates the IBM experimental schools computer
//
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


#include "fsms.h"
#include "esc_fsms.h"

#include "esc.h"

////////////////////////////////////////////////////////////////////////////////

#define TEXT_PARAMETER_LEN 40

void serial_help(void);
void prompt(void);

int keypress = 0;
int parameter = 0;
int auto_increment_parameter = 0;
int auto_increment_address   = 0;
char text_parameter[TEXT_PARAMETER_LEN+1] = "";
int address     = 0;

////////////////////////////////////////////////////////////////////////////////
//
//
//

// The computer instance

ESC_STATE esc_state;


////////////////////////////////////////////////////////////////////////////////

// Execution

void run_stage_a(ESC_STATE *s, int display)
{
  // load aux iar
  s->aux_iar = s->iar;
  
  // Load instruction into instruction register
  s->instruction_register = s->store[s->aux_iar];

  s->update_display = display;
}
void run_stage_b(ESC_STATE *s, int display)
{

  s->update_display = display;
}

void run_stage_c(ESC_STATE *s, int display)
{

  s->update_display = display;
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



// Load ADDR from KBD
void state_esc_load_addr(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  es->address_register2 = es->keyboard_register;

  es->update_display = 1;
}

// Load store from KBD
void state_esc_load_store(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  es->store[es->address_register2] = es->keyboard_register;

  es->update_display = 1;
}

void state_esc_incr_addr(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  es->address_register2 += 1;

  es->update_display = 1;
}

void state_esc_decr_addr(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  es->address_register2 -= 1;

  es->update_display = 1;
}

// Load IAR from KBD
void state_esc_load_iar(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  es->iar = es->keyboard_register;
  es->update_display = 1;
}

// Add digit to the keybord register
void state_esc_numeric(FSM_DATA *s, TOKEN tok)
{
  int num = tok - TOK_KEY_0;
  ESC_STATE *es;

  es = (ESC_STATE *)s;
  
  es->keyboard_register *= 16;
  es->keyboard_register += num;

  es->update_display = 1;
}

void state_esc_normal_reset(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  // Everything cleared except IAR
  
  es->keyboard_register = 0xFFFFFFFF;
  
  es->ki_reset_flag = 0;
  es->address_register0 = 0xFFFFFFFF;
  es->address_register1 = 0xFFFFFFFF;
  es->address_register2 = 0xFFFFFFFF;
  
  // Re-display
  es->update_display = 1;
}

void state_esc_a_disp(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  run_stage_a(es, DISPLAY_UPDATE);
  es->update_display = 1;
}

void state_esc_b_disp(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  run_stage_b(es, DISPLAY_UPDATE);
  es->update_display = 1;
}

void state_esc_c_disp(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  run_stage_c(es, DISPLAY_UPDATE);
  es->update_display = 1;
}

void state_esc_a_no_disp(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  run_stage_a(es, DISPLAY_NO_UPDATE);
  es->update_display = 1;
}

void state_esc_b_no_disp(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  run_stage_b(es, DISPLAY_NO_UPDATE);
  es->update_display = 1;
}

void state_esc_c_no_disp(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  run_stage_c(es, DISPLAY_NO_UPDATE);
  es->update_display = 1;
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
     {CTOK_NUMERIC,         STATE_ESC_INIT,  state_esc_numeric},
     {TOK_KEY_NORMAL_RESET, STATE_ESC_INIT,  state_esc_normal_reset},
     {TOK_KEY_LOAD_IAR,     STATE_ESC_INIT,  state_esc_load_iar},
     {TOK_KEY_LOAD_ADDR,    STATE_ESC_INIT,  state_esc_load_addr},
     {TOK_KEY_INCR_ADDR,    STATE_ESC_INIT,  state_esc_incr_addr},
     {TOK_KEY_DECR_ADDR,    STATE_ESC_INIT,  state_esc_decr_addr},
     {TOK_KEY_LOAD_STORE,   STATE_ESC_INIT,  state_esc_load_store},
     {TOK_KEY_A,            STATE_ESC_INIT,  state_esc_a_disp},
     {TOK_KEY_B,            STATE_ESC_INIT,  state_esc_b_disp},
     {TOK_KEY_C,            STATE_ESC_INIT,  state_esc_c_disp},
     {CTOK_ERROR,           STATE_ESC_INIT,  NULL},
     {CTOK_END,             STATE_NULL,      NULL},
    }
   },
   {
    _STATE(STATE_ESC_ENT),
    null_entry_fn,
    null_every_fn,
    {
     {CTOK_NUMERIC,  STATE_ESC_ENT,      NULL},
     {CTOK_END,      STATE_NULL,          NULL},
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

void cli_boot_mass(void)
{
  reset_usb_boot(0,0);
}

// Another digit pressed, update the parameter variable
void cli_digit(void)
{
  printf("\n%d", keypress);
  queue_token(TOK_KEY_0 + keypress - '0');
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

void cli_load_ki_reset(void)
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
    'N',
    "Normal Reset",
    cli_normal_reset,
   },
   {
    'I',
    "Load IAR",
    cli_load_iar,
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

// Spaces if empty or the value (in hex)

char *display_word(WORD w)
{
  static char result[16];
  
  sprintf(result, "%08X", w);
  return(result);
  
}

char *display_register_single_word(REGISTER_SINGLE_WORD x)
{
  static char result[16];
  
  if( x == 0xFFFFFFFF )
    {
      return("        ");
    }

  sprintf(result, "%08X", x);
  return(result);
}

char *display_address(REGISTER_SINGLE_WORD x)
{
  static char result[16];
  
  if( x == 0xFFFFFFFF )
    {
      return("  ");
    }

  sprintf(result, "%02X", x);
  return(result);
}

void update_display(ESC_STATE *s)
{
  if( s->update_display )
    {
      s->update_display = 0;
      printf("\n");
      printf("\nKeyboard register: %08X   IAR:%02X", s->keyboard_register, s->iar);
      printf("\n");

      // Print a representation of the TV display
      printf("\n%02X     %8s", s->iar, display_register_single_word(s->keyboard_register));
      if( s->ki_reset_flag )
	{
	  printf("\n%c",s->ki_reset_flag?'K':' ');
	}
      else
	{
	  printf("\n%02X     %8s", s->aux_iar, display_register_single_word(s->instruction_register));
	}
      
      printf("\n");
      printf("\n");
      printf("\n");
      if( s->address_register2 != 0xFFFFFFFF )
	{
	  WORD w = s->store[s->address_register2];
	  printf("\n%s     %8s", display_address(s->address_register2), display_word(w));
	}
      
    }
}

////////////////////////////////////////////////////////////////////////////////

int main(void)
{
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

  sleep_ms(2000);

  printf("\n");
  printf("\n                                  ********************************************");
  printf("\n                                  * Experimental Schools Computer Simulation *");
  printf("\n                                  ********************************************");
  printf("\n");

  // Main loop
  while(1)
    {
      drive_fsms();
      serial_loop();
      update_display(&esc_state);
    }
}
