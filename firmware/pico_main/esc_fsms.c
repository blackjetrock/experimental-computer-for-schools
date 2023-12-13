////////////////////////////////////////////////////////////////////////////////

#include "fsms.h"
#include "esc_fsms.h"

TOKEN_INFO ctok_names[] =
  {
   {CTOK_AC,           "CTOK_AC"},
   {CTOK_NUMERIC,      "CTOK_NUMERIC"},
   {CTOK_ERROR,        "CTOK_ERROR"},          // Signal from action
   {CTOK_NONE,         "CTOK_NONE"},
   {CTOK_END,          "CTOK_END"},
  };

TOKEN_INFO token_names[] =
  {
   {TOK_NONE, "TOK_NONE"},
   {TOK_NO_TOK,  "TOK_NO_TOK"},
   
   {TOK_KEY_LOAD_IAR,         "TOK_KEY_LOAD_IAR"},
   {TOK_KEY_LOAD_ADDR,        "TOK_KEY_LOAD_ADDR"},
   {TOK_KEY_LOAD_STORE,       "TOK_KEY_LOAD_STORE"},
   {TOK_KEY_LOAD_DEC_ADDR,    "TOK_KEY_LOAD_DEC_ADDR"},
   {TOK_KEY_LOAD_INCR_ADDR,   "TOK_KEY_LOAD_INCR_ADDR"},
   {TOK_KEY_LOAD_NORMAL_RESET,"TOK_KEY_LOAD_NORMAL_RESET"},
   {TOK_KEY_CLEAR,            "TOK_KEY_CLEAR"},
   {TOK_KEY_MINUS,            "TOK_KEY_MINUS"},
   {TOK_KEY_DOT,              "TOK_KEY_DOT"},
   {TOK_KEY_1,                "TOK_KEY_1"},
   {TOK_KEY_2,                "TOK_KEY_2"},
   {TOK_KEY_3,                "TOK_KEY_3"},
   {TOK_KEY_4,                "TOK_KEY_4"},
   {TOK_KEY_5,                "TOK_KEY_5"},
   {TOK_KEY_6,                "TOK_KEY_6"},
   {TOK_KEY_7,                "TOK_KEY_7"},
   {TOK_KEY_8,                "TOK_KEY_8"},
   {TOK_KEY_9,                "TOK_KEY_9"},
   {TOK_KEY_0,                "TOK_KEY_0"},
   {TOK_KEY_A,                "TOK_KEY_A"},
   {TOK_KEY_B,                "TOK_KEY_B"},
   {TOK_KEY_C,                "TOK_KEY_C"},
   {TOK_KEY_RUN,              "TOK_KEY_RUN"},
   {TOK_KEY_STOP,             "TOK_KEY_STOP"},
   {TOK_KEY_KI_RESET,         "TOK_KEY_KI_RESET"},
   {TOK_KEY_DUMP,             "TOK_KEY_DUMP"},
   {TOK_KEY_CHECK,            "TOK_KEY_CHECK"},
   {TOK_KEY_RELOAD,           "TOK_KEY_RELOAD"},

  };

char *name_of_token(TOKEN tok)
{
  for(int i=0; i<CTOK_NAMES_LEN; i++)
    {
      if( ctok_names[i].tok_value == tok )
	{
	  return((char *)ctok_names[i].tok_name);
	}
    }
  for(int i=0; i<TOKEN_NAMES_LEN; i++)
    {
      if( token_names[i].tok_value == tok )
	{
	  return((char *)token_names[i].tok_name);
	}
    }
   
  return((char *)"TOK???");
}

char *label_of_token(TOKEN tok)
{
  for(int i=0; i<TOKEN_NAMES_LEN; i++)
    {
      if( token_names[i].tok_value == tok )
	{
	  return((char *)token_names[i].tok_name);
	}
    }
   
  return((char *)"???");
}
