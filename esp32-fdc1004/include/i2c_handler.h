#include <driver/i2c.h>
#include <driver/gpio.h>

#define I2C_MASTER_FREQ_HZ  100000

#define IS_I2C_PORT(x) (I2C_NUM_0 <= x && x <= I2C_NUM_MAX)

/*
    Configures & Installs I2C driver for specified pins
*/
esp_err_t i2c_master_init(i2c_port_t i2c_master_channel, gpio_num_t sda_pin, gpio_num_t scl_pin);

/*
    Send a byte on the I2C line
*/
esp_err_t write_byte(i2c_port_t i2c_port_num, uint8_t byte);

/*
    Send n bytes data to an address
*/
esp_err_t write_n_bytes(i2c_port_t i2c_port_num, uint8_t *data, uint8_t data_len);