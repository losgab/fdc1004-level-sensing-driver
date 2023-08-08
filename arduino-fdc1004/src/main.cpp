#include <Arduino.h>
#include <Protocentral_FDC1004.h>

#define UPPER_BOUND 0X4000 // max readout capacitance
#define LOWER_BOUND (-1 * UPPER_BOUND)
#define REF_CHANNEL 0 // measurment channel
#define LEVEL_CHANNEL 1 // measurment channel
// #define MEASURMENT2 2 // measurment channel

int capdac0, capdac1 = 0;

FDC1004 FDC;

void setup()
{
	Wire.begin();		  // i2c begin
	Serial.begin(115200); // serial baud rate
	FDC.configureMeasurementSingle(REF_CHANNEL, REF_CHANNEL, capdac0);
	FDC.configureMeasurementSingle(LEVEL_CHANNEL, LEVEL_CHANNEL, capdac1);
	// FDC.configureMeasurementSingle(MEASURMENT2, CHANNEL, capdac);
}

void loop()
{
	FDC.triggerSingleMeasurement(REF_CHANNEL, FDC1004_400HZ);

	// wait for completion
	delay(15);
	uint16_t ref_value[2];
	if (!FDC.readMeasurement(REF_CHANNEL, ref_value))
	{
		int16_t msb = (int16_t)ref_value[0];
		int32_t capacitance = ((int32_t)457) * ((int32_t)msb); // in attofarads
		capacitance /= 1000;								   // in femtofarads
		capacitance += ((int32_t)3028) * ((int32_t)capdac0);

		if (msb > UPPER_BOUND) // adjust capdac accordingly
		{
			if (capdac0 < FDC1004_CAPDAC_MAX)
				capdac0++;
		}
		else if (msb < LOWER_BOUND)
		{
			if (capdac0 > 0)
				capdac0--;
		}
		Serial.print("Ref Sensor: ");
		Serial.print((((float)capacitance / 1000)), 2);
		Serial.println("  pf");
	}


	uint16_t level_value[2];
	FDC.triggerSingleMeasurement(LEVEL_CHANNEL, FDC1004_400HZ);
	delay(15);
	if (!FDC.readMeasurement(LEVEL_CHANNEL, level_value))
	{
		int16_t msb = (int16_t)level_value[0];
		int32_t capacitance = ((int32_t)457) * ((int32_t)msb); // in attofarads
		capacitance /= 1000;								   // in femtofarads
		capacitance += ((int32_t)3028) * ((int32_t)capdac1);

		if (msb > UPPER_BOUND) // adjust capdac accordingly
		{
			if (capdac1 < FDC1004_CAPDAC_MAX)
				capdac1++;
		}
		else if (msb < LOWER_BOUND)
		{
			if (capdac1 > 0)
				capdac1--;
		}
		Serial.print("Level Sensor: ");
		Serial.print((((float)capacitance / 1000)), 2);
		Serial.println("  pf");
	}
	delay(500);
}
