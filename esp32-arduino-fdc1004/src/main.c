#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_PIN  GPIO_NUM_22
#define I2C_MASTER_SDA_PIN  GPIO_NUM_21
#define I2C_MASTER_NUM      I2C_NUM_0
#define I2C_MASTER_FREQ_HZ  100000   // I2C master clock frequency

void i2c_master_init() {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_PIN;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
}

void app_main() {
    i2c_master_init();
    
    // Your application code here
    
    while (1) {
        // Your main loop code here
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000 milliseconds
    }
}
