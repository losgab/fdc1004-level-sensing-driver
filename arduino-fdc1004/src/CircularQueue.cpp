#include "CircularQueue.h"

CircularQueue::CircularQueue()
{
	front = 0;
	rear = WINDOW_SIZE - 1;
	count = 0;
}

void CircularQueue::enqueue(float item)
{
	rear = (rear + 1) % WINDOW_SIZE;
	if (count >= WINDOW_SIZE)
	{
		front = (front + 1) % WINDOW_SIZE;
	}
	else
	{
		count++;
	}
	data[rear] = item;
	// Serial.println(data[rear]);
	// updateMinMax(queue, item);
}

float CircularQueue::calculateMovingAverage()
{
	float sum = 0;
	for (int i = 0; i < count; i++)
	{
		sum += data[(front + i) % WINDOW_SIZE];
	}
	// Serial.println(sum / count);
	return sum / count;
}

int CircularQueue::getCount()
{
	return count;
}
