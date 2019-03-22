/*************************************************************************************
File Name: prod_cons_mt.c

Objective: Implement a monitor to allow the passage of data between producer and 
consumer threads via a shared buffer.

Created By: Kristopher Lowell
Date Created: 3/20/2019

File History:
	Created - Approx. 8 hours - KCL - 3/20/2019
*************************************************************************************/

#include "prod_cons_mt.h"

void monitor_init(void)																		// Initialize monitor values and buffer
{
	monitor.buffer = (int *) malloc(sizeof(int) * bufferCapacity);							// Dynamically allocate buffer since the capacity is dependent on passed value from command line
	monitor.head = 0;
	monitor.tail = 0;																		// Circular buffer (technically array) has head and tail start in same place
	monitor.size = 0;

	pthread_cond_init(&monitor.notFull, NULL);												// Initializing the condition variables
	pthread_cond_init(&monitor.notEmpty, NULL);

	pthread_mutex_init(&monitor.lock, NULL);												// Initializing the lock (mutual exclusion)
}

void monitor_read(unsigned threadID)
{
	int data;
	bool blocked = false;

	pthread_mutex_lock(&monitor.lock);

	while(!monitor.size)																	// If the buffer is empty, wait for a producer to put an integer in
	{
		if(!blocked)
		{
			printf("C%d: Blocked due to empty buffer.\n", threadID);
			blocked = true;
		}
		pthread_cond_wait(&monitor.notEmpty, &monitor.lock);
	}
	if(blocked)
		printf("C%d: Done waiting on empty buffer.\n", threadID);

	data = monitor.buffer[monitor.head];
	printf("C%d: Reading %d from position %d.\n", threadID, data, monitor.head);
	monitor.head = (monitor.head + 1) % bufferCapacity;										// Increment head node index, taking care of overflow
	monitor.size--;																			// Decrement size to reflect a dequeue

	pthread_cond_signal(&monitor.notFull);													// Signal another (producer) thread that the buffer can no longer be full
	pthread_mutex_unlock(&monitor.lock);
}

void monitor_write(int data, unsigned threadID)
{
	bool blocked = false;

	pthread_mutex_lock(&monitor.lock);

	while(monitor.size == bufferCapacity)													// If the buffer is full, wait for a consumer to take an integer out
	{
		if(!blocked)
		{
			printf("P%d: Blocked due to full buffer.\n", threadID);
			blocked = true;
		}
		pthread_cond_wait(&monitor.notFull, &monitor.lock);
	}

	if(blocked)
		printf("P%d: Done waiting on full buffer.\n", threadID);

	printf("P%d: Writing %d to position %d.\n", threadID, data, monitor.tail);
	monitor.buffer[monitor.tail] = data;
	monitor.tail = (monitor.tail + 1) % bufferCapacity;										// Increment tail node index, taking care of overflow
	monitor.size++;

	pthread_cond_signal(&monitor.notEmpty);													// Singal another (consumer) thread that the buffer can no longer be empty
	pthread_mutex_unlock(&monitor.lock);
}

void monitor_destroy(void)
{
	free(monitor.buffer);																	// De-allocate the memory used for the buffer
	monitor.buffer = NULL;

	pthread_cond_destroy(&monitor.notFull);													// Properly destroy the condition variables
	pthread_cond_destroy(&monitor.notEmpty);

	pthread_mutex_destroy(&monitor.lock);													// Properly destroy the lock
}