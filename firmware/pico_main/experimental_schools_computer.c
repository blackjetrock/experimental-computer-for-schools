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
//
//
//

// The computer instance

ESC_STATE esc_state;


////////////////////////////////////////////////////////////////////////////////
//
// State machines
//
////////////////////////////////////////////////////////////////////////////////

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
     {CTOK_NUMERIC,   STATE_COMP_INIT,  numeric_a},
     {CTOK_ERROR,     STATE_COMP_INIT,  enter_error_state},
     {CTOK_END,       STATE_NULL,          NULL},
    }
   },
   {
    _STATE(STATE_ESC_ENT),
    null_entry_fn,
    null_every_fn,
    {
     {CTOK_NUMERIC,  STATE_COMP_ENT,      numeric_store},
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
      
      do_fsm(&esc_fsm, &state, tok);
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
    'O',
    "OLED Demo",
    cli_oled_demo,
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

int main(void)
{

  // Main loop
  while(1)
    {
      drive_fsms();
      serial_loop();
    }
}
