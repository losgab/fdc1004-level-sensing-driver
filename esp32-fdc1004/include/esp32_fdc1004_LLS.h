/*
    Driver code for Capacitive Liquid Level Sensor
    Based on FDC1004 IC from Texas Instruments
    
    Author: Gabriel Thien
    
    Based on original code written by Ashwin Whitchurch, Protocentral for Arduino framework
*/

#include <driver/i2c.h>

#define FDC1004_100HZ (0x01)
#define FDC1004_200HZ (0x02)
#define FDC1004_400HZ (0x03)
#define FDC1004_IS_RATE(x) (FDC1004_100HZ <= x && x <= FDC1004_400HZ)

#define FDC1004_CAPDAC_MAX (0x1F)

#define FDC1004_CHANNEL_MAX (0x03)
#define FDC1004_IS_CHANNEL(x) (0 <= x && x <= FDC1004_CHANNEL_MAX)

#define FDC_REGISTER (0x0C)

#define ATTOFARADS_UPPER_WORD (457) //number of attofarads for each 8th most lsb (lsb of the upper 16 bit half-word)
#define FEMTOFARADS_CAPDAC (3028) //number of femtofarads for each lsb of the capdac

#define IS_I2C_PORT(x) (I2C_NUM_0 <= x && x <= I2C_NUM_MAX)

// Measurement Output
typedef struct fdc1004_channel
{
    int16_t value;
    uint8_t capdac;
} fdc1004_channel;

// Configuration Data
typedef struct lls_config_t
{
    uint8_t i2c_master_num;
    uint8_t rate;
} lls_config_t;

lls_config_t config;

/*
    Public Functions
*/
esp_err_t lls_init(uint8_t i2c_master_channel, uint8_t rate);
esp_err_t configure_single_measurement(uint8_t channel, uint8_t capdac);
uint8_t trigger_single_measurement(uint8_t channel, uint8_t rate);
uint8_t readMeasurement(uint8_t channel);

/*
    Internal Functions
*/
void write16(uint8_t reg, uint16_t data);
uint16_t read16(uint8_t reg);


