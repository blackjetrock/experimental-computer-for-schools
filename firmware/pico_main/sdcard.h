
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

typedef int boolean;

extern boolean sd_ok_flag;
extern char sd_error[200];
extern int max_filenum;

#define FILE_LIST_DATA_LINES_MAX  8
#define FILE_LIST_DATA_LINES_LEN 20

extern char file_list_data[FILE_LIST_DATA_LINES_MAX][FILE_LIST_DATA_LINES_LEN];

void mount_sd(void);
void unmount_sd(void);
int cd_to_dir(char *to_dir);
int file_list(char *dir);
int find_next_file_number(char *dir, char *scan_fmt, char *print_fmt, char *glob);
int file_partial_list(char *dir, int first, int len);
void delete_file(char *dir, char *name);
