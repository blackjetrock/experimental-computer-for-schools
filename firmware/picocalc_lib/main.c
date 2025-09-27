/**
 * PicoCalc Hello World
 * https://www.clockworkpi.com/
 */
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "i2ckbd.h"
#include "lcdspi.h"

const uint LEDPIN = 25;

int main() {

    stdio_init_all();

    init_i2c_kbd();
    lcd_init();

    gpio_init(LEDPIN);
    gpio_set_dir(LEDPIN, GPIO_OUT);

    lcd_clear();
    lcd_print_string("Hello World");
    gpio_put(LEDPIN, 1);
    sleep_ms(500);
    gpio_put(LEDPIN, 0);
    while (1) {
        int c = lcd_getc(0);
        if(c != -1 && c > 0) {
            lcd_putc(0,c);
        }
        sleep_ms(10);
    }
}

