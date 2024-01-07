
#include "f_util.h"

#include "ff.h"
#include "ff_stdio.h"

#include "sd_card.h"

// We have SD clock defined as pin 5 here, but that is wired to SW3 so
// this doesn't make sense. It does seem to work, though.
// The actual clock pin is on GPIO18, which is CMD here, so I don't know what's
// going on

#define PICO_SD_CLK_PIN        5
#define PICO_SD_CMD_PIN       18
#define PICO_SD_DAT0_PIN      19
#define ENABLE_4_PIN           0

#define PAK_DIR                "/ESC"

typedef int boolean;

extern boolean sd_ok_flag;

void mount_sd(void);
void unmount_sd(void);


