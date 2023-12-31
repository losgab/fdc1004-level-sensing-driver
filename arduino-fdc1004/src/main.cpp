#include <Arduino.h>
#include <Protocentral_FDC1004.h>
#include <I2CScanner.h>
#include "capSensor3.hpp"

I2CScanner scanner;

#define REF_CHANNEL 0	// measurment channel
#define LEVEL_CHANNEL 1 // measurment channel
#define ENV_CHANNEL 2	// measurment channel

int capdac0, capdac1, capdac2 = 0;

FDC1004 FDC;

void setup()
{
	Wire.begin();		// i2c begin
	Serial.begin(9600); // serial baud rate
	scanner.Init();
	delay(200);
	scanner.Scan();
	delay(200);
}

void loop()
{
	// // testLoop();
	// measure(FDC, REF_CHANNEL, FDC1004_400HZ, &capdac0);
	// float ref_val = getMeasurement(FDC, REF_CHANNEL, &capdac0);

	// measure(FDC, LEVEL_CHANNEL, FDC1004_400HZ, &capdac1);
	// float lev_val = getMeasurement(FDC, LEVEL_CHANNEL, &capdac1);

	// measure(FDC, ENV_CHANNEL, FDC1004_400HZ, &capdac2);
	// float env_val = getMeasurement(FDC, ENV_CHANNEL, &capdac2);

	// Serial.println("----------");
	// Serial.print("R: ");
	// Serial.println(ref_val, 1);
	// // Serial.println("  pf");
	// Serial.print("L: ");
	// Serial.println(lev_val, 1);
	// // Serial.println("  pf");
	// Serial.print("E: ");
	// Serial.println(env_val, 1);
	// // Serial.println("  pf");
	// Serial.println("----------");
	// float level = calculateLevel(ref_val, lev_val, env_val);
	// Serial.print("O: ");
	// Serial.println(level, 1);
	// Serial.println("----------");

	// delay(200);
}