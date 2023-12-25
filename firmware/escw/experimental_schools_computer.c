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

char warning_message[200];

void warning(void)
{
  printf("\n*** WARNING: %s ***\n", error_message);
}

////////////////////////////////////////////////////////////////////////////////
//
// BCD
//
//
// Normalise a value after a binary addition
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
	  break;
	}
    }

#if DEBUG_BCD_CORRECTION
  printf("\nValue:%08X", v);
#endif

  return(v);
}

////////////////////////////////////////////////////////////////////////////////
//
// Nines complement
//

REGISTER_SINGLE_WORD bcd_nines_complement(REGISTER_SINGLE_WORD n)
{
  return(0x00999999 - n);
}


////////////////////////////////////////////////////////////////////////////////
// BCD single word addition
// Signed
//
// Both positive: Add, check for overflow
// Both negative: Add, check for overflow
// One negative, one positive: Nines complement negative, add, drop carry, add one, check for overflow (inverted)

REGISTER_SINGLE_WORD bcd_sw_addition(REGISTER_SINGLE_WORD a, REGISTER_SINGLE_WORD b)
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
  printf("\na(rs)=%08X  b(rs)=%08X", a, b);
#endif

  // If both nubers are positive or both are negative the just add the digits
  if( ((a_sign == WORD_SIGN_MINUS) && (b_sign == WORD_SIGN_MINUS)) ||
      ((a_sign == WORD_SIGN_PLUS) && (b_sign == WORD_SIGN_PLUS)) )
    {
#if DEBUG_SW_BCD_SUM
      printf("\nSigns identical");
#endif

      c = a + b;

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
#endif
	  sprintf(error_message, "Overflow (%08X)", c);
	  error();
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
      a = bcd_nines_complement(a);
    }

  if( b_sign == WORD_SIGN_MINUS )
    {
      b = bcd_nines_complement(b);
    }

#if DEBUG_SW_BCD_SUM
  printf("\n%s:a=%08X  b=%08X", __FUNCTION__, a, b);
#endif
  
#if DEBUG_SW_BCD_SUM
      printf("\nSigns different");
#endif

      c = a + b;
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

      sprintf(error_message, "Overflow (%08X)", c);
      error();
    }

  // Drop the carry
  c = CLEAR_SW_CARRY(c);
  
  // Add one
  c = c + 1;
  c = single_sum_normalise(c);

  // If result is negative then nines complement it and add one as we use sign plus digits form for numbers
  // This format matches he floating point format
  if( res_sign == WORD_SIGN_MINUS )
    {
      c = bcd_nines_complement(c);
      c = c + 1;
      c = single_sum_normalise(c);
    }
  
  // The sign of the result will be that of the largest argument absoulte value
  c = SET_SW_SIGN(c, res_sign);
    
#if DEBUG_SW_BCD_SUM
  printf("\nc=%08X (9c:%08X)", c, bcd_nines_complement(c));
#endif
  
  return(c);
}


////////////////////////////////////////////////////////////////////////////////

WORD load_from_store(ESC_STATE *s, ADDRESS address)
{
  return(s->store[address]);
}

REGISTER_DOUBLE_WORD get_register(ESC_STATE *s, int reg)
{
  return((REGISTER_DOUBLE_WORD)s->R[reg]);
}


void register_assign_register_literal(ESC_STATE *s, int dest,  int literal)
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

void register_assign_sum_register_literal(ESC_STATE *s, int dest, int src, int literal)
{
  if( IS_SW_REGISTER(dest) )
    {
      s->R[dest] = s->R[src] + (REGISTER_SINGLE_WORD) literal;
      s->R[dest] = single_sum_normalise(s->R[dest]);
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      s->R[dest] = s->R[src] + (REGISTER_DOUBLE_WORD) literal;
    }
}

void register_assign_sub_literal_register(ESC_STATE *s, int dest, int literal, int src)
{
  if( IS_SW_REGISTER(dest) )
    {
      s->R[dest] = (REGISTER_SINGLE_WORD) literal - s->R[src];
      s->R[dest] = single_sum_normalise(s->R[dest]);
    }
  
  if( IS_DW_REGISTER(dest) )
    {
      s->R[dest] = (REGISTER_DOUBLE_WORD) literal - s->R[src];
    }
}

void register_assign_sum_register_register(ESC_STATE *s, int dest, int src1, int src2)
{
  if( IS_SW_REGISTER(dest) && IS_SW_REGISTER(src1) && IS_SW_REGISTER(src2) )
    {
      s->R[dest] = s->R[src1] + s->R[src2];
      s->R[dest] = single_sum_normalise(s->R[dest]);
      return;
    }

  if( IS_DW_REGISTER(dest) && IS_DW_REGISTER(src1) && IS_DW_REGISTER(src2) )
    {
      s->R[dest] = s->R[src1] + s->R[src2];
      return;
    }

  // error
  sprintf(error_message, "Registers of different sizes");
  error();
}

////////////////////////////////////////////////////////////////////////////////
//
// IAR handling
//
// IAR is a simple two digit counter, plus a top half/bottom half flag for
// 4 digit instructions
//
// Logic:
//   Digit a determines type of instruction
//    If a = 7,8, or 9 then IAR incremented and half flag cleared (it shouldn't be set)
//    Otherwise
//       If half flag is set then clear it and increment IAR
//       Otherwise set half flag
//
////////////////////////////////////////////////////////////////////////////////
//

void next_iar(ESC_STATE *s)
{
  int digit_a = INST_A_FIELD(s->instruction_register);
  
  switch(digit_a)
    {
    case 7:
    case 8:
    case 9:
      if( s->iar.a_flag )
	{
	  sprintf(warning_message, "8 digit instruction has A flag set");
	  warning();
	}
      
      // 8 digit instruction so move to next address
      s->iar.address++;
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
	  s->iar.address++;
	  s->iar.a_flag = 0;
	}
      else
	{
	  s->iar.a_flag = !(s->iar.a_flag);
	}
      break;
      
    default:
      // error
      sprintf(error_message, "Unknown digit a");
      error();
      
      break;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// Instruction decode
//
// Completes instructions that require a third stage
// Moves IAR to next instruction

void stage_c_decode(ESC_STATE *s)
{
  // Decode the instruction
  // First the digits 1-4

  switch(s->inst_digit_a)
    {
    case 0:
    case 1:
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
	case 4:
	  // Unconditional branch
	  // IAR already set up.
	  break;
	}
      
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

////////////////////////////////////////////////////////////////////////////////
// Stage B
// Decode absolute addresses
// Execute if possible
////////////////////////////////////////////////////////////////////////////////

void stage_b_decode(ESC_STATE *s)
{
  // Decode the instruction
  // First the digits 1-4
  //  stage_b_decode_core(s, 0);

  int extreme_left_digit = 0;
    
  switch(s->inst_digit_a)
    {
    case 0:
      switch(s->inst_digit_b)
	{
	case 0:
	  register_assign_sum_register_literal(s, s->reginst_rc, s->reginst_rc, s->reginst_literal);
	  break;

	case 1:
	  register_assign_sum_register_literal(s, s->reginst_rc, s->reginst_rc, -(s->reginst_literal));
	  break;

	case 2:
	  register_assign_sub_literal_register(s, s->reginst_rc, s->reginst_literal, s->reginst_rc);
	  break;
	  
	case 3:
	  register_assign_register_literal(s, s->reginst_rc, s->reginst_literal);
	  break;

	case 5:
#if DEBUG_TEST
	  printf("\nTEST (5) ");
#endif
	  switch(s->inst_digit_d)
	    {
	    case 0:
#if DEBUG_TEST
	      printf("\nTEST (5) ");
#endif
	      
	      if( s->R[s->reginst_rc] == 0 )
		{
		  s->control_latch = 1;
		}
	      else
		{
		  s->control_latch = 0;
		}
	      break;
	      
	    case 1:
	      if( s->R[s->reginst_rc] > 0 )
		{
		  s->control_latch = 1;
		}
	      else
		{
		  s->control_latch = 0;
		}
	      break;
	      
	    case 2:
	      if( s->R[s->reginst_rc] < 0 )
		{
		  s->control_latch = 1;
		}
	      else
		{
		  s->control_latch = 0;
		}
	      break;
	      
	    case 3:

	      if( IS_SW_REGISTER(s->reginst_rc) )
		{
		  extreme_left_digit = (s->reginst_rc & 0x00F00000) >> (5*4);
		}

	      if( IS_DW_REGISTER(s->reginst_rc) )
		{
		  extreme_left_digit = (s->reginst_rc & 0x0000F00000000000) >> (11*4);
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

	    }
	  break;

	  // Shift (Rc) left d places 
	case 6:
	  register_assign_register_literal(s, s->reginst_rc, get_register(s, s->reginst_rc) << (4*(s->reginst_literal)));
	  break;
	  
	  // Shift (Rc) right d places
	case 7:
	  register_assign_register_literal(s, s->reginst_rc, get_register(s, s->reginst_rc) >> (4*(s->reginst_literal)));
	  break;
	}
      break;
      
    case 1:
      switch(s->inst_digit_b)
	{
	case 0:
	  // Add registers
	  register_assign_sum_register_register(s, s->reginst_rc, s->reginst_rc, s->reginst_rd);
	    break;
	  
	case 3:
	  register_assign_sum_register_literal(s, s->reginst_rc, s->reginst_rd, 0);
	  break;

	  // Shift (Rc) left (Rd) places 
	case 6:
	  register_assign_register_literal(s,
					   s->reginst_rc,
					   get_register(s, s->reginst_rc) << (4*get_register(s, (s->reginst_rd))));
	  break;
	  
	  // Shift (Rc) right (Rd) places
	case 7:
	  register_assign_register_literal(s,
					   s->reginst_rc,
					   get_register(s, s->reginst_rc) >> (4*get_register(s, (s->reginst_rd))));
	  break;

	}

      break;

      // Single address instructions
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      switch(s->inst_digit_a)
	{
	case 2:
	  switch(s->inst_digit_b)
	    {
	    case 0:
	      break;
	      
	    case 1:
	      break;
	      
	    case 2:
	      break;
	      
	    case 3:
	      break;
	      
	    case 4:
	      // Unconditional branch
	      // Move the IAR on to the next address and store that in the link register
	      next_iar(s);
	      s->link_register = s->iar.address;

	      // Now over-write that IAR with the address we want to jump to
	      s->iar.address = s->inst_aa;
	      s->iar.a_flag = 0;
	      
	      break;
	      
	    case 5:
	      break;
	      
	    case 6:
	      break;
	      
	    case 7:
	      break;
	      
	    case 8:
	      break;
	      
	    case 9:
	      break;
	      
	    }
	  break;
	}
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
  printf("\nabcd = %d%d%d%d",
	 s->inst_digit_a,
	 s->inst_digit_b,
	 s->inst_digit_c,
	 s->inst_digit_d);
#endif

  // Pre-calculate the Aa field of the instruction for some later
  // decoding

  s->inst_ap = (s->inst_digit_c) *10 + (s->inst_digit_d);
  
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
	  break;
	  
	case 4:
	  // Not used
	  break;
	  
	case 5:
	  // TEST
	  // Performd in stage B
	  break;

	case 6:
	case 7:
	  s->reginst_rc = s->inst_digit_c;
	  s->reginst_rd = s->inst_digit_d;
	  s->reginst_literal = s->inst_digit_d;	  
	  break;
	}
      break;
      
    case 1:
      // Register instructions
      s->reginst_rc = s->inst_digit_c;
      s->reginst_rd = s->inst_digit_d;
      break;

    case 2:
      // Absolute addressing
      s->inst_aa = s->inst_ap;
      break;

      // relative addressing
    case 3:
      s->inst_aa = s->inst_ap + get_register(s, s->R[3]);
      break;

    case 4:
      s->inst_aa = s->inst_ap + get_register(s, s->R[4]);
      break;
      
    case 5:
      s->inst_aa = s->inst_ap + get_register(s, s->R[5]);
      break;
      
    case 6:
      // Indirect addressing
      s->inst_aa = s->store[s->inst_ap];
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
  // Set instruction stage
  s->stage = 'B';

  stage_b_decode(s);
  
  s->update_display = display;
}

void run_stage_c(ESC_STATE *s, int display)
{
  // Set instruction stage
  s->stage = 'C';

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
  es->reginst_literal = NO_VALUE;
  
  // Re-display
  es->update_display = 1;
}

void state_esc_ki_reset(FSM_DATA *s, TOKEN tok)
{
  ESC_STATE *es;

  es = (ESC_STATE *)s;

  // Everything cleared except IAR

  es->stage = ' ';
  es->keyboard_register = EMPTY_ADDRESS;
  
  es->ki_reset_flag = 1;
  es->address_register0 = EMPTY_ADDRESS;
  es->address_register1 = EMPTY_ADDRESS;
  es->address_register2 = EMPTY_ADDRESS;

  es->reginst_rc = NO_VALUE;
  es->reginst_rd = NO_VALUE;
  es->reginst_literal = NO_VALUE;
  
  // Re-display
  es->update_display = 1;
}

//------------------------------------------------------------------------------

void state_esc_a_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;

  s = (ESC_STATE *)es;

  switch(s->stage)
    {
    case ' ':
      if( s->ki_reset_flag )
	{
	  s->instruction_register = s->keyboard_register;
	}
      
      run_stage_a(s, display_flag);
      break;

    case 'A':
      break;
      
    case 'B':
      break;

      // If in stage C then next instruction is ready to go
    case 'C':
      run_stage_a(s, display_flag);
      break;
    }
  
  s->update_display = display_flag;
}


void state_esc_a_disp(FSM_DATA *es, TOKEN tok)
{
  state_esc_a_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_a_no_disp(FSM_DATA *es, TOKEN tok)
{
  state_esc_a_core(es, tok, DISPLAY_NO_UPDATE);
}

//------------------------------------------------------------------------------

void state_esc_b_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;

  s = (ESC_STATE *)es;

  switch(s->stage)
    {
    case ' ':
      if( s->ki_reset_flag )
	{
	  s->instruction_register = s->keyboard_register;
	}
      
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

  s->update_display = display_flag;
}

void state_esc_b_disp(FSM_DATA *es, TOKEN tok)
{
  state_esc_b_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_b_no_disp(FSM_DATA *es, TOKEN tok)
{
  state_esc_b_core(es, tok, DISPLAY_NO_UPDATE);
}

//------------------------------------------------------------------------------

void state_esc_c_core(FSM_DATA *es, TOKEN tok, int display_flag)
{
  ESC_STATE *s;

  s = (ESC_STATE *)es;

    switch(s->stage)
    {
    case ' ':
      if( s->ki_reset_flag )
	{
	  s->instruction_register = s->keyboard_register;
	}
      
      run_stage_a(s, display_flag);
      run_stage_b(s, display_flag);
      run_stage_c(s, display_flag);
      break;

    case 'A':
      run_stage_b(s, display_flag);
      run_stage_c(s, display_flag);
      break;

    case 'B':
      run_stage_c(s, display_flag);
      break;

    case 'C':
      run_stage_a(s, display_flag);
      run_stage_b(s, display_flag);
      run_stage_c(s, display_flag);
      break;
    }

  s->update_display = display_flag;
}

void state_esc_c_disp(FSM_DATA *es, TOKEN tok)
{
  state_esc_c_core(es, tok, DISPLAY_UPDATE);
}

void state_esc_c_no_disp(FSM_DATA *es, TOKEN tok)
{
  state_esc_c_core(es, tok, DISPLAY_NO_UPDATE);
}

//------------------------------------------------------------------------------

void state_esc_run(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  s->run = 1;
}

void state_esc_stop(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  s->stop = 1;
}

// Run at full speed

void state_esc_execute(FSM_DATA *es, TOKEN tok)
{
  ESC_STATE *s = (ESC_STATE *)es;

  // If not running then exit, nothing to do
  if( s->run )
    {
      // Check for stop
      if( s->stop )
	{
	  // Turn off the execution
	  // As we run to stage C for eah instruction this stops at the end
	  // of stage C for the current instruction.
	  s->run = 0;
	  s->stop = 0;
	  return;
	}
      
      // Run to end of stage C repeatedly
      state_esc_c_no_disp(s, tok);
    }
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
     {TOK_KEY_KI_RESET,     STATE_ESC_INIT,  state_esc_ki_reset},
     {TOK_KEY_LOAD_IAR,     STATE_ESC_INIT,  state_esc_load_iar},
     {TOK_KEY_LOAD_ADDR,    STATE_ESC_INIT,  state_esc_load_addr},
     {TOK_KEY_INCR_ADDR,    STATE_ESC_INIT,  state_esc_incr_addr},
     {TOK_KEY_DECR_ADDR,    STATE_ESC_INIT,  state_esc_decr_addr},
     {TOK_KEY_LOAD_STORE,   STATE_ESC_INIT,  state_esc_load_store},
     {TOK_KEY_A,            STATE_ESC_INIT,  state_esc_a_disp},
     {TOK_KEY_B,            STATE_ESC_INIT,  state_esc_b_disp},
     {TOK_KEY_C,            STATE_ESC_INIT,  state_esc_c_disp},
     {TOK_KEY_RUN,          STATE_ESC_INIT,  state_esc_run},
     {TOK_KEY_STOP,         STATE_ESC_INIT,  state_esc_stop},
     
     // Exeute code at full speed
     {TOK_NO_TOK,           STATE_ESC_INIT,  state_esc_execute},
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

void cli_update_display(void)
{
  ESC_STATE *s = &esc_state;

  s->update_display = 1;
}

// Dump state info
void cli_dump(void)
{
  ESC_STATE *s = &esc_state;

  printf("\nState");
  printf("\n=====");

  printf("\nIAR           : %s", display_iar(s->iar));
  printf("\nAux IAR       : %s", display_iar(s->aux_iar));
  printf("\nInst Reg      : %08X", s->instruction_register);
  printf("\nControl latch : %d   Run:%d   Stop:%d", s->control_latch, s->run, s->stop);
  printf("\nAddr R0       : %s", display_address(s->address_register0));
  printf("\nAddr R1       : %s", display_address(s->address_register1));
  printf("\nAddr R2       : %s", display_address(s->address_register2));

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

  printf("\nReg Inst Rc   : %d", s->reginst_rc);
  printf("\nReg Inst Rd   : %d", s->reginst_rd);
  printf("\nReg Inst Lit  : %d", s->reginst_literal);
  printf("\nInst Aa       : %02X   ap : %02X ", s->inst_aa, s->inst_ap);
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
  
  c = bcd_sw_addition(a, b);
  
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
   {0xA0000010, 0xA0000020, 0xA0000030},
   {0xA0000020, 0xA0000010, 0xA0000030},
   {0xA0000007, 0xA0000005, 0xA0000012},
   {0xB0000020, 0xB0000010, 0xB0000030},
   {0xA0123456, 0xA0654321, 0xA0777777},
   {0xB0123456, 0xB0654321, 0xB0777777},
   {0xB0077777, 0xB0777777, 0xB0855554},
   {0xA0002345, 0xB0000045, 0xA0002300},
   {0xA0000045, 0xB0002345, 0xB0002300},

   {0xA0999900, 0xA0000099, 0xA0999999},
   {0xA0999900, 0xA0000100, 0xA0000000},

   {0xB0999900, 0xB0000099, 0xB0999999},
   {0xB0999900, 0xB0000100, 0xB0000000},

   
  };

#define NUM_SW_BCD_TEST (sizeof(sw_bcd_test)/sizeof(SW_BCD_TEST))

void cli_test_bcd(void)
{
  int a, b, c;
  
  // Perform some arithmetic
  for(int i=0; i<NUM_SW_BCD_TEST; i++)
    {
      test_sw_add(sw_bcd_test[i].a, sw_bcd_test[i].b, sw_bcd_test[i].r);
    }
  
  printf("\n");
}

void cli_load_test_code_2(void)
{
  ESC_STATE *s = &esc_state;
  int i = 0;
  
  // R5 and 6 have 2 and 1 in them
  s->store[i++] = 0x03520361;

  // Add R5 and R6 and add 6
  s->store[i++] = 0x10560000;

  // Branch to 1 to count forever
  s->store[i++] = 0x24012401;
  
  // Stop Executing
  s->store[i++] = 0x06510751;

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

void update_display(void)
{
  char dsp[(MAX_LINE+5)*(NUM_LINES+2)+1];
  char tmp[MAX_LINE*NUM_LINES+1];

  ESC_STATE *s = &esc_state;

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

  stdio_init_all();

  sleep_ms(2000);

  printf("\n");
  printf("\n                                  ********************************************");
  printf("\n                                  * Experimental Schools Computer Simulation *");
  printf("\n                                  ********************************************");
  printf("\n");

#if 0
  
  // Main loop
  while(1)
    {
#if DEBUG_LOOP
      printf("\nLoop");
#endif
      drive_fsms();
      serial_loop();
      update_display(&esc_state);
    }

#endif
  wifi_main();
}
