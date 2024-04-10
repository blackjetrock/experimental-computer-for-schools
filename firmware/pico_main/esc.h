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


#define EMPTY_REGISTER         0xA0000000

// Address of +0
#define EMPTY_ADDRESS          0xA0000000
#define NO_VALUE               -1

#define DISPLAY_UPDATE         1
#define DISPLAY_NO_UPDATE      0
#define MAX_LINE               16
#define NUM_LINES              6
#define MAX_FILE_LINE          200

#define WORD_SIGN_PLUS         0xA
#define WORD_SIGN_MINUS        0xB
#define WORD_SIGN_NONE         0xF

typedef uint32_t SINGLE_WORD;
typedef uint64_t DOUBLE_WORD;
typedef uint32_t REGISTER_SINGLE_WORD;
typedef uint64_t REGISTER_DOUBLE_WORD;
typedef int BOOLEAN;
typedef uint32_t ADDRESS;

// Bound an address, force to positive
#define BOUND_ADDRESS(XXX) SW_PLUS((XXX&0x00FFFFFF)>0x199?0x199:XXX)

// Get the single word sign
#define SW_SIGN(XX)            ((XX & 0xF0000000)>>28)
#define SW_DIGITS(XX)          ((XX & 0x00FFFFFF)>>0 )

#define STORE_GET_EXPONENT(XX)     ((XX & 0x0F000000)>>24)
#define STORE_GET_DIGITS(XX)       ((XX & 0x00FFFFFF)>>0 )
#define STORE_GET_SIGN(XX)         ((XX & 0xF0000000)>>28)
#define STORE_GET_LH4_DIGITS(XX)   ((XX & 0xFFFF0000)>>16)
#define STORE_GET_RH4_DIGITS(XX)   ((XX & 0x0000FFFF)>>0 )

#define STORE_SET_EXPONENT(XX,EE)   ((XX & 0xF0FFFFFF) | ((EE & 0x0000000F) <<24))
#define STORE_SET_DIGITS(XX,EE)     ((XX & 0xFF000000) | ((EE & 0x00FFFFFF) <<0 ))
#define STORE_SET_SIGN(XX,EE)       ((XX & 0x0FFFFFFF) | ((EE & 0x0000000F) <<28))
#define STORE_SET_LH4_DIGITS(XX,EE) ((XX & 0x0000FFFF) | ((EE & 0x0000FFFF) <<16))
#define STORE_SET_RH4_DIGITS(XX,EE) ((XX & 0xFFFF0000) | ((EE & 0x0000FFFF) <<0 ))

// Get the double word sign
#define DW_SIGN(XX)            ((XX & 0xF000000000000000L)>>60)
#define DW_DIGITS(XX)          ((XX & 0x0000FFFFFFFFFFFFL)>> 0)

// Remove signs from values
#define REMOVED_SW_SIGN(XX)    (XX & 0x0FFFFFFF)
#define REMOVED_DW_SIGN(XX)    (XX & 0x0FFFFFFFFFFFFFFFL)
#define REMOVED_SW_UNUSED(XX)  (XX & 0xF0FFFFFF)
#define REMOVED_DW_UNUSED(XX)  (XX & 0xF000FFFFFFFFFFFFL)

#define OVERFLOW_SW(XX)        ((XX & 0x0F000000) != 0 )
#define OVERFLOW_DW(XX)        ((XX & 0x000F000000000000L) != 0 )

#define CLEAR_SW_CARRY(XX)     (XX & 0xF0FFFFFF)
#define CLEAR_DW_CARRY(XX)     (XX & 0xF000FFFFFFFFFFFFL)
#define SET_SW_SIGN(XX, SGN)   (REMOVED_SW_SIGN(XX) | (SGN << 28))
#define SET_DW_SIGN(XX, SGN)   (REMOVED_DW_SIGN(XX) | ((DOUBLE_WORD)SGN <<60))
#define SW_PLUS(XX)            SET_SW_SIGN(XX, WORD_SIGN_PLUS)
#define SW_MINUS(XX)           SET_SW_SIGN(XX, WORD_SIGN_MINUS)
#define DW_PLUS(XX)            SET_DW_SIGN(XX, WORD_SIGN_PLUS)
#define DW_MINUS(XX)           SET_DW_SIGN(XX, WORD_SIGN_MINUS)

// Convert SW to DW
#define SW_TO_DW(XX)         ((SW_DIGITS(XX)             ) |  SET_DW_SIGN(SW_DIGITS(XX),SW_SIGN(XX)))
#define DW_TO_SW(XX)         ((DW_DIGITS(XX) & 0x00FFFFFF) | (SET_SW_SIGN(DW_DIGITS(XX),DW_SIGN(XX)) & 0xF0FFFFFF))
  
// Instruction fields
#define INST_A_FIELD(INST)   ((INST & 0xF0000000)>>28)
#define INST_B_FIELD(INST)   ((INST & 0x0F000000)>>24)
#define INST_C_FIELD(INST)   ((INST & 0x00F00000)>>20)
#define INST_D_FIELD(INST)   ((INST & 0x000F0000)>>16)
#define INST_E_FIELD(INST)   ((INST & 0x0000F000)>>12)
#define INST_F_FIELD(INST)   ((INST & 0x00000F00)>> 8)
#define INST_G_FIELD(INST)   ((INST & 0x000000F0)>> 4)
#define INST_H_FIELD(INST)   ((INST & 0x0000000F)>> 0)
#define INST_3_ADDR_1(INST)  ((INST & 0x00FF0000)>>16)
#define INST_3_ADDR_2(INST)  ((INST & 0x0000FF00)>> 8)
#define INST_3_ADDR_3(INST)  ((INST & 0x000000FF)>> 0)
#define INST_1_ADDR_AP(INST) ((INST & 0x000000FF)>> 0)

#define R0 R[0]
#define R1 R[1]
#define R2 R[2]
#define R3 R[3]
#define R4 R[4]
#define R5 R[5]
#define R6 R[6]
#define R7 R[7]
#define R8 RD[0]
#define R9 RD[1]

#define SW_REG_CONTENTS(RR) (s->R[RR])
#define DW_REG_CONTENTS(RR) (s->RD[RR-8])
  
#define IS_SW_REGISTER(REGNO) ((REGNO >= 0) && (REGNO < NUM_WORD_REGISTERS))
#define IS_DW_REGISTER(REGNO) ((REGNO >= NUM_WORD_REGISTERS) && (REGNO < NUM_WORD_REGISTERS+NUM_DBL_WORD_REGISTERS))
  
// IAR type registers have an implicit flag that addresses
// one of the two 4 digit instructions in a register

typedef struct _IAR
{
  ADDRESS address;
  BOOLEAN a_flag;
} IAR;

typedef struct _ESC_STATE
{
  // Main store
  SINGLE_WORD store[STORE_SIZE];

  REGISTER_SINGLE_WORD  R[NUM_WORD_REGISTERS];
  REGISTER_DOUBLE_WORD RD[NUM_DBL_WORD_REGISTERS];
  
  DOUBLE_WORD    keyboard_register;
  ADDRESS address_register0;
  ADDRESS address_register1;
  ADDRESS address_register2;

  SINGLE_WORD    instruction_register;

  IAR iar;
  IAR aux_iar;
  
  ADDRESS link_register;
  
  BOOLEAN control_latch;
  BOOLEAN ki_reset_flag;

  // Instruction processing
  int inst_digit_a;
  int inst_digit_b;
  int inst_digit_c;
  int inst_digit_d;
  int inst_digit_e;
  int inst_digit_f;
  int inst_digit_g;
  int inst_digit_h;

  int reginst_rc;
  int reginst_rd;
  int reginst_literal;
  int inst_aa;
  int inst_ap;
  
  ADDRESS Ap1, Ap2, Ap3, Aa1, Aa2, Aa3;

  // execution
  int run;
  int stop;
  
  //--------------------------------------------------------------------
  // We can push a token into the FSM
  TOKEN insert_token;
  int update_display;
  int reload_display;
  int delete_display;
  
  char stage;

  //---------------------------------------------------------------------
  int reload_file_first;

} ESC_STATE;


extern ESC_STATE esc_state;

////////////////////////////////////////////////////////////////////////////////

char *display_register_double_word(REGISTER_DOUBLE_WORD x);
char *display_register_single_word(REGISTER_SINGLE_WORD x);
char *display_iar(IAR iar);
char *display_address(REGISTER_SINGLE_WORD x);
char *get_string_state(void);
void drive_fsms(void);
void serial_loop();
void update_display(void);
char *get_display(void);
int wifi_main(void);

////////////////////////////////////////////////////////////////////////////////

#define ESC_DIR "/ESC"

typedef int (*FIELD_WORK_FN)(ESC_STATE *es, void *fi, char *line);

typedef struct _FIELD_INFO
{
  char *name;
  FIELD_WORK_FN fn;
} FIELD_INFO;

#define STORE_DATA_FILE_CHUNK_SIZE 8

