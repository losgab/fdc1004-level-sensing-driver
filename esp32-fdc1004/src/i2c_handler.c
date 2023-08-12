#include "i2c_handler.h"

int i2c_master_port = 0;

esp_err_t i2c_master_init(int i2c_master_channel)
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO, // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = I2C_MASTER_SCL_IO, // select GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, // select frequency specific to your project
    };
    i2c_param_config(i2c_master_channel, &config);
    i2c_driver_install(i2c_master_channel, config.mode, 0, 0, 0);
}

// Include a slave init function
esp_err_t write_i2c_byte(i2c_port_t i2c_port, uint8_t data)
{
    void *link = i2c_cmd_link_create();
    i2c_master_start(&link);

    i2c_master_write_byte(&link, data, true);
    
    i2c_master_write(&link, data, 1, true);

    i2c_master_stop(&link);

    i2c_master_cmd_begin(i2c_port, &link, 100);

    i2c_cmd_link_delete(&link);
}