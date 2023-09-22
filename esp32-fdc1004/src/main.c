#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// Custom Convenience Libraries
#include "esp32_fdc1004_LLS.h"
// #include "MovingAverage.h"
#include "Button.h"
#include "i2c_handler.h"

#define I2C_MASTER_SDA_PIN GPIO_NUM_21
#define I2C_MASTER_SCL_PIN GPIO_NUM_22
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000 // I2C master clock frequency

#define SYS_DELAY(x) vTaskDelay(pdMS_TO_TICKS(x))

#define REF_CHANNEL 0
#define LEV_CHANNEL 1
#define ENV_CHANNEL 2

// Timer Stuff
#define TIMER_INTERVAL 5000 // milliseconds

// Function prototypes
void timer_callback(TimerHandle_t xTimer);

void app_main()
{
    i2c_master_init(I2C_MASTER_NUM, I2C_MASTER_SDA_PIN, I2C_MASTER_SCL_PIN);

    fdc_channel_t ref_channel = init_channel(I2C_MASTER_NUM, REF_CHANNEL, FDC1004_100HZ);
    fdc_channel_t lev_channel = init_channel(I2C_MASTER_NUM, LEV_CHANNEL, FDC1004_100HZ);
    fdc_channel_t env_channel = init_channel(I2C_MASTER_NUM, ENV_CHANNEL, FDC1004_100HZ);

    level_calc_t level_calc = init_level_calculator();

    button_t button = create_button(GPIO_NUM_25, true);

    TimerHandle_t timer = xTimerCreate("MyTimer",                       // Timer name
                                       pdMS_TO_TICKS(TIMER_INTERVAL),   // Timer period in milliseconds (e.g., 1000 ms for 1 second)
                                       pdTRUE,                          // Auto-reload the timer
                                       (void *)level_calc,              // Timer parameters
                                       timer_callback);                 // Timer callback function

    configure_single_measurement(ref_channel);
    configure_single_measurement(lev_channel);
    configure_single_measurement(env_channel);

    xTimerStart(timer, 0);

    while (1)
    {
        // Update button
        update_button(button);

        // Update readings from the sensor
        update_measurement(ref_channel);
        update_measurement(lev_channel);
        update_measurement(env_channel);

        // Get Moving Average stabilised results & update the calculator
        level_calc->ref_value = ref_channel->value;
        level_calc->lev_value = lev_channel->value;
        level_calc->env_value = env_channel->value;

        // Get level
        uint8_t level = calculate_level(level_calc);

        printf("Ref Value: %.2f\n", level_calc->ref_value);
        printf("Lev Value: %.2f\n", level_calc->lev_value);
        printf("Env Value: %.2f\n", level_calc->env_value);
        printf("Level:     %d\n", level);
        printf("---------------------------------\n");

        if (was_pushed(button))
        {
            calibrate(level_calc);
            printf("Calibration Manually Triggered!\n");
            SYS_DELAY(500);
        }

        SYS_DELAY(500);
    }

    xTimerDelete(timer, 0);
}

void timer_callback(TimerHandle_t xTimer) {
    level_calc_t level_calc = (level_calc_t)pvTimerGetTimerID(xTimer);

    calibrate(level_calc);

    printf("Interrupt! Calibration triggered!\n");
}