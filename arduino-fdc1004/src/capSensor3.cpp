#include "capSensor3.hpp"

uint8_t measure(FDC1004 fdc, uint8_t channel, uint8_t freq, int *capdac_addr)
{
    fdc.configureMeasurementSingle(channel, channel, *capdac_addr);
    fdc.triggerSingleMeasurement(channel, freq);
    delay(MEASURE_DELAY);
    return 0;
}

float getMeasurement(FDC1004 fdc, uint8_t channel, int *capdac_addr)
{
    uint16_t value[2];
    if (!fdc.readMeasurement(channel, value))
    {
        int capdac = *capdac_addr;
        int16_t msb = (int16_t)value[0];
        int32_t capacitance = ((int32_t)457) * ((int32_t)msb); // in attofarads
        capacitance /= 1000;                                   // in femtofarads
        capacitance += ((int32_t)3028) * ((int32_t)capdac);

        if (msb > UPPER_BOUND) // adjust capdac accordingly
        {
            if (capdac < FDC1004_CAPDAC_MAX)
                capdac++;
        }
        else if (msb < LOWER_BOUND)
        {
            if (capdac > 0)
                capdac--;
        }
        *capdac_addr = capdac;
        return ((float)capacitance / 1000);
    }
    return 0;
}

float calculateLevel(float ref_val, float level_val, float env_val)
{
    if (level_val <= LEVEL_ZERO && ref_val > REF_ZERO)
    {
        return ref_val / REF_ZERO;
    }
    else if (level_val > LEVEL_ZERO)
    {
        return level_val * LEVEL_GAIN + LEVEL_OFFSET;
    }
    else return 0;
        // float raw_level = REF_HEIGHT * ((level_val - LEVEL_ZERO) / (ref_val - env_val));
        // return raw_level * LEVEL_GAIN + LEVEL_OFFSET;
        
    // float level = ((level_val - LEVEL_ZERO) / (ref_val - (env_val / 4)));
}

void testLoop()
{
    FDC1004 fdc;
    int capdac0, capdac1, capdac2 = 0;

    while (!Serial.available())
    {
        measure(fdc, REF_CHANNEL, FDC1004_400HZ, &capdac0);
        float ref_val = getMeasurement(fdc, REF_CHANNEL, &capdac0);

        Serial.print("Ref Sensor: ");
        Serial.print(ref_val, 2);
        Serial.println("  pf");

        measure(fdc, LEV_CHANNEL, FDC1004_400HZ, &capdac1);
        float lev_val = getMeasurement(fdc, LEV_CHANNEL, &capdac1);
        Serial.print("Level Sensor: ");
        Serial.print(lev_val, 2);
        Serial.println("  pf");

        measure(fdc, ENV_CHANNEL, FDC1004_400HZ, &capdac2);
        float env_val = getMeasurement(fdc, ENV_CHANNEL, &capdac2);
        Serial.print("Env Sensor: ");
        Serial.print(env_val, 2);
        Serial.println("  pf");

        Serial.println("--------------");
        float level = calculateLevel(ref_val, lev_val, env_val);
        Serial.print("Predicted Level: ");
        Serial.println(level, 2);
        Serial.println("--------------");
        delay(500);
    } 
}

float updateMovingAverage(CircularQueue queue, float value)
{
    queue.enqueue(value);
    float result = queue.calculateMovingAverage();

    return result;
}