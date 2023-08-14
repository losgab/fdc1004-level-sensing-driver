#include <Arduino.h>

#define WINDOW_SIZE 30

class CircularQueue
{
public:
	CircularQueue();
	void enqueue(float item);
	float calculateMovingAverage();
	int getCount();

private:
	float data[WINDOW_SIZE];
	int front;
	int rear;
	int count;
};