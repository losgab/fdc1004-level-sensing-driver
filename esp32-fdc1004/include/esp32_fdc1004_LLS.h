/*
    Driver code for Capacitive Liquid Level Sensor
    Based on FDC1004 IC from Texas Instruments
    
    Author: Gabriel Thien
    
    Based on code written by Ashwin Whitchurch (Protocentral) for Arduino framework
*/

#include <driver/i2c.h>

#define FDC_SLAVE_ADDRESS 0b1010000

#define FDC1004_100HZ (0x01)
#define FDC1004_200HZ (0x02)
#define FDC1004_400HZ (0x03)
#define FDC1004_IS_RATE(x) (FDC1004_100HZ <= x && x <= FDC1004_400HZ)

#define FDC1004_CAPDAC_MAX (0x1F)

#define FDC1004_CHANNEL_MAX (0x03)
#define FDC1004_IS_CHANNEL(x) (x <= FDC1004_CHANNEL_MAX)

#define FDC1004_IS_CONFIG_ADDRESS(a) (0x08 <= a && a <= 0x0B)

#define FDC1004_IS_MSB_ADDRESS(a) (a % 2 == 0 && 0x00 <= a && a <= 0x06)
#define FDC1004_IS_LSB_ADDRESS(a) (a % 2 == 1 && 0x01 <= a && a <= 0x07)

#define FDC_REGISTER (0x0C)

#define ATTOFARADS_UPPER_WORD (457) //number of attofarads for each 8th most lsb (lsb of the upper 16 bit half-word)
#define FEMTOFARADS_CAPDAC (3028) //number of femtofarads for each lsb of the capdac

#define FDC1004_UPPER_BOUND ((int16_t) 0x4000)
#define FDC1004_LOWER_BOUND (-1 * FDC1004_UPPER_BOUND)

static const uint8_t config[] = {0x08, 0x09, 0x0A, 0x0B};
static const uint8_t msb_addresses[] = {0x00, 0x02, 0x04, 0x06};
static const uint8_t lsb_addresses[] = {0x01, 0x03, 0x05, 0x07};

// Measurement Output
typedef struct fdc1004_channel
{
    i2c_port_t i2c_port_num;
    uint8_t channel;
    uint8_t rate;
    uint8_t config_address;
    uint8_t msb_address;
    uint8_t lsb_address;
    int raw_msb;
    int raw_lsb;
    int capdac;
    float value;
} fdc1004_channel;

typedef fdc1004_channel* fdc_channel_t;

/**
 * @brief Initialises a channel struct for storing all data associated with channel readings
 * 
 * @param i2c_port_num I2C port number to initialise the channel on
 * @param channel Channel number on the FDC1004 to use for readings
 * @param rate Rate at which measurements are taken on the FDC1004
 * 
 * @return Pointer to fdc_channel struct, NULL if failed because out of memory or some other internal error
*/
fdc_channel_t init_channel(i2c_port_t i2c_port_num, uint8_t channel, uint8_t rate);

/**
 * @brief Frees associated memory with pointer
 * 
 * @param channel_obj Pointer to channel struct
 * 
 * @return ESP_OK if all good.
*/
esp_err_t del_channel(fdc_channel_t channel_obj);

/**
 * @brief Validates the fields in the channel struct
 * 
 * @return ESP_OK if good, ESP_ERR_INVLD_ARG if there is mismatch data
*/
esp_err_t validate_channel_obj(fdc_channel_t channel_obj);

/**
 * @brief Uses I2C interface to read data at a particular address
 * 
 * @param i2c_port_num I2C port number
 * @param reg_address Address of the register in the FDC1004 to be read
 * 
 * @return byte stored at that address
*/
uint8_t read_register(i2c_port_t i2c_port_num, uint8_t reg_address);

/**
 * @brief Configures by building and configuring 16 bit configuration for config registers
 * 
 * @param channel_obj Pointer to channel struct
 * 
 * @return ESP_OK if good, ESP_ERR_INVLD_ARG if there is mismatch data
*/
esp_err_t configure_single_measurement(fdc_channel_t channel_obj);

/**
 * @brief Triggers and updates measurements of the channel struct
 * 
 * @param channel_obj Pointer to channel struct
 * 
 * @return ESP_OK if good, ESP_ERR_INVLD_ARG if there is mismatch data
*/
esp_err_t update_measurement(fdc_channel_t channel_obj);

/**
 * @brief Updates the capdac associated to the channel
 * 
 * @param channel_obj Pointer to channel struct
 * 
 * @return ESP_OK if good, ESP_ERR_INVLD_ARG if there is mismatch data
*/
esp_err_t update_capdac(fdc_channel_t channel_obj);
