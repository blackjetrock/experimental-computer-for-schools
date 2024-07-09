#define uchar unsigned char  
#define uint  unsigned int 


void esc_desktop_display_gpio_init(void);
void escdd_display_start(void);
void cli_escdd(void);
void escdd_text(int x, int y, char *t, int n);
void lcd_init(void);
void lcd_cls(uchar part);
void show_char(unsigned char x, unsigned char y, unsigned char *string);
