#include "i2c_handler.h"

int i2c_master_port = 0;

esp_err_t i2c_master_init(void)
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO, // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_io_num = I2C_MASTER_SCL_IO, // select GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, // select frequency specific to your project
    };
    // esp_err_t = i2c_param_config();
}