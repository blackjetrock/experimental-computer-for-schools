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
//
//
char error_message[200];

void error(void)
{
  printf("\n*** %s ***\n", error_message);
}


////////////////////////////////////////////////////////////////////////////////

WORD load_from_store(ESC_STATE *s, ADDRESS address)
{
  return(s->store[address]);
}

void register_assign_literal(ESC_STATE *s, int dest, int literal)
{
  if( IS_SW_REGISTER(dest) )
    {
      s->R[dest] = (REGISTER_SINGLE_WORD) literal;
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      s->R[dest] = (REGISTER_DOUBLE_WORD) literal;
    }
}

void register_assign_register(ESC_STATE *s, int dest, int src)
{
  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src) )
    {
      s->R[dest] = s->R[src];
      return;
    }

  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src) )
    {
      s->R[dest] = s->R[src];
      return;
    }

  // error
  sprintf(error_message, "Registers of different sizes");
  error();
}

////////////////////////////////////////////////////////////////////////////////
//

void increment_iar(IAR *iar)
{
  if( iar->a_flag )
    {
      iar->address++;
      iar->a_flag = 0;
    }
  else
    {
      iar->a_flag = !(iar->a_flag);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Instruction decode
//

void stage_c_decode(ESC_STATE *s)
{
  // Decode the instruction
  // First the digits 1-4
  stage_c_decode(s);

  switch(s->inst_digit_a)
    {
    case 0:
    case 1:
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      break;
      
    case 7:
    case 8:
    case 9:
      break;
    }
}

void stage_b_decode_core(ESC_STATE *s, int shift)
{
  // Decode the instruction
  // First the digits 1-4


  switch(s->inst_digit_a)
    {
    case 0:
    case 1:
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      break;
      
    case 7:
      // Absolute
      
      s->Aa1 = s->Ap1;
      s->Aa2 = s->Ap2;
      s->Aa3 = s->Ap3;
      break;
      
    case 8:
      // Relative
      
      s->Aa1 = s->Ap1 + s->R3; 
      s->Aa2 = s->Ap2 + s->R4; 
      s->Aa3 = s->Ap3 + s->R5; 
      break;
      
    case 9:
      // Indirect
      s->Aa1 = s->store[s->Ap1];
      s->Aa2 = s->store[s->Ap2];
      s->Aa3 = s->store[s->Ap3];
      break;
    }
}

// Stage B
// Decode absolute addresses
// Execute if possible

void stage_b_decode(ESC_STATE *s)
{
  // Decode the instruction
  // First the digits 1-4
  //  stage_b_decode_core(s, 0);

  switch(s->inst_digit_a)
    {
    case 0:
      switch(s->inst_digit_b)
	{
	case 3:
	  register_assign_literal(s, s->reginst_rc, s->reginst_rd);
	  break;
	}
      break;      
    case 1:
            switch(s->inst_digit_b)
	{
	case 3:
	  register_assign_register(s, s->reginst_rc, s->reginst_rd);
	  break;
	}

      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      break;
      
    case 7:
    case 8:
    case 9:
      // Indirect
      s->Aa1 = s->store[s->Ap1];
      s->Aa2 = s->store[s->Ap2];
      s->Aa3 = s->store[s->Ap3];
      break;
    }
}

// Fetch instruction
// Decode presumptive addresses

void stage_a_decode(ESC_STATE *s)
{
  if( s->iar.a_flag )
    {
      s->inst_digit_e = INST_E_FIELD(s->instruction_register);
      s->inst_digit_f = INST_F_FIELD(s->instruction_register);
      s->inst_digit_g = INST_G_FIELD(s->instruction_register);
      s->inst_digit_h = INST_H_FIELD(s->instruction_register);
    }
  else
    {
      s->inst_digit_a = INST_A_FIELD(s->instruction_register);
      s->inst_digit_b = INST_B_FIELD(s->instruction_register);
      s->inst_digit_c = INST_C_FIELD(s->instruction_register);
      s->inst_digit_d = INST_D_FIELD(s->instruction_register);
    }

#if DEBUG_A_DECODE
  printf("\nabcd = %d%d%d%d",
	 s->inst_digit_a,
	 s->inst_digit_b,
	 s->inst_digit_c,
	 s->inst_digit_d);
#endif
  
  // Calculate the presumptive addresses
  
  switch(s->inst_digit_a)
    {
    case 0:
    case 1:
      // Register instructions
      s->reginst_rc = s->inst_digit_c;
      s->reginst_rd = s->inst_digit_d;
      break;

    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      break;
      
    case 7:
    case 8:
    case 9:
      // 3 address instructions
      s->Ap1 = INST_3_ADDR_1(s->instruction_register);
      s->Ap2 = INST_3_ADDR_2(s->instruction_register);
      s->Ap3 = INST_3_ADDR_3(s->instruction_register);
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////

// Execution
// Basic instructions are stored two to a word
// The two instructions are processed, in turn, digit position 1-4
// then digit position 5-8

void run_stage_a(ESC_STATE *s, int display)
{
  // Set instruction stage
  s->stage = 'A';
  
  // load aux iar
  s->aux_iar = s->iar;
  
  // Load instruction into instruction register
  s->instruction_register = load_from_store(s, s->aux_iar.address);

  // Decode instruction
  stage_a_decode(s);
  
  s->update_display = display;
}

//
// Stage B
//
// The addresses have been alculated, now the instruction is executed
//

void run_stage_b(ESC_STATE *s, int display)
{
  stage_b_decode(s);
  
  s->update_display = display;
}

void run_stage_c(ESC_STATE *s, int display)
{
  stage_c_decode(s);
  
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

  es->iar.address = (ADDRESS)es->keyboard_register;
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

  es->stage = ' ';
  es->keyboard_register = EMPTY_ADDRESS;
  
  es->ki_reset_flag = 0;
  es->address_register0 = EMPTY_ADDRESS;
  es->address_register1 = EMPTY_ADDRESS;
  es->address_register2 = EMPTY_ADDRESS;

  es->reginst_rc = NO_VALUE;
  es->reginst_rd = NO_VALUE;
  
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

// Dump state info
void cli_dump(void)
{
  ESC_STATE *s = &esc_state;

  printf("\nState");
  printf("\n=====");

  printf("\nIAR     : %s", display_iar(s->iar));
  printf("\nAux IAR : %s", display_iar(s->aux_iar));
  printf("\nInst Reg: %08X", s->instruction_register);
  printf("\nAddr R0 : %s", display_address(s->address_register0));
  printf("\nAddr R1 : %s", display_address(s->address_register1));
  printf("\nAddr R2 : %s", display_address(s->address_register2));

  for(int i=0; i<NUM_WORD_REGISTERS; i++)
    {
      printf("\nR%d    : %s", i, display_register_single_word(s->R[i]));
    }

  for(int i=0; i<NUM_DBL_WORD_REGISTERS; i++)
    {
      printf("\nR%d    : %s", i+NUM_WORD_REGISTERS, display_register_double_word(s->RD[i]));
    }
  
  printf("\n");
  printf("\nInternal");
  printf("\n========");

  printf("\nReg Inst Rc  : %d", s->reginst_rc);
  printf("\nReg Inst Rd  : %d", s->reginst_rd);

  printf("\n");
}

// Dump Store
void cli_dump_store(void)
{
    ESC_STATE *s = &esc_state;

    for(int i=0; i<STORE_SIZE; i++)
      {
	if( (i % 5) == 0 )
	  {
	    printf("\n");
	  }

	printf("  %03d:%08X (1234.56-)", i, s->store[i]);
      }
    printf("\n");
	   
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
    '*',
    "Dump State",
    cli_dump,
   },
   {
    '&',
    "Dump Store",
    cli_dump_store,
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
  static char result[MAX_LINE];
  
  sprintf(result, "%08X", w);
  return(result);
  
}

char *display_register_single_word(REGISTER_SINGLE_WORD x)
{
  static char result[MAX_LINE];
  
  if( x == 0xFFFFFFFF )
    {
      return("        ");
    }

  sprintf(result, "%08X", x);
  return(result);
}

char *display_register_double_word(REGISTER_DOUBLE_WORD x)
{
  static char result[MAX_LINE];
  
  if( x == 0xFFFFFFFFFFFFFFFFL )
    {
      return("        ");
    }

  sprintf(result, "%16lX", x);
  return(result);
}


char *display_register_and_contents(ESC_STATE *s, int regno)
{
  static char result[MAX_LINE*2];
  
  if( IS_SW_REGISTER(regno) )
    {
      sprintf(result, "R%d   %s", regno, display_register_single_word(s->R[regno]));
    }
  return(result);
}

char *display_address(REGISTER_SINGLE_WORD x)
{
  static char result[MAX_LINE];
  
  if( x == 0xFFFFFFFF )
    {
      return("  ");
    }

  sprintf(result, "%02X", x);
  return(result);
}


char *display_iar(IAR iar)
{
  static char result2[MAX_LINE];
  
  sprintf(result2, "%02X%c", iar.address, iar.a_flag?'A':' ');
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
// We append to a stringot get the display, this allows formatting in the
// functions we call to be shown.
//

void update_display(ESC_STATE *s)
{
  char dsp[(MAX_LINE+5)*(NUM_LINES+2)+1];
  char tmp[MAX_LINE*NUM_LINES+1];
  
  if( s->update_display )
    {
      s->update_display = 0;
      printf("\n");

      printf("\nKeyboard register: %08X   IAR:%02X", s->keyboard_register, display_iar(s->iar));
      printf("\n");

      //
      // Print a representation of the TV display
      //

      strcpy(dsp, "   012345678901234");
	     
      // Line 1
      sprintf(tmp, "\n1: %02s   %8s",
	     display_iar(s->iar),
	     display_register_single_word(s->keyboard_register));
      strcat(dsp, tmp);
      
      // Line 2
      if( s->ki_reset_flag )
	{
	  sprintf(tmp, "\n2: %c",s->ki_reset_flag?'K':' ');
	}
      else
	{
	  sprintf(tmp, "\n2: %2s   %8s%c",
		 display_iar(s->aux_iar),
		 display_register_single_word(s->instruction_register),
		 s->stage
		 );
	}
      strcat(dsp, tmp);
      
      // Line 3
      sprintf(tmp, "\n3: %s",  display_presumptive_address_1(s));
      strcat(dsp, tmp);
      sprintf(tmp, "\n4: %s",  display_presumptive_address_2(s));
      strcat(dsp, tmp);
      
      sprintf(tmp, "\n5: ");
      strcat(dsp, tmp);
      
      // Line 6
      if( s->address_register2 != 0xFFFFFFFF )
	{
	  WORD w = s->store[s->address_register2];
	  sprintf(tmp, "\n6: %s     %8s", display_address(s->address_register2), display_word(w));
	}
      strcat(dsp, tmp);

      // Now update the display output device(s)
      printf("\n%s\n", dsp);
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
