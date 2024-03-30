////////////////////////////////////////////////////////////////////////////////
//
// Seperate file to decouple FSM code from application
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>

#include "fsms.h"
#include "esc_fsms.h"
#include "esc.h"

////////////////////////////////////////////////////////////////////////////////
//
// Compares a token to a 'classification token'
// CTOKs can match more than one token, for example, CTOK_NUMERIC
// matches any numeric token, like digits or the TOK_DOT token.
//
////////////////////////////////////////////////////////////////////////////////


int match_token(TOKEN tok, int ctok)
{
  int ret = 0;
  
  if( (tok != TOK_NO_TOK) && (tok!=CTOK_NONE) )
    {
#if DUMP_TOK_MATCH      
      printf("\n%s  tok:%02X ctok:%02X", __FUNCTION__, tok, ctok);
#endif
    }
  
  switch(ctok)
    {
    case CTOK_DEFAULT:
      switch(ctok)
	{
	case TOK_NONE:
	case TOK_NO_TOK:
	  ret = 0;
	  break;
	default:
	  ret = 1;
	  break;
	}
      break;
      
    case CTOK_NUMERIC:
      switch(tok)
	{
	case TOK_KEY_0:
	case TOK_KEY_1:
	case TOK_KEY_2:
	case TOK_KEY_3:
	case TOK_KEY_4:
	case TOK_KEY_5:
	case TOK_KEY_6:
	case TOK_KEY_7:
	case TOK_KEY_8:
	case TOK_KEY_9:
	  ret = 1;
	  break;
	  
	default:
	  break;
	}
      break;

    default:
      if( tok == ctok )
	{
	  ret = 1;
	}
	  
      break;
    }

  if( (tok != TOK_NO_TOK) && (tok!=CTOK_NONE) )
    {
#if DUMP_TOK_MATCH      
      printf("  ret=%d", ret);
#endif
    }
  return(ret);
}
