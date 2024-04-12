////////////////////////////////////////////////////////////////////////////////
//
// Switches
//
////////////////////////////////////////////////////////////////////////////////

// All debug switches off or set up
#if 0

#define DEBUG_CALC_STATE       1
#define DEBUG_A_DECODE         1
#define DEBUG_LOOP             0
#define DEBUG_BCD_CORRECTION   0
#define DEBUG_SW_BCD_SUM       0
#define DEBUG_DW_BCD_SUM       0
#define DEBUG_REG_ASSIGN       0
#define DEBUG_FP               0

#define DEBUG_TEST             1    // Test instruction
#define DEBUG_TEST_SEQ         1    // Test sequences

#define DEBUG_SERIAL           0
#define DEBUG_KEY_SCAN         0
#define DEBUG_ADDR_MODES       1
#define DEBUG_INST_21          1
#define DEBUG_INST_22          1

#define DEBUG_RESTART          1
#define DEBUG_EXECUTE          1
#define DEBUG_FP_SUMMARY       1

#define DEBUG_DISPLAY          1
#else

#define DEBUG_CALC_STATE       0
#define DEBUG_A_DECODE         0
#define DEBUG_LOOP             0
#define DEBUG_BCD_CORRECTION   0
#define DEBUG_SW_BCD_SUM       0
#define DEBUG_DW_BCD_SUM       0
#define DEBUG_REG_ASSIGN       0
#define DEBUG_FP               0
#define DEBUG_FP_SUMMARY       1

#define DEBUG_TEST             0    // Test instruction
#define DEBUG_TEST_SEQ         0    // Test sequences

#define DEBUG_SERIAL           0
#define DEBUG_KEY_SCAN         0
#define DEBUG_ADDR_MODES       0
#define DEBUG_INST_21          0
#define DEBUG_INST_22          0

#define DEBUG_DISPLAY          0

#define DEBUG_RESTART          0
#define DEBUG_EXECUTE          0
#endif


#define OLED_ON                1
#define SD_ON                  1
#define ERRORS_ON              0
#define WARNINGS_ON            0


