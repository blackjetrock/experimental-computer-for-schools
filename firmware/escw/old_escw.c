#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

char ssid[] = "BML11";
char pass[] = "freddabearfre";

int main() {
  stdio_init_all();
  sleep_ms(2000);
  int e;

  printf("ESCW");
  if (e = cyw43_arch_init_with_country(CYW43_COUNTRY_UK))
    {
      printf("Failed to initialise: %d\n", e);
      return 1;
    }
  
  printf("initialised\n");
  cyw43_arch_enable_sta_mode();
  while (e = cyw43_arch_wifi_connect_timeout_ms(ssid, pass, CYW43_AUTH_WPA2_AES_PSK, 10000))
    {
      printf("failed to connect: %d\n", e);
      //return 1;
    }
  printf("connected\n");
}
