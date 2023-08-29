#include "i2c_handler.h"

int i2c_master_port = 0;

esp_err_t i2c_master_init(i2c_port_t i2c_master_channel, gpio_num_t sda_pin, gpio_num_t scl_pin)
{
    // Checks for valid GPIO
    if (!GPIO_IS_VALID_GPIO(sda_pin) || !GPIO_IS_VALID_GPIO(scl_pin))
        return ESP_ERR_INVALID_ARG;

    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = scl_pin,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, // select frequency specific to your project
    };
    i2c_param_config(i2c_master_channel, &config);
    i2c_driver_install(i2c_master_channel, config.mode, 0, 0, 0);
    return ESP_OK;
}

esp_err_t write_n_bytes(i2c_port_t i2c_port_num, uint8_t target_address, void *data, uint8_t data_len)
{
    void *link = i2c_cmd_link_create();
    i2c_master_start(&link);

    // Sends slave address with write command
    i2c_master_write_byte(&link, (target_address << 1) | I2C_MASTER_WRITE, true);

    // Sends data of specificed length
    i2c_master_write(&link, (uint8_t *)data, data_len, true);

    i2c_master_stop(&link);
    i2c_master_cmd_begin(i2c_port_num, &link, 100);
    i2c_cmd_link_delete(&link);
    return ESP_OK;
}