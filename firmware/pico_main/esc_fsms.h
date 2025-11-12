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
      
      TOK_KEY_LOAD_IAR     = 2,
      TOK_KEY_LOAD_ADDR    = 3,
      TOK_KEY_LOAD_STORE   = 4,
      TOK_KEY_DECR_ADDR    = 5,
      TOK_KEY_INCR_ADDR    = 6,
      TOK_KEY_NORMAL_RESET = 7,
      
      TOK_KEY_CLEAR  = 8,
      TOK_KEY_MINUS  = 9,
      TOK_KEY_DOT    = 10,
      TOK_KEY_0      = 11,
      TOK_KEY_1,
      TOK_KEY_2,
      TOK_KEY_3,
      TOK_KEY_4,
      TOK_KEY_5,
      TOK_KEY_6,
      TOK_KEY_7,
      TOK_KEY_8,
      TOK_KEY_9      = 20,
      
      TOK_KEY_A      = 21,
      TOK_KEY_B      = 22,
      TOK_KEY_C      = 23,

      TOK_KEY_RUN    = 24,
      TOK_KEY_STOP   = 25,
      TOK_KEY_KI_RESET  = 26,
      
      TOK_KEY_DUMP      = 27,
      TOK_KEY_CHECK     = 28,
      TOK_KEY_RELOAD    = 29,

      // Check results at this point
      TOK_TEST_CHECK_RES,

      // Wait while testing until a stop is seen
      TOK_TEST_WAIT_FOR_STOP,
};

//------------------------------------------------------------------------------
typedef struct _TOKEN_INFO
{
  TOKEN tok_value;
  const char *tok_name;
} TOKEN_INFO;

#define TOK_KEY_SETUP  TOK_KEY_CHECK

#define CTOK_NAMES_LEN (sizeof(ctok_names)/sizeof(TOKEN_INFO))
#define TOKEN_NAMES_LEN (sizeof(token_names)/sizeof(TOKEN_INFO))

extern TOKEN_INFO ctok_names[];
extern TOKEN_INFO token_names[];

char *name_of_token(TOKEN tok);

//------------------------------------------------------------------------------

enum
  {
   STATE_ESC_INIT = 1,
   STATE_ESC_RELOAD,
   STATE_ESC_DELETE,
   STATE_ESC_SETUP,
  };
