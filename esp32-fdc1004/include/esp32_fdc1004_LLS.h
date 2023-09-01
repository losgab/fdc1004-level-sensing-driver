/*
    Driver code for Capacitive Liquid Level Sensor
    Based on FDC1004 IC from Texas Instruments
    
    Author: Gabriel Thien
    
    Based on code written by Ashwin Whitchurch (Protocentral) for Arduino framework
*/

#include <driver/i2c.h>
#include "i2c_handler.h"

#define FDC_SLAVE_ADDRESS 0b1010000

#define FDC1004_100HZ (0x01)
#define FDC1004_200HZ (0x02)
#define FDC1004_400HZ (0x03)
#define FDC1004_IS_RATE(x) (FDC1004_100HZ <= x && x <= FDC1004_400HZ)

#define FDC1004_CAPDAC_MAX (0x1F)

#define FDC1004_CHANNEL_MAX (0x03)
#define FDC1004_IS_CHANNEL(x) (0 <= x && x <= FDC1004_CHANNEL_MAX)

#define FDC1004_IS_CONFIG_ADDRESS(a) (0x08 <= a && a <= 0x0B)

#define FDC1004_IS_MSB_ADDRESS(a) (a % 2 == 0 && 0x00 <= a && a <= 0x06)
#define FDC1004_IS_LSB_ADDRESS(a) (a % 2 == 1 && 0x01 <= a && a <= 0x07)

#define FDC_REGISTER (0x0C)

#define ATTOFARADS_UPPER_WORD (457) //number of attofarads for each 8th most lsb (lsb of the upper 16 bit half-word)
#define FEMTOFARADS_CAPDAC (3028) //number of femtofarads for each lsb of the capdac

#define FDC1004_UPPER_BOUND ((int16_t) 0x4000)
#define FDC1004_LOWER_BOUND (-1 * FDC1004_UPPER_BOUND)

const uint8_t config[] = {0x08, 0x09, 0x0A, 0x0B};
const uint8_t msb[] = {0x00, 0x02, 0x04, 0x06};
const uint8_t lsb[] = {0x01, 0x03, 0x05, 0x07};

// Measurement Output
typedef struct fdc1004_channel
{
    i2c_port_t i2c_port_num;
    uint8_t channel;
    uint8_t rate;
    uint8_t config_address;
    uint8_t msb_address;
    uint8_t lsb_address;
    uint8_t raw_msb;
    uint8_t raw_lsb;
    int capdac;
    int16_t value;
} fdc1004_channel_obj;

typedef fdc1004_channel_obj* fdc_channel_t;

/*
    Creates a channel struct for keeping track of measurements
*/
esp_err_t init_channel(fdc_channel_t channel_obj, i2c_port_t i2c_port_num, uint8_t channel, uint8_t rate);

/*
    Validates struct data for FDC guidelines
*/
esp_err_t validate_channel_obj(fdc_channel_t channel_obj);

/*
    Reads the byte currently stored inside the specified register
*/
uint8_t read_register(i2c_port_t i2c_port_num, uint8_t reg_address);

/*
    Configures a channel for measurement
*/
esp_err_t configure_single_measurement(fdc_channel_t channel_obj);

/*
    Triggers, Reads & Updates measurement
*/
esp_err_t update_measurement(fdc_channel_t channel_obj);

/*
    Update channel capdac
*/
esp_err_t update_capdac(fdc_channel_t channel_obj);
