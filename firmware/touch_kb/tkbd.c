///////////////////////////////////////////////////////////////////////////////
//
// Experimental Schools Computer Simulator
//
////////////////////////////////////////////////////////////////////////////////
//
// Toouch Keyboard
//
// Runs on RP Pico
//
////////////////////////////////////////////////////////////////////////////////


#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/flash.h"
#include "hardware/structs/bus_ctrl.h"

#define DEBUG_STOP while(1) {}

// Some logic to analyse:
#include "hardware/structs/pwm.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

void set_gpio_output(int p)
{
  gpio_init(p);
  gpio_set_dir(p, GPIO_OUT);
}

void set_gpio_input(int p)
{
  gpio_init(p);
  gpio_set_dir(p, GPIO_IN);
}

int main(void)
{
  ////////////////////////////////////////////////////////////////////////////////
  //
  // Overclock as needed
  //
  ////////////////////////////////////////////////////////////////////////////////
  
#define OVERCLOCK 135000
  //#define OVERCLOCK 200000
  //#define OVERCLOCK 270000
  //#define OVERCLOCK 360000
  
#if OVERCLOCK > 270000
  /* Above this speed needs increased voltage */
  vreg_set_voltage(VREG_VOLTAGE_1_20);
  sleep_ms(1000);
#endif
  
  /* Overclock */
  set_sys_clock_khz( OVERCLOCK, 1 );

  stdio_init_all();

  sleep_ms(2000);

  printf("\n");
  printf("\n                                  ********************************************");
  printf("\n                                  * Experimental Schools Keyboard            *");
  printf("\n                                  ********************************************");
  printf("\n");


  ////////////////////////////////////////////////////////////////////////////////
  // Set up GPIOs
  
#define DRV 2
  
  gpio_init(DRV);
  gpio_set_dir(DRV, GPIO_OUT);

#if 0
  while(1)
    {
      gpio_put(DRV, 1);
      sleep_us(20);
      gpio_put(DRV, 0);
      sleep_us(5);
    }

#endif

////////////////////////////////////////////////////////////////////////////////
  
  // Induced voltage sensing
  
#if 0

  for(int j=10; j<12; j++)
    {
      gpio_init(j);
      gpio_set_dir(j, GPIO_IN);
      gpio_set_pulls (j, 0, 0);
    }
  
  int x[30];
  int g[30];

  for(int j=10; j<12; j++)
    {
      x[j] = 0;
      g[j] = 0;
    }
	
  while(1)
    {
      for(int j=10; j<12; j++)
	{
	  
	  g[j] = gpio_get(j);
	  
	  if( !g[j] )
	    {
	      if( x[j] > 0 )
		{
		  x[j]--;
		}
	    }
	  else
	    {
	      if( x[j] <100 )
		{
		  x[j]++;
		}
	      
	    }
	  

	}

      printf("\n");
      for(int j=10; j<12; j++)
	{
	  printf("%d    ", x[j]);
	}
    }
  
#endif


////////////////////////////////////////////////////////////////////////////////
//
// Shift register voltage sensing
//
////////////////////////////////////////////////////////////////////////////////

#if 1

////////////////////////////////////////////////////////////////////////////////
//
// Read an 8 bit value from a 165 latch
//
////////////////////////////////////////////////////////////////////////////////

#define PIN_SHLD    2
#define PIN_SCLKIN  3
#define PIN_SDAIN   4
#define PIN_SER     5
#define PIN_CLK_INH 6

uint8_t read_165(const uint latchpin)
{
  uint8_t value = 0;
  
  // Latch the data
  gpio_put(latchpin, 0);
  gpio_put(latchpin, 0);
  sleep_us(100);
  
  gpio_put(latchpin, 1);

  // Clock the data out of the latch
  for(int i=0; i<8; i++)
    {
      sleep_us(1);
      
      // Read data
      value <<= 1;
      if( gpio_get(PIN_SDAIN) )
	{
	  value |= 1;
	}

      sleep_us(1);
      gpio_put(PIN_SCLKIN, 0);
      sleep_us(1);
      gpio_put(PIN_SCLKIN, 1);


    }

#if XP_DEBUG  
  printxy_hex(0,3, value);
#endif
  return(value);
}

 set_gpio_output(PIN_SHLD);
 set_gpio_output(PIN_SCLKIN);
 set_gpio_input(PIN_SDAIN);
 set_gpio_input(PIN_SER);
 set_gpio_output(PIN_CLK_INH);



 
 int d = 0;
 gpio_put(PIN_CLK_INH, 0);
 
while(1)
  {
    d = read_165(PIN_SHLD);

    printf("\nD:%02X", d);
  }

#endif
}
