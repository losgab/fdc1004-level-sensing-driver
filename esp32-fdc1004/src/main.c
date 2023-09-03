#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp32_fdc1004_LLS.h"

#define I2C_MASTER_SCL_PIN GPIO_NUM_22
#define I2C_MASTER_SDA_PIN GPIO_NUM_21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency

#define SYS_DELAY(x) vTaskDelay(pdMS_TO_TICKS(x))

void app_main()
{
    i2c_master_init(I2C_MASTER_NUM, I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN);


    fdc_channel_t ref_channel = init_channel(I2C_MASTER_NUM, 0, FDC1004_400HZ);
    configure_single_measurement(ref_channel);

    while (1)
    {
        update_measurement(ref_channel);

        printf("Value: %d\n", ref_channel->value);

        SYS_DELAY(100);
    }
}