#include "switches.h"

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdcard.h"

boolean sd_ok_flag = false;
char sd_error[200];

#if 0
void oled_error(char *msg)
{
  printf("\n%s", msg);
}
#endif

// Mount the SD card.

FATFS p_fs;

void mount_sd(void)
{
  //  p_fs = get_fs_by_name("0:");

  sd_ok_flag = false;

#if 0
  if (!p_fs)
    {
      oled_error("Mount:Unknown drive:'0:'");
      return;
    }
#endif
  
  FRESULT fr = f_mount(&p_fs, "0:", 1);
  
  if (FR_OK != fr)
    {
      sprintf(sd_error, "Mount error");
      return;
    }

  sd_ok_flag = true;
}

void unmount_sd(void)
{
  f_unmount("0:");
}

#if 1
int file_list(char *dir)
{
  int file_n = 0;
  int num_listfiles = 0;
  int i;
  
  char cwdbuf[FF_LFN_BUF] = {0};
  FRESULT fr;
  char const *p_dir;

  mount_sd();
  
  f_chdrive("0:");

  cd_to_dir(dir);
  
  fr = f_getcwd(cwdbuf, sizeof cwdbuf);

  if (FR_OK != fr)
    {
      sprintf(sd_error, "f_getcwd error: %s (%d)\n", FRESULT_str(fr), fr);
      unmount_sd();
      return(0);
    }
  
  p_dir = cwdbuf;

  printf("\nDirectory Listing: %s\n", p_dir);
  
  DIR dj;      /* Directory object */
  FILINFO fno; /* File information */

  memset(&dj, 0, sizeof dj);
  memset(&fno, 0, sizeof fno);

  fr = f_findfirst(&dj, &fno, p_dir, "*");

  if (FR_OK != fr)
    {
      sprintf(sd_error, "f_findfirst error: %s (%d)\n", FRESULT_str(fr), fr);
      unmount_sd();
      return(0);
    }
  
  while( (fr == FR_OK) && fno.fname[0]  )
    { 
      if (fno.fattrib & AM_DIR)
	{
	  // Directory, we gnore these
	}
      else
	{
	  char extension[40];
	  char name[80];
	   
	  // If the file has an extension of .esc then display it
	  // otherwise ignore.
	  extension[0] = '\0';

	  printf("\n%s", fno.fname);
	  
	  if( sscanf(fno.fname, "%[^.].%s", name, extension) )
	    {
	      if( strcmp(extension, "esc") == 0 )
		{
		  printf(" ESC");
		}
	    }
	}
       
      fr = f_findnext(&dj, &fno); /* Search for next item */
    }
  f_closedir(&dj);

  printf("\n");
  
  unmount_sd();
  return(1);
}
#endif

// Change to directory
// 0 on error, 1 on success

int cd_to_dir(char *to_dir)
{
  char line[40];
  FRESULT fr;
  char cwdbuf[FF_LFN_BUF] = {0};
  char const *p_dir;
  
  f_chdrive("0:");
  
  // Are we in the pak directory already?
  fr = f_getcwd(cwdbuf, sizeof cwdbuf);
  
  if (FR_OK != fr)
    {
      sprintf(sd_error, "getcwd error %d", fr);
      return(0);
    }
  
  if( strcmp(cwdbuf, to_dir) != 0 )
    {
      // Change to the PAK directory
      fr = f_chdir(to_dir);
      
      if (FR_OK != fr)
	{
	  sprintf(sd_error, "Error opening directory %d", fr);
	  return(0);
	}
    }
  
  return(1);
}


////////////////////////////////////////////////////////////////////////////////
//
// Read and process the config file
//
////////////////////////////////////////////////////////////////////////////////
#if 0

void process_config_file(I2C_SLAVE_DESC *slave)
{
  char line[40];
  char fileline[80];

  //DEBUG_STOP;
  
  mount_sd();
  
  if( cd_to_pak_dir(slave) )
    {
      unmount_sd();
      return;
    }  mount_sd();
  
  if( cd_to_pak_dir(slave) )
    {
      unmount_sd();
      return;
    }

  
  oled_clear_display(slave);
  oled_set_xy(slave, 0, 0);
  sprintf(line, "Processing");
  oled_display_string(slave, line);
      
  oled_set_xy(slave, 0, 8);
  sprintf(line, "config.txt");
  oled_display_string(slave, line);
  
  loop_delay(1000000);

  // Read the file from the SD card into the pak memory
  FF_FILE *fp = ff_fopen("config.txt", "r");

  if (fp == NULL)
    {
      sprintf(line, "Failed to open:");
      oled_clear_display(slave);
      oled_set_xy(slave, 0, 0);
      oled_display_string(slave, line);
      
      oled_set_xy(slave, 0, 7);
      sprintf(line, "config.txt");
      oled_display_string(slave, line);

      loop_delay(3000000);
      unmount_sd();
      return;
    }



  
  // Get lines from the file
  while( ff_fgets(&(fileline[0]), sizeof(fileline)-1, fp) != NULL )
    {
      char keyword[80];
      char name[80];

      // Remove trailing newline
      fileline[strlen(fileline)-1] = '\0';
      
      keyword[0] = '\0';
      name[0] = '\0';

      oled_clear_display(slave);
      oled_set_xy(slave, 0, 0);
      sprintf(line, "[%s]", fileline);
      oled_display_string(slave, line);
      loop_delay(3000000);
      
      sscanf(fileline, "%[^=]=%s", keyword, name);

      if( strcmp(keyword, "startfile") == 0 )
	{
	  strcpy(current_file, name);
	  
	  // read the file
	  core_read(&oled0, current_file);
	}
      
      if( strcmp(keyword, "brightness") == 0 )
	{
	  sscanf(name, "%d", &brightness_percent);
	  oled_set_brightness(&oled0, brightness_percent);
	  
	  oled_clear_display(slave);
	  oled_set_xy(slave, 0, 0);
	  sprintf(line, "Brightness: %d%%", brightness_percent);
	  oled_display_string(slave, line);
	  loop_delay(3000000);
	}
      
    }
  
  ff_fclose(fp);
  unmount_sd();
}
#endif

#if 0
// Read the file with the given name into the buffer
// Pak images are in a directory called PAK

void core_read(I2C_SLAVE_DESC *slave, char * arg)
{
  char line[40];

  //DEBUG_STOP;

  mount_sd();
  
  if( cd_to_pak_dir(slave) )
    {
      unmount_sd();
      return;
    }
  
  oled_clear_display(slave);
  oled_set_xy(slave, 0, 0);
  sprintf(line, "Reading file");
  oled_display_string(slave, line);
      
  oled_set_xy(slave, 0, 8);
  sprintf(line, "%s", arg);
  oled_display_string(slave, line);
  
  loop_delay(1000000);

  // Read the file from the SD card into the pak memory
  FIL fil;
  FRESULT fr = f_open(&fil, arg, FA_READ);

  if (FR_OK != fr)
    {
      sprintf(line, "Failed to open:");
      oled_clear_display(slave);
      oled_set_xy(slave, 0, 0);
      oled_display_string(slave, line);
      
      oled_set_xy(slave, 0, 7);
      sprintf(line, "%s", arg);
      oled_display_string(slave, line);

      loop_delay(3000000);
      unmount_sd();
      return;
    }
  
  char buf[1];
  int pak_i = 0;
  int br = 0;
  boolean done = false;
  char name[80];
  char extension[20];
  boolean modify_header = false;
  
  // If the extension is .opk then we drop the first 6 bytes
  if( sscanf(arg, "%[^.].%s", name, extension) == 2 )
    {
      if( strcmp(extension, "opk") == 0 )
	{
	  // Drop 6 bytes from start of file
	  for(int i=0; i<6; i++)
	    {
	      f_read(&fil, buf, sizeof buf, &br);
	    }

	  // We need to modify the header, so set flag telling us to do
	  // that later
	  modify_header = true;
	}
    }
  
  while( !done )
    {
      f_read(&fil, buf, sizeof buf, &br);
      
      if( br == 0 )
	{
	  done = true;
	  continue;
	}
      
      for(int i=0; i<br; i++)
	{
	  pak_memory[pak_i++] = buf[i];
	}
    }
  f_close(&fil);
  
  oled_set_xy(slave, 0, 16);
  sprintf(line, "%d bytes read", pak_i);
  oled_display_string(slave, line);

  // Do we modify the header?

  if( modify_header )
    {
      uint16_t csum = 0;
      
      // For reference, this arrangement works as 32K pack
      //  pak_memory[0] = 0x78;
      //  pak_memory[1] = 0x04; //32K, could allow original length through
      //  pak_memory[2] = 0x56;
      //  pak_memory[3] = 0x00;
      //  pak_memory[4] = 0x03;
      //  pak_memory[5] = 0x02;
      //  pak_memory[6] = 0x35;
      //  pak_memory[7] = 0x46;
      //  pak_memory[8] = 0x06;
      //  pak_memory[9] = 0x4c;

      // Pass flags through as we can now handle paged packs
#if 0
      pak_memory[0] = 0x68;
#endif

      // We can allow sizes up to and including 128K
      if( pak_memory[1] > 0x10 )
	{
	  pak_memory[1] = 0x10; //128K
	}
      
      // Pass next bytes on unaltered
#if 0      
      pak_memory[2] = 0x56;
      pak_memory[3] = 0x00;
      pak_memory[4] = 0x03;
      pak_memory[5] = 0x02;
      pak_memory[6] = 0x35;
      pak_memory[7] = 0x46;
#endif
      
      // Calculate correct checksum
      // The next two bytes are a cheksum of the first four WORDS
      for(int i = 0; i<8; i+=2)
	{
	  csum += pak_memory[i]*256+pak_memory[i+1];
	}
      pak_memory[8] = csum >> 8;
      pak_memory[9] = csum & 0xFF;
      
    }
  
  loop_delay(3000000);
  unmount_sd();
}

////////////////////////////////////////////////////////////////////////////////
// Writes the buffer to a file.
// Deletes any file that exists with the same name so that the resulting
// file is the same size as the buffer
// Searches the card directory for the highest file number and uses the next one
//

void core_writefile(boolean oled_nserial)
{
  char filename[20];
  int i;
  FRESULT fr;
  FIL fp;
  int bytes_written = 0;
  char line[40];

  //DEBUG_STOP

  mount_sd();
  
  if( cd_to_pak_dir(&oled0) )
    {
      unmount_sd();
      return;
    }

  find_next_file_number();
  sprintf(filename, PAK_FILE_NAME_FORMAT, ++max_filenum);
  
  oled_clear_display(&oled0);
  oled_set_xy(&oled0, 0, 0);
  oled_display_string(&oled0, "Writing");
  oled_set_xy(&oled0, 0, 8);
  oled_display_string(&oled0, filename);
  
  // Open file for writing
  fr = f_open(&fp, filename, FA_CREATE_NEW | FA_WRITE);

  if (FR_OK != fr && FR_EXIST != fr)
    {
      oled_error("Open error");
      unmount_sd();
      return;
    }

  // As we are writing an OPK file, we add a header in the first 6 bytes. We don't
  BYTE hdr[6] = {'O', 'P', 'K', 0, 0, 0};

  f_write (&fp, &(hdr[0]), 6, &bytes_written);
  
  // Write the entire buffer in one go
  f_write (&fp, &(pak_memory[0]), PAK_MEMORY_SIZE, &bytes_written);

  bytes_written += 6;
  
  fr = f_close(&fp);
  if (FR_OK != fr)
    {
      printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
      unmount_sd();
      return;
    }

  oled_set_xy(&oled0, 0, 3*8);
  sprintf(line, "%d bytes", bytes_written);
  oled_display_string(&oled0, line);

  unmount_sd();

}

// The switch menu/OLED display system
void to_back_menu(struct MENU_ELEMENT *e)
{
  menu_selection = 0;
  menu_offset = 0;
  current_menu = last_menu;
  draw_menu(&oled0, current_menu, true);
}

void to_home_menu(struct MENU_ELEMENT *e)
{
  menu_selection = 0;
  menu_offset = 0;
  current_menu = the_home_menu;
  draw_menu(&oled0, current_menu, true);
}
void button_read(struct MENU_ELEMENT *e)
{
  // Find a filename that hasn't been used
  char filename[20];
  int file_index = 0;
  
  mount_sd();
  
  if( cd_to_pak_dir(&oled0) )
    {
      unmount_sd();
      return;
    }

  for(;;)
    {
      sprintf(filename, "PAK%04d.OPK", file_index);
      
      FF_FILE *fp = ff_fopen(filename, "r");
      
      if (fp == NULL)
	{
	  // We have a filename we can use
	  break;
	}
      else
	{
	  // Try the next index number
	  file_index++;
	  ff_fclose(fp);

	}
    }

  unmount_sd();
  
  // We have a filename now, use it
  read_opk_file(&oled0, filename);
  
  draw_menu(&oled0, current_menu, true);
}
#endif
