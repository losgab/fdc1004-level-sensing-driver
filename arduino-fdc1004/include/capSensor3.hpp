#include <Protocentral_FDC1004.h>
#include "CircularQueue.h"

#define UPPER_BOUND 0X4000 // max readout capacitance
#define LOWER_BOUND (-1 * UPPER_BOUND)

#define REF_CHANNEL 0
#define LEV_CHANNEL 1
#define ENV_CHANNEL 2

#define REF_HEIGHT 7
#define REF_ZERO 1.81
#define REF_MAX 2.3
#define LEVEL_ZERO 6.8
#define ENV_ZERO 1.77

#define REF_VAL_OFFSET (REF_ZERO - ENV_ZERO)

#define MEASURE_DELAY 15

#define LEVEL_GAIN 1.481
#define LEVEL_OFFSET 8.178

uint8_t measure(FDC1004, uint8_t, uint8_t, int*);
float getMeasurement(FDC1004, uint8_t, int *);
float calculateLevel(float, float, float);
void testLoop();

void calibrateZero(); // Take moving averageo of a 3 second snapshot

float updateMovingAverage(CircularQueue, float);