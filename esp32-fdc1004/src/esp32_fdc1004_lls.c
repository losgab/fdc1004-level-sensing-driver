#include "esp32_fdc1004_lls.h"

fdc_channel_t init_channel(i2c_port_t i2c_port_num, uint8_t channel, uint8_t rate)
{
    if (!FDC1004_IS_CHANNEL(channel))
        return NULL;

    if (!FDC1004_IS_RATE(rate))
        return NULL;

    fdc_channel_t new_channel = malloc(sizeof(fdc1004_channel));

    // Checks for failed memory allocation
    if (new_channel == NULL)
    {
        printf("Memory allocation for new channel failed!\n");
        return NULL;
    }

    // Assigning fields
    new_channel->i2c_port_num = i2c_port_num;
    new_channel->channel = channel;
    new_channel->rate = rate;

    new_channel->config_address = config[channel];
    new_channel->msb_address = msb_addresses[channel];
    new_channel->lsb_address = lsb_addresses[channel];
    new_channel->raw_msb = 0;
    new_channel->raw_lsb = 0;
    new_channel->capdac = 0;
    new_channel->value = 0;
    return new_channel;
}

esp_err_t del_channel(fdc_channel_t channel)
{
    if (channel == NULL)
        return ESP_ERR_INVALID_ARG;
    free(channel);
    return ESP_OK;
}

esp_err_t validate_channel_obj(fdc_channel_t channel_obj)
{
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

uint8_t read_register(i2c_port_t i2c_port_num, uint8_t reg_address)
{
    int8_t error;

    // Specify which register to read with pointer byte
    uint8_t data[1];
    data[0] = reg_address;
    error = i2c_master_write_to_device(i2c_port_num, FDC_SLAVE_ADDRESS, data, sizeof(data), pdMS_TO_TICKS(100));
    if (error != ESP_OK)
        printf("Specify read register failed! %d\n", error);

    // i2c_cmd_handle_t link = i2c_cmd_link_create();
    // i2c_master_start(link);

    // // Sends slave address
    // i2c_master_write_byte(link, (FDC_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE, true);

    // // Sets the register in the pointer register to be read from
    // i2c_master_write_byte(link, reg_address, true);
    // i2c_master_stop(link);
    // i2c_master_cmd_begin(i2c_port_num, link, 100);
    // i2c_cmd_link_delete(link);

    // Sends read command for reading the current value in the register stored in the pointer register
    error = i2c_master_read_from_device(i2c_port_num, FDC_SLAVE_ADDRESS, data, sizeof(data), pdMS_TO_TICKS(100));
    if (error != ESP_OK)
        printf("Read byte in register failed! %d\n", error);

    printf("Received: %d\n", data[0]);
    return data[0];
}

esp_err_t configure_single_measurement(fdc_channel_t channel_obj)
{
    int8_t error;
    // Validation
    if (validate_channel_obj(channel_obj))
        return ESP_ERR_INVALID_ARG;

    // Write the pointer byte
    uint8_t config_address[1];
    config_address[0] = channel_obj->config_address;
    error = i2c_master_write_to_device(channel_obj->i2c_port_num, FDC_SLAVE_ADDRESS, config_address, sizeof(config_address), pdMS_TO_TICKS(1000));
    if (error != ESP_OK)
        printf("Configure slave address failed! %d\n", error);

    // Write on I2C line
    // write_byte(channel_obj->i2c_port_num, (FDC_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE);
    // write_byte(channel_obj->i2c_port_num, channel_obj->config_address);
    // write_byte(channel_obj->i2c_port_num, (uint8_t)(configuration >> 8));
    // write_byte(channel_obj->i2c_port_num, (uint8_t)(configuration));
    // Build 16 bit configuration

    uint16_t configuration = 0;
    configuration = (uint16_t)(channel_obj->channel) << 13; // CHA
    configuration |= ((uint16_t)0x04) << 10;                // CHB disable * CAPDAC enable
    configuration |= (uint16_t)(channel_obj->capdac) << 5;  // Capdac Value

    uint8_t config[2];
    config[0] = (uint8_t)(configuration >> 8);
    config[1] = (uint8_t)(configuration);
    error = i2c_master_write_to_device(channel_obj->i2c_port_num, FDC_SLAVE_ADDRESS, config, sizeof(config), pdMS_TO_TICKS(10));
    if (error != ESP_OK)
        printf("Configure slave address failed! %d\n", error);

    // write_n_bytes(channel_obj->i2c_port_num, &(uint8_t)(configuration), sizeof(configuration));

    return ESP_OK;
}

esp_err_t update_measurement(fdc_channel_t channel_obj)
{
    int8_t error;
    // Validation
    if (validate_channel_obj(channel_obj))
        return ESP_ERR_INVALID_ARG;

    // Build trigger
    uint16_t trigger = 0;
    trigger = (uint16_t)(channel_obj->rate) << 10; // Sample Rate
    trigger |= 0 << 8;                             // Disable repeat
    trigger |= (1 << (7 - channel_obj->channel));

    // Write & trigger measurement
    uint8_t fdc_register[1];
    fdc_register[0] = FDC_REGISTER;
    error = i2c_master_write_to_device(channel_obj->i2c_port_num, FDC_SLAVE_ADDRESS, fdc_register, sizeof(fdc_register), pdMS_TO_TICKS(10));
    if (error != ESP_OK)
        printf("Specific FDC_REGISTER failed! %d\n", error);
    
    uint8_t trig[2];
    trig[0] = (uint8_t)(trigger >> 8);
    trig[1] = (uint8_t)(trigger);
    error = i2c_master_write_to_device(channel_obj->i2c_port_num, FDC_SLAVE_ADDRESS, trig, sizeof(trig), pdMS_TO_TICKS(10));
    if (error != ESP_OK)
        printf("Trigger failed! %d\n", error);

    // write_byte(channel_obj->i2c_port_num, (FDC_SLAVE_ADDRESS << 1) | I2C_MASTER_WRITE);
    // write_byte(channel_obj->i2c_port_num, FDC_REGISTER);
    // write_byte(channel_obj->i2c_port_num, (uint8_t)(trigger >> 8));
    // write_byte(channel_obj->i2c_port_num, (uint8_t)(trigger));

    // write_n_bytes(channel_obj->i2c_port_num, &trigger, 2);

    // Wait for measurement to complete
    vTaskDelay(pdMS_TO_TICKS(100));

    // Read value & store
    uint8_t raw_msb = read_register(channel_obj->i2c_port_num, channel_obj->msb_address);
    uint8_t raw_lsb = read_register(channel_obj->i2c_port_num, channel_obj->lsb_address);
    channel_obj->raw_msb = raw_msb;
    channel_obj->raw_lsb = raw_lsb;

    // int16_t measurement_value = ((int16_t)msb << 8) | (int16_t)lsb;

    // Calculate capacitance
    int32_t capacitance = ((int32_t)457) * ((int32_t)raw_msb); // in attofarads
    capacitance /= 1000;                                       // in femtofarads
    capacitance += ((int32_t)3028) * ((int32_t)(channel_obj->capdac));

    // Update capdac
    update_capdac(channel_obj);

    channel_obj->value = ((float)capacitance / 1000);
    return ESP_OK;
}

esp_err_t update_capdac(fdc_channel_t channel_obj)
{
    if ((int16_t)(channel_obj->raw_msb) > FDC1004_UPPER_BOUND) // adjust capdac accordingly
    {
        if (channel_obj->capdac < FDC1004_CAPDAC_MAX)
            channel_obj->capdac++;
    }
    else if ((int16_t)(channel_obj->raw_msb) < FDC1004_LOWER_BOUND)
    {
        if (channel_obj->capdac > 0)
            channel_obj->capdac--;
    }
    return ESP_OK;
}