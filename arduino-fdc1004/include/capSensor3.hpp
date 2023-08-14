#include <Protocentral_FDC1004.h>
#include "CircularQueue.h"

#define UPPER_BOUND 0X4000 // max readout capacitance
#define LOWER_BOUND (-1 * UPPER_BOUND)

#define REF_CHANNEL 0
#define LEV_CHANNEL 1
#define ENV_CHANNEL 2

#define REF_HEIGHT 6
#define REF_ZERO 2.1
#define REF_MAX 2.35
#define LEVEL_ZERO 7.6
#define LEVEL_MAX 13.2
#define ENV_ZERO 1.8

#define REF_VAL_OFFSET (REF_ZERO - ENV_ZERO)

#define MEASURE_DELAY 15

#define LEVEL_GAIN 1.481
#define LEVEL_OFFSET 8.178

#define LEVEL_TRANSFORM_M ((10 - 1) / (LEVEL_MAX - LEVEL_ZERO))
#define LEVEL_TRANSFORM_B (1 - LEVEL_TRANSFORM_M * LEVEL_ZERO)

#define INVERSE_M 17.574
#define INVERSE_B -135.149

uint8_t measure(FDC1004, uint8_t, uint8_t, int*);
float getMeasurement(FDC1004, uint8_t, int *);
float calculateLevel(float, float, float);
void testLoop();

void calibrateZero(); // Take moving averageo of a 3 second snapshot

float updateMovingAverage(CircularQueue, float);

/* 
10 -> 7.9 - 8.0
15 -> 8.4 - 8.5
20 -> 8.7
25 -> 8.9 - 9.0
30 -> 9.4
35 -> 9.8 - 9.9
40 -> 10.1
45 -> 10.4
50 -> 10.7 - 10.8
55 -> 11.1
60 -> 11.3 
65 -> 11.5
70 -> 11.7
75 -> 11.9 - 12.0
80 -> 12.2
85 -> 12.5
90 -> 12.7
95 -> 12.9
100 -> 13.2

y = 0.0569x + 7.69
y_1 = 17.574x - 135.149

*/
