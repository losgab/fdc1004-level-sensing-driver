#include "esp32_fdc1004_lls.h"


esp_err_t validate_channel_obj(fdc1004_channel *channel_obj)
{
    if (!IS_I2C_PORT(channel_obj->i2c_port_num))
        return ESP_ERR_INVALID_ARG;

    if (!FDC1004_IS_CHANNEL(channel_obj->channel))
        return ESP_ERR_INVALID_ARG;
    
    if (!FDC1004_IS_RATE(channel_obj->rate))
        return ESP_ERR_INVALID_ARG;

    if (!FDC1004_IS_CONFIG_ADDRESS(channel_obj->config_address))
        return ESP_ERR_INVALID_ARG;

    if (!FDC1004_IS_MSB_ADDRESS(channel_obj->msb_address))
        return ESP_ERR_INVALID_ARG;

    if (!FDC1004_IS_LSB_ADDRESS(channel_obj->lsb_address))
        return ESP_ERR_INVALID_ARG;
    
    return ESP_OK;
}

/*
    Uses I2C interface to read data at a particular address
*/
uint8_t read_byte(i2c_port_t i2c_port_num, uint8_t reg_address)
{
    uint8_t data = 0x00;
    void *link = i2c_cmd_link_create();
    i2c_master_start(&link);

    // Sends slave address
    i2c_master_write_byte(&link, (FDC_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE, true);

    // Sets the register in the pointer register to be read from
    i2c_master_write_byte(&link, reg_address, true);

    // Sends read command for reading the current value in the register stored in the pointer register
    i2c_master_write_byte(&link, (FDC_SLAVE_ADDRESS << 1) | I2C_MASTER_READ, true);
    i2c_master_read(&link, &data, 1, true);
    i2c_master_stop(&link);
    i2c_master_cmd_begin(i2c_port_num, &link, 100);
    i2c_cmd_link_delete(&link);
    return data;
}

esp_err_t configure_single_measurement(fdc1004_channel *channel_obj)
{
    // Validation
    if (validate_channel_obj(channel_obj))
        return ESP_ERR_INVALID_ARG;

    // Build 16 bit configuration
    uint16_t configuration = 0;
    configuration = (uint16_t)(channel_obj->channel) << 13; // CHA
    configuration |= ((uint16_t)0x04) << 10;                // CHB disable * CAPDAC enable
    configuration |= (uint16_t)(channel_obj->capdac) << 5;  // Capdac Value
    write_n_bytes(channel_obj->i2c_port_num, FDC_REGISTER, &configuration, sizeof(configuration));
    return ESP_OK;
}

esp_err_t update_measurement(fdc1004_channel *channel_obj)
{
    // Validation
    if (validate_channel_obj(channel_obj))
        return ESP_ERR_INVALID_ARG;

    // Build & trigger a measurement
    uint16_t trigger = 0;
    trigger = (uint16_t)(channel_obj->rate) << 10; // Sample Rate
    trigger |= 0 << 8;                             // Disable repeat
    trigger |= (1 << (7 - channel_obj->channel));
    write_n_bytes(channel_obj->i2c_port_num, FDC_REGISTER, &trigger, sizeof(trigger));

    // Wait for measurement to complete
    vTaskDelay(pdMS_TO_TICKS(50));

    // Read value & store
    uint8_t msb = read_byte(channel_obj->i2c_port_num, channel_obj->msb_address);
    uint8_t lsb = read_byte(channel_obj->i2c_port_num, channel_obj->lsb_address);
    uint16_t measurement_value = ((uint16_t)msb << 8) | (uint16_t)lsb;
    channel_obj->value = measurement_value;
    return ESP_OK;
}