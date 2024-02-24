#ifndef I2C_FUNCTIONS_H_
#define I2C_FUNCTIONS_H_

// For the OLED display
#define SDA_PIN  14
#define SCL_PIN  15

typedef uint8_t BYTE;

// I2C Port descriptor
typedef struct _I2C_PORT_DESC
{
  unsigned char sdaport;
  unsigned char sdabit;
  unsigned char sclport;
  unsigned char sclbit;
} I2C_PORT_DESC;


// Slave device descriptor
typedef struct _I2C_SLAVE_DESC
{
  I2C_PORT_DESC *port;             // Port the device is on
  unsigned char slave_7bit_addr;        // SLave address
} I2C_SLAVE_DESC;

// I2C functions
void i2c_release(I2C_PORT_DESC *port);
void i2c_delay(I2C_PORT_DESC *port);
void i2c_sda_low(I2C_PORT_DESC *port);
void i2c_sda_high(I2C_PORT_DESC *port);
void i2c_scl_low(I2C_PORT_DESC *port);
void i2c_scl_high(I2C_PORT_DESC *port);
void i2c_start(I2C_PORT_DESC *port);
void i2c_stop(I2C_PORT_DESC *port);
int i2c_send_byte(I2C_PORT_DESC *port, BYTE b);
int i2c_read_bytes(I2C_SLAVE_DESC *slave, int n, BYTE *data);
void i2c_send_bytes(I2C_SLAVE_DESC *slave, int n, BYTE *data);
void i2c_init(I2C_PORT_DESC *port);
void oled_setup(I2C_SLAVE_DESC *slave);;
void oled_send_cmd(I2C_SLAVE_DESC *slave, int n, const unsigned char *data, int command, int repeat);
void oled_set_xy(I2C_SLAVE_DESC *slave, int x, int y);
void oled_set_pixel_xy(I2C_SLAVE_DESC *slave, int x, int y);
void oled_set_byte_xy(I2C_SLAVE_DESC *slave, int x, int y, int b);
void oled_gap(I2C_SLAVE_DESC *slave);
void oled_display_int(I2C_SLAVE_DESC *slave, long int n, int num_digits);
void oled_display_string(I2C_SLAVE_DESC *slave, char *string);
void oled_clear_display(I2C_SLAVE_DESC *slave);
void oled_printf(I2C_SLAVE_DESC *slave, char *format, ...);

void oled_setup(I2C_SLAVE_DESC *slave);
void oled_display_scaled_string_xy(I2C_SLAVE_DESC *slave, char *string, int x, int y, int scale);

I2C_SLAVE_DESC oled0;
I2C_PORT_DESC i2c_bus_0;

#endif /* I2C_FUNCTIONS_H_ */
