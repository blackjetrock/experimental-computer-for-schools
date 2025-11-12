////////////////////////////////////////////////////////////////////////////////
//
//
//
//
////////////////////////////////////////////////////////////////////////////////

typedef int TOKEN;
typedef int FSM_STATE;
typedef void FSM_DATA;

typedef void (*FSM_FNPTR)(FSM_DATA *s, TOKEN tok);

typedef struct _TRANSITION
{
  TOKEN stim;
  FSM_STATE state;
  FSM_FNPTR fn;
} TRANSITION;

#define NUM_TRANS  24

#define _STATE(XX)  XX, (char *) #XX
  
typedef struct _STATE
{
  FSM_STATE   state_num;
  char        *state_name;
  FSM_FNPTR   entry_fn;                // Called on entry
  FSM_FNPTR   pre_every_fn;            // Called before every stim is processed
                                       // even if no transition is taken
  TRANSITION trans[NUM_TRANS];
} STATE;

typedef struct _FSM
{
  const char *name;
  FSM_STATE current_state;
  STATE *table;
  TOKEN insert_tok;           // Drives token into FSM
} FSM;

#define STATE_NULL 0


//------------------------------------------------------------------------------
void do_fsm(FSM *fsm, FSM_DATA *s, TOKEN tok);
void do_fsm_core(FSM *fsm, FSM_DATA *s, TOKEN stim);
void print_fsm_info(FSM *fsm);
char *name_of_state(FSM *fsm, int state_num);
STATE *find_state_as_ptr(FSM *fsm, int state_num);
char *label_of_token(TOKEN tok);
void queue_token(TOKEN tok);
int unqueue_token(void);
int token_queue_empty(void);

