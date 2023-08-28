#include "esp32_fdc1004_lls.h"

/*
    Uses I2C interface to send commands and data through to FDC1004
*/
void write16(uint8_t address, uint16_t data)
{
    void *link = i2c_cmd_link_create();
    i2c_master_start(&link);
    i2c_master_write_byte(&link, (address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(&link, data, sizeof(data), true);
    i2c_master_stop(&link);
    i2c_master_cmd_begin(config.i2c_master_num, &link, 100);
    i2c_cmd_link_delete(&link);
}

/*
    Uses I2C interface to read data at a particular address
*/
uint8_t read8(uint8_t address)
{
    uint8_t data = 0;
    void *link = i2c_cmd_link_create();
    i2c_master_start(&link);
    i2c_master_write_byte(&link, (address << 1) | I2C_MASTER_READ, true);
    i2c_master_read(&link, &data, 1, true);
    i2c_master_stop(&link);
    i2c_master_cmd_begin(config.i2c_master_num, &link, 100);
    i2c_cmd_link_delete(&link);
    return data;
}

/*
    Initialises communication and configures FDC1004 data transfer
*/
esp_err_t lls_init(uint8_t i2c_master_channel, uint8_t rate)
{
    if (!FDC1004_IS_RATE(rate) || !IS_I2C_PORT(i2c_master_channel))
        return ESP_ERR_INVALID_ARG;

    config.i2c_master_num = i2c_master_channel;
    config.rate = rate;
}

/*
    Configures a channel for a measurement
*/
esp_err_t configure_single_measurement(fdc1004_channel *channel_obj)
{
    if (!FDC1004_IS_CHANNEL(channel_obj->channel))
        return ESP_ERR_INVALID_ARG;

    // Build 16 bit configuration
    uint16_t configuration = 0;
    configuration = (uint16_t)(channel_obj->channel) << 13; // CHA
    configuration |= ((uint16_t)0x04) << 10;                // CHB disable * CAPDAC enable
    configuration |= (uint16_t)(channel_obj->capdac) << 5;  // Capdac Value
    write16(channel_obj->config_address, configuration);
    return ESP_OK;
}

/*
    Triggers, Reads & Updates measurement
*/
esp_err_t update_measurement(fdc1004_channel *channel_obj)
{
    // Checks valid Channel num & Rate
    if (!FDC1004_IS_CHANNEL(channel_obj->channel) || !FDC1004_IS_RATE(channel_obj->rate))
        return ESP_ERR_INVALID_ARG;

    // Checks MSB & LSB addresses
    if (!FDC1004_IS_MSB_ADDRESS(channel_obj->msb_address) || !FDC1004_IS_LSB_ADDRESS(channel_obj->lsb_address))
        return ESP_ERR_INVALID_ARG;

    // Build & trigger a measurement
    uint16_t trigger = 0;
    trigger = (uint16_t)(channel_obj->rate) << 10; // Sample Rate
    trigger |= 0 << 8;                             // Disable repeat
    trigger |= (1 << (7 - channel_obj->channel));
    write16(FDC_REGISTER, trigger);

    // Wait for measurement to complete
    vTaskDelay(pdMS_TO_TICKS(50));

    // Read value & store
    uint8_t msb = read8(channel_obj->msb_address);
    uint8_t lsb = read8(channel_obj->lsb_address);
    uint16_t measurement_value = ((uint16_t)msb << 8) | (uint16_t)lsb;
    channel_obj->value = measurement_value;
    return ESP_OK;
}