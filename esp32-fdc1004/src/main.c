#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "i2c_handler.h"

#define I2C_MASTER_SCL_PIN GPIO_NUM_22
#define I2C_MASTER_SDA_PIN GPIO_NUM_21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency

#define SYS_DELAY(x) vTaskDelay(pdMS_TO_TICKS(x))

void app_main()
{
    pin_setup();
    i2c_master_init(I2C_MASTER_NUM);

    while (1)
    {
        // update the sensor
        // get readings from internally stored stuff
        SYS_DELAY(1000);
    }
}

void pin_setup()
{
    gpio_set_direction(GPIO_NUM_0, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_pull_mode(GPIO_NUM_0, GPIO_PULLUP_ONLY);
}