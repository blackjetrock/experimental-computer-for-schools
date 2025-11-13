typedef enum
  {
    EXTRACODE_STANDARD = 1,
    EXTRACODE_BASIC,
  } EXTRACODE_ID;

typedef struct
{
  EXTRACODE_ID id;
  char         *desc;
  uint32_t     csum;
  int          *extracode_data;
} EXTRACODE_INFO;

#define NUM_EXTRACODE_IDS 2

extern EXTRACODE_INFO extracode_info[NUM_EXTRACODE_IDS];



void load_extracode_by_id(ESC_STATE *s, EXTRACODE_ID extracode_id);
