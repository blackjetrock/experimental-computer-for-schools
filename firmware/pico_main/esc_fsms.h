// Classification tokens
//
// 0x20..

typedef enum
  {
   CTOK_ZERO = 0x00,    // What deleted tokens look like
   CTOK_AC   = 0x10,

   CTOK_NUMERIC,
   CTOK_DEFAULT,
   CTOK_ERROR,
   CTOK_NONE,
   CTOK_END,

   CTOK_NEXT_UNUSED,
  } CTOK;

//------------------------------------------------------------------------------
// Instruction tokens

enum {
      TOK_NONE   = 0x00,
      TOK_NO_TOK = 0x01,
      
      TOK_KEY_LOAD_IAR,
      TOK_KEY_LOAD_ADDR,
      TOK_KEY_LOAD_STORE,
      TOK_KEY_LOAD_DEC_ADDR,
      TOK_KEY_LOAD_INCR_ADDR,
      TOK_KEY_LOAD_NORMAL_RESET,
      
      TOK_KEY_CLEAR,
      TOK_KEY_MINUS,
      TOK_KEY_DOT,
      TOK_KEY_1,
      TOK_KEY_2,
      TOK_KEY_3,
      TOK_KEY_4,
      TOK_KEY_5,
      TOK_KEY_6,
      TOK_KEY_7,
      TOK_KEY_8,
      TOK_KEY_9,
      TOK_KEY_0,
      
      TOK_KEY_A,
      TOK_KEY_B,
      TOK_KEY_C,

      TOK_KEY_RUN,
      TOK_KEY_STOP,
      TOK_KEY_KI_RESET,
      
      TOK_KEY_DUMP,
      TOK_KEY_CHECK,
      TOK_KEY_RELOAD,

};

//------------------------------------------------------------------------------
typedef struct _TOKEN_INFO
{
  TOKEN tok_value;
  const char *tok_name;
} TOKEN_INFO;


#define CTOK_NAMES_LEN (sizeof(ctok_names)/sizeof(TOKEN_INFO))
#define TOKEN_NAMES_LEN (sizeof(token_names)/sizeof(TOKEN_INFO))

extern TOKEN_INFO ctok_names[];
extern TOKEN_INFO token_names[];

char *name_of_token(TOKEN tok);