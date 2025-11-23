////////////////////////////////////////////////////////////////////////////////
//
// Switches
//
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
//
// Execution flow debug
//
#define DEBUG_ENTRY           1

#if DEBUG_ENTRY
#define FN_ENTRY         printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%s\n", __FUNCTION__)
#define FN_ENTRY_DISPLAY printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%s display:%d\n", __FUNCTION__, display)
#define FN_EXIT          printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<%s\n", __FUNCTION__)
#else
#define FN_ENTRY
#define FN_EXIT
#endif


//------------------------------------------------------------------------------
//
// All debug switches off or set up
#if 0

#define DEBUG_STAGES           1
#define DEBUG_EXTRACODE        0
#define DEBUG_CALC_STATE       0
#define DEBUG_A_DECODE         0
#define DEBUG_LOOP             0
#define DEBUG_BCD_CORRECTION   0
#define DEBUG_SW_BCD_SUM       0
#define DEBUG_DW_BCD_SUM       0
#define DEBUG_REG_ASSIGN       0
#define DEBUG_FP               0
#define DEBUG_FP_SUMMARY       0
#define DEBUG_MSD_SHIFT        0
#define DEBUG_SW_SHIFT         0
#define DEBUG_BCD_TO_BINARY    0

#define DEBUG_TEST             0    // Test instruction
#define DEBUG_TEST_SEQ         0    // Test sequences

#define DEBUG_SERIAL           0
#define DEBUG_KEY_SCAN         0
#define DEBUG_ADDR_MODES       0
#define DEBUG_INST_21          0
#define DEBUG_INST_22          0

#define DEBUG_STOP             0
#define DEBUG_RESTART          0
#define DEBUG_EXECUTE          0
#define DEBUG_PREPARE          0
#define DEBUG_DISPLAY          0
#define DEBUG_DISPLAY_ON_LINE  0
#define DEBUG_BRANCH           0

#define DUMP_STATE_STAGE_C     0
#define DEBUG_REGISTER_ASSIGN  0

#define DEBUG_SUPPRESSED       0
#endif

// All debug switches off or set up
#if 1

#define DEBUG_STAGES           1
#define DEBUG_CALC_STATE       1
#define DEBUG_A_DECODE         1
#define DEBUG_LOOP             0
#define DEBUG_BCD_CORRECTION   1
#define DEBUG_SW_BCD_SUM       1
#define DEBUG_DW_BCD_SUM       1
#define DEBUG_REG_ASSIGN       1
#define DEBUG_FP               1
#define DEBUG_FP_SUMMARY       1
#define DEBUG_MSD_SHIFT        1
#define DEBUG_SW_SHIFT         1
#define DEBUG_BCD_TO_BINARY    1

#define DEBUG_TEST             1    // Test instruction
#define DEBUG_TEST_SEQ         1    // Test sequences

#define DEBUG_SERIAL           1
#define DEBUG_KEY_SCAN         1
#define DEBUG_ADDR_MODES       1
#define DEBUG_INST_21          1
#define DEBUG_INST_22          1

#define DEBUG_RESTART          1
#define DEBUG_EXECUTE          0
#define DEBUG_PREPARE          1

#define DEBUG_DISPLAY          1
#define DEBUG_DISPLAY_ON_LINE  1
#define DEBUG_BRANCH           1

#define DUMP_STATE_STAGE_C     1
#define DEBUG_REGISTER_ASSIGN  1
#define DEBUG_DISPLAY_IAR      1
#define DEBUG_FN_CALL          1
#define DEBUG_KEYS             1
#define DEBUG_NEXT_IAR         1
#define DEBUG_ENTER_EXTRACODE  1
#define DEBUG_LOAD_FROM_STORE  1

#endif

//------------------------------------------------------------------------------

#if 0

#define DEBUG_CALC_STATE       0
#define DEBUG_A_DECODE         0
#define DEBUG_LOOP             0
#define DEBUG_BCD_CORRECTION   0
#define DEBUG_SW_BCD_SUM       0
#define DEBUG_DW_BCD_SUM       0
#define DEBUG_REG_ASSIGN       0
#define DEBUG_FP               1
#define DEBUG_FP_SUMMARY       1
#define DEBUG_MSD_SHIFT        1
#define DEBUG_SW_SHIFT         1
#define DEBUG_BCD_TO_BINARY    1

#define DEBUG_TEST             0    // Test instruction
#define DEBUG_TEST_SEQ         0    // Test sequences

#define DEBUG_SERIAL           0
#define DEBUG_KEY_SCAN         0
#define DEBUG_ADDR_MODES       0
#define DEBUG_INST_21          0
#define DEBUG_INST_22          0

#define DEBUG_DISPLAY          0
#define DEBUG_DISPLAY_ON_LINE  0

#define DEBUG_RESTART          0
#define DEBUG_EXECUTE          0
#define DEBUG_PREPARE          1
#define DEBUG_BRANCH           1

#define DUMP_STATE_STAGE_C     1
#define DEBUG_REGISTER_ASSIGN  1
#endif

////////////////////////////////////////////////////////////////////////////////
//
// Main types of firmware
//
// SMALL:
// This firmware for a small PCB that has a 0.96" OLED display, a keyboard
// made form tactile switches and an SD catrd slot. There are some headers
// for unused GPIOs.
//
// DESKTOP:
// This is a PCB which has a Pico, a voltage regulator and circuitry to
// drive a capacitive touch keyboard and a large LCD screen. There is no
// keyboard or display on this PCB.

#define ESC_TYPE_SMALL         1
#define ESC_TYPE_DESKTOP       0

#define ESC_KBD_VOLT_TOUCH     0
#define ESC_KBD_QT             1

// Implement the extracode framework as instruction sin 100..199
// Otherwise coded instructions.
#define EXTRACODE_FRAMEWORK    1

// Suppressed output has no leading zeros and no + sign.
// This is closer to the original machine.

#define SUPPRESSED_OUTPUT      1
#define OLED_ON                1
#define SD_ON                  1
#define ERRORS_ON              0
#define WARNINGS_ON            0


#define PIXEL_SCALE  3
#define PIXEL_SCALE2 2

