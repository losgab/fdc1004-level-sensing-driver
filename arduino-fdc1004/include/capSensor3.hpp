#include <Protocentral_FDC1004.h>

#define UPPER_BOUND 0X4000 // max readout capacitance
#define LOWER_BOUND (-1 * UPPER_BOUND)

#define REF_CHANNEL 0
#define LEV_CHANNEL 1
#define ENV_CHANNEL 2

#define REF_HEIGHT 5
#define REF_ZERO 2.1
#define LEVEL_ZERO 6.05
#define ENV_ZERO 1.8

#define MEASURE_DELAY 50

uint8_t measure(FDC1004, uint8_t, uint8_t, int*);
float getMeasurement(FDC1004, uint8_t, int *);
float calculateLevel(float, float, float);
void testLoop();