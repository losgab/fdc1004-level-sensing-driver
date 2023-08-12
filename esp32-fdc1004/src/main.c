#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c_handler.h"

#define I2C_MASTER_SCL_PIN GPIO_NUM_22
#define I2C_MASTER_SDA_PIN GPIO_NUM_21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency

void app_main()
{
    i2c_master_init(I2C_MASTER_NUM);

    while (1)
    {
        // update the sensor
        // get readings from internally stored stuff
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1000 milliseconds
    }
}
