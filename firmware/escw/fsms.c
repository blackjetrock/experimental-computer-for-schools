
////////////////////////////////////////////////////////////////////////////////
//
// FSMs
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "fsms.h"
#include "esc_fsms.h"
#include "match_token.h"
#include "esc.h"


////////////////////////////////////////////////////////////////////////////////


STATE *find_state_as_ptr(FSM *fsm, int state_num)
{
  STATE *state_ptr = fsm->table;
    
  // Find the current state
  while(state_ptr->state_num != STATE_NULL )
    {
      if( state_num == state_ptr->state_num )
	{
	  return(state_ptr);
	}
      
      state_ptr++;
    }
  
  return(NULL);
}

char *name_of_state(FSM *fsm, int state_num)
{
  STATE *sp = find_state_as_ptr(fsm, state_num);

  if( sp == NULL )
    {
      return((char *)"???");
    }

  return((char *)sp->state_name);
}

void print_fsm_info(FSM *fsm)
{
  printf("\nFSM:%s", fsm->name);
  printf("   %s (%d)", name_of_state(fsm, fsm->current_state), fsm->current_state);

}

void do_fsm_core(FSM *fsm, FSM_DATA *s, TOKEN tok)
{
#if DEBUG_FN_TRACE
  printf("\nTRACE:%s", __FUNCTION__);
#endif
  
  STATE *state_ptr = fsm->table;
  
  // Find the current state
  state_ptr = find_state_as_ptr(fsm, fsm->current_state);

  char *from_st = name_of_state(fsm, fsm->current_state);
 
#if DEBUG_FSM
  if( (tok != TOK_NO_TOK) && (tok != CTOK_NONE))
    {
      printf("\nTok:%03X  Current_state:%s", tok, from_st);
    }
#endif
  
  if( state_ptr != NULL )
    {
      (state_ptr->pre_every_fn)(s, tok);
      
      // Find the token and hence the transition
      for(int i=0; i<NUM_TRANS; i++)
	{
	  // If the next state pointer is NULL then this is an end of transition
	  // table marker
	  if( state_ptr->trans[i].state == STATE_NULL )
	    {
	      break;
	    }

	  if( match_token(tok, state_ptr->trans[i].stim) )
	    {
	      // Do the transition after we call the function
	      char *to_st = name_of_state(fsm, state_ptr->trans[i].state);
	      int n_from_st = fsm->current_state;
	      int n_to_st = state_ptr->trans[i].state;
	      
#if DO_LOG
	      fprintf(log_fp, "\nTransition tok:%15s %s (%d)",

		      name_of_token(tok),
		      from_st, n_from_st);

	      fprintf(log_fp, " => %s (%d)",
		      to_st, n_to_st);
	      fflush(log_fp);
#endif		      

#if DEBUG_CALC_STATE
	      int do_print_status = 0;
#endif

#if DEBUG_CALC_STATE
	      switch(tok)
		{

		case CTOK_NONE:
		case TOK_NO_TOK:
		case TOK_NONE:
		  // Only display real transitions for these stimulii
		  if( fsm->current_state != state_ptr->trans[i].state )
		    {
		      printf("\nTransition tok:%15s (tok:%03X) %s (%d) (i=%d) (",
			     
			     name_of_token(tok),
			     tok,
			     from_st, n_from_st,
			     i);
		      
		      printf(" => %s (%d)",
			     to_st, n_to_st);
		      
		      do_print_status = 1;
		    }
		  break;

		default:
		  printf("\nTransition tok:%15s (tok:%03X) %s (%d) (i=%d) (",
			 
			 name_of_token(tok),
			 tok,
			 from_st, n_from_st,
			 i);
		  
		  printf(" => %s (%d)",
			 to_st, n_to_st);
		  
		  do_print_status = 1;
		  break;
		}
	      
#endif		      
	      if( state_ptr->trans[i].fn !=NULL )
		{
		  (*state_ptr->trans[i].fn)(s, tok);
		}

#if DEBUG_CALC_STATE
	      if( do_print_status )
		{
		  print_state(s);
		}
#endif

	      // Move to new state
	      // Call entry action if state changed
	      int run_entry_action = 0;
	      
	      if( fsm->current_state != state_ptr->trans[i].state )
		{
		  run_entry_action = 1;
		}

	      // Transition to new state
	      fsm->current_state = state_ptr->trans[i].state;
	      
	      if( run_entry_action )
		{
		  STATE *sp = find_state_as_ptr(fsm, fsm->current_state);

		  if( sp != NULL )
		    {
#if DEBUG_CALC_STATE
		      printf("\n  Entry_fn executed");
#endif

		      (sp->entry_fn)(s, tok);
		    }
		}

	      // Done
	      return;
	    }
	}
    }
}

// Perform tok in an FSM
void do_fsm(FSM *fsm, FSM_DATA *s, TOKEN tok)
{
  // Is a token pushed on the insert stack?
  if( fsm->insert_tok != TOK_NONE )
    {
      do_fsm_core(fsm, s,fsm->insert_tok);
    }

  do_fsm_core(fsm, s, tok);
}

////////////////////////////////////////////////////////////////////////////////
//
// Token queue
//
////////////////////////////////////////////////////////////////////////////////

// Tokens are sent to this queue to be executed.
// They come from the keyboard and also the executing program.
#define TOKEN_QUEUE_LENGTH 10

int token_queue[TOKEN_QUEUE_LENGTH];

// When in and out are equal, queue is empty
TOKEN token_queue_in  = 0;
TOKEN token_queue_out = 0;
TOKEN token_queue_overflow = 0;

void queue_token(TOKEN tok)
{
  int next_in = ((token_queue_in+1) % TOKEN_QUEUE_LENGTH);

  // If queue would be empty if we added this token then we have overflow
  
  if( next_in == token_queue_out )
    {
      token_queue_overflow = 1;
    }
  else
    {
      // Add token to queue
      token_queue[next_in] = tok;
      token_queue_in = next_in;
    }
}

// Take a token from the queue
// If no token then TOK_NO_TOK is returned

int unqueue_token(void)
{
  TOKEN tok = TOK_NO_TOK;;
  
  // Empty?
  if( token_queue_in == token_queue_out )
    {
      // No token
    }
  else
    {
      // Get token from queue
      int next_out = ((token_queue_out+1) % TOKEN_QUEUE_LENGTH);
      
      tok = token_queue[next_out];
      token_queue_out = next_out;
    }

  return(tok);
}

int token_queue_empty(void)
{
  return(token_queue_in == token_queue_out);
}

