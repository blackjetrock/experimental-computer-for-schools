
#include "switches.h"

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

////////////////////////////////////////////////////////////////////////////////
//
// Start the execution of an extracode instruction.
//

void enter_extracode(ESC_STATE *s)
{
  // Set up TAR1..TAR3
  s->store[100] = s->Aa1;
  s->store[101] = s->Aa2;
  s->store[102] = s->Aa3;

  // Put the link address in the link register
  next_iar(s);
  s->link_register = s->iar.address;
   
  // Now set up the IAR to the first instruction of this extracode instruction
  s->iar.address = 0x110+s->inst_digit_b;
  s->iar.a_flag = 0;

  // Now run the stage a of the instruction we ar epointing at, this will over-ride the
  // three instruction stage a
  // Execution will continue until a return to program

  // Indicate we are executing extracode instructions
  
  s->extracode = 1;
  
  // instruction must come from store
  s->instruction_register = load_from_store(s, s->iar.address);

  // Run first part of first extracode instruction
  run_stage_a(s, DISPLAY_NO_UPDATE);
  
}


void load_extracode(ESC_STATE *s)
{
  for(int i = 0; i<100; i++)
    {
      s->store[100+i] = extracode_fp[i];
    }
}
