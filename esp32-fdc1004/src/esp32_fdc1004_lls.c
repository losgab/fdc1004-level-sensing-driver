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

esp_err_t del_channel(fdc_channel_t channel_obj)
{
    if (channel_obj == NULL)
        return ESP_ERR_INVALID_ARG;
    free(channel_obj);
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
    uint8_t data = 0;

    // Specify which register to read with pointer byte
    error = i2c_master_write_to_device(i2c_port_num, FDC_SLAVE_ADDRESS, &reg_address, sizeof(reg_address), pdMS_TO_TICKS(100));
    if (error != ESP_OK)
        printf("Specify read register failed! %d\n", error);

    // Sends read command for reading the current value in the register stored in the pointer register
    error = i2c_master_read_from_device(i2c_port_num, FDC_SLAVE_ADDRESS, &data, sizeof(data), pdMS_TO_TICKS(100));
    if (error != ESP_OK)
        printf("Read byte in register failed! %d\n", error);

    return data;
}

esp_err_t configure_single_measurement(fdc_channel_t channel_obj)
{
    int8_t error;
    uint8_t config[3];

    // Validation
    if (validate_channel_obj(channel_obj))
        return ESP_ERR_INVALID_ARG;

    // Build 16 bit configuration
    uint16_t configuration = 0;
    configuration = (uint16_t)(channel_obj->channel) << 13; // CHA
    configuration |= ((uint16_t)0x04) << 10;                // CHB disable * CAPDAC enable
    configuration |= (uint16_t)(channel_obj->capdac) << 5;  // Capdac Value

    config[0] = channel_obj->config_address;
    config[1] = (uint8_t)(configuration >> 8);
    config[2] = (uint8_t)(configuration);
    error = i2c_master_write_to_device(channel_obj->i2c_port_num, FDC_SLAVE_ADDRESS, config, sizeof(config), pdMS_TO_TICKS(1000));
    if (error != ESP_OK)
        printf("Configure measurement failed! %d\n", error);

    return ESP_OK;
}

esp_err_t update_measurement(fdc_channel_t channel_obj)
{
    int8_t error;
    uint8_t trigger[3];

    // Validation
    if (validate_channel_obj(channel_obj))
        return ESP_ERR_INVALID_ARG;

    // Build trigger
    uint16_t trigger_config = 0;
    trigger_config = (uint16_t)(channel_obj->rate) << 10; // Sample Rate
    trigger_config |= 0 << 8;                             // Disable repeat
    trigger_config |= (1 << (7 - channel_obj->channel));

    // Write trigger command
    trigger[0] = FDC_REGISTER;
    trigger[1] = (uint8_t)(trigger_config >> 8);
    trigger[2] = (uint8_t)(trigger_config);
    error = i2c_master_write_to_device(channel_obj->i2c_port_num, FDC_SLAVE_ADDRESS, trigger, sizeof(trigger), pdMS_TO_TICKS(10));
    if (error != ESP_OK)
        printf("Trigger failed! %d\n", error);

    // Wait for measurement to complete
    vTaskDelay(pdMS_TO_TICKS(50));

    // Read value & store
    uint8_t raw_msb = read_register(channel_obj->i2c_port_num, channel_obj->msb_address);
    uint8_t raw_lsb = read_register(channel_obj->i2c_port_num, channel_obj->lsb_address);
    channel_obj->raw_msb = raw_msb;
    channel_obj->raw_lsb = raw_lsb;

    int16_t measurement_value = ((int16_t)raw_msb << 8) | (int16_t)raw_lsb;

    // Calculate capacitance
    int32_t capacitance = (int32_t)ATTOFARADS_UPPER_WORD * (int32_t)measurement_value; // in attofarads
    capacitance /= 1000;                                                               // in femtofarads
    capacitance += (int32_t)FEMTOFARADS_CAPDAC * (int32_t)(channel_obj->capdac);

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

level_calc_t init_level_calculator()
{
    level_calc_t new_calc = malloc(sizeof(level_calculator));
    new_calc->level = 0;
    // level->calculated_delta = REF_FULL - REF_BASELINE;
    new_calc->calculated_delta = CALCULATED_DELTA;

    // Calculate forecasts based on calcualted delta and level baseline
    float forecast_val = 0;
    for (uint8_t i = 0; i < (FORECAST_NUM_INCREMENTS); i++)
    {
        forecast_val = LEV_BASELINE + i * new_calc->calculated_delta;
        forecast_val = round_2dp(forecast_val);
        new_calc->forecast[i] = forecast_val;
        new_calc->levels[i] = i * 5 + 5;
    }

    // Calculate Means & Slope
    float x_sum = 0, y_sum = 0, xy_sum = 0, xx_sum = 0;
    for (uint8_t i = 0; i < FORECAST_NUM_INCREMENTS; i++)
    {
        x_sum += new_calc->levels[i];
        y_sum += new_calc->forecast[i];
        xy_sum += new_calc->levels[i] * new_calc->forecast[i];
        xx_sum += new_calc->levels[i] * new_calc->levels[i];
    }

    // Calculate Slope & Intercept
    new_calc->forecast_m = round_2dp(FORECAST_NUM_INCREMENTS * xy_sum - x_sum * y_sum) / (FORECAST_NUM_INCREMENTS * xx_sum - x_sum * x_sum);
    new_calc->forecast_b = round_2dp((y_sum -  new_calc->forecast_m * x_sum) / FORECAST_NUM_INCREMENTS);

    // Calculate linear corrections
    for (uint8_t i = 0; i < FORECAST_NUM_INCREMENTS; i++)
    {
        new_calc->linear_corrections[i] = (new_calc->forecast_m * CORRECTION_GAIN) * new_calc->forecast[i] + (new_calc->forecast_b + CORRECTION_OFFSET);
    }

    return new_calc;
}

uint8_t calculate_level(level_calc_t level, float ref_value, float lev_value, float env_value)
{
    // Apply linear correction
    float linear_corrected = (new_calc->forecast_m * CORRECTION_GAIN) * new_calc->forecast[i] + (new_calc->forecast_b + CORRECTION_OFFSET);

    // Calculate level delta
    float raw_prediction = (lev_value - TARGET_CALIBRATION_B) / (TARGET_CALIBRATION_M);
    uint8_t level_prediction = round_nearest_5(raw_prediction);

    return level_prediction;
}

float round_2dp(float value)
{
    return (float)((int)(value * 100 + .5) / 100);
}

uint8_t round_nearest_5(float value)
{
    return (int)((value / 5) * 5);
}
