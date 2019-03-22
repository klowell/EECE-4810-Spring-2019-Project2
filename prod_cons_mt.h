/*************************************************************************************
File Name: prod_cons_mt.h

Objective: Implement a monitor to allow the passage of data between producer and 
consumer threads via a shared buffer.

Created By: Kristopher Lowell
Date Created: 3/20/2019

File History:
	Created - Approx. 8 hours - KCL - 3/20/2019
*************************************************************************************/

#ifndef PROD_CONS_MT_H
#define PROD_CONS_MT_H

#include <pthread.h>									// Needed for thread operations (mutex and cond)
#include <stdio.h>										// Needed for printf
#include <stdlib.h>										// Needed for malloc
#include <stdbool.h>									// Needed for bool data type

extern unsigned bufferCapacity;							// Global variable read from command line through the main source file

struct monitor_t
{														// Monitor structure type
	int *buffer;
	unsigned head;
	unsigned tail;
	unsigned size;

	pthread_cond_t notFull;
	pthread_cond_t notEmpty;

	pthread_mutex_t lock;
};

struct monitor_t monitor;								// Declaring a working monitor

void monitor_init(void);								// Initialize the working monitor
void monitor_read(unsigned threadID);					// Read and dequeue an integer from the shared buffer
void monitor_write(int data, unsigned threadID);		// Enqueue an integer intothe shared buffer
void monitor_destroy(void);								// Properly destory the working monitor

#endif