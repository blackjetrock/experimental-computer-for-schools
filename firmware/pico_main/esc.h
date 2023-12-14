////////////////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// The data format used is an integer coded format
//
// registers
// A word is 8 decimal digits so can be coded in 32 bits
// A double word holds 12 decimal digits so will fit in a uint64
//
// Store
// The store uses words of 8 decimal digits. This fits in 32 bits
//
////////////////////////////////////////////////////////////////////////////////

#define NUM_WORD_REGISTERS     8
#define NUM_DBL_WORD_REGISTERS 2
#define STORE_SIZE             200

typedef uint32_t WORD;
typedef uint64_t DOUBLE_WORD;
typedef uint32_t REGISTER_SINGLE_WORD;
typedef uint64_t REGISTER_DOUBLE_WORD;
typedef int BOOLEAN;
typedef int ADDRESS;



#define R0 (R[0])
#define R1 (R[1])
#define R2 (R[2])
#define R3 (R[3])
#define R4 (R[4])
#define R5 (R[5])
#define R6 (R[6])
#define R7 (R[7])
#define R8 (RD[0])
#define R9 (RD[1])



typedef struct _ESC_STATE
{
  // Main store
  WORD store[STORE_SIZE];

  REGISTER_SINGLE_WORD  R[NUM_WORD_REGISTERS];
  REGISTER_DOUBLE_WORD RD[NUM_DBL_WORD_REGISTERS];
  
  WORD    keyboard_register;
  ADDRESS address_register0;
  ADDRESS address_register1;
  ADDRESS address_register2;
  
  ADDRESS iar;
  ADDRESS aux_iar;
  ADDRESS link_register;
  
  BOOLEAN control_latch;

  //--------------------------------------------------------------------
  // We can push a token into the FSM
  TOKEN insert_token;
  int update_display;
  
} ESC_STATE;

////////////////////////////////////////////////////////////////////////////////
