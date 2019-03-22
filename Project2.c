/*************************************************************************************
File Name: Project2.c

Objective: Implement a monitor to allow the passage of data between producer and 
consumer threads via a shared buffer.

Created By: Kristopher Lowell
Date Created: 3/20/2019

File History:
	Created - Approx. 8 hours - KCL - 3/20/2019
*************************************************************************************/

#include "prod_cons_mt.h"														// Needed for monitor, also includes the rest of the necessary header files
#include <time.h>																// Needed for time seed

unsigned bufferCapacity;														// Global variable for size, used as extern in monitor header file

unsigned numOfProducers;														// Global variables
unsigned numOfConsumers;

unsigned eachProducer;															// How much integers each producer will producer
unsigned eachConsumer;															// How much integers each consumer will be responsbile for
unsigned lastConsumer;															// The last consumer will consumer the remainder after an even distribution

void *ProduceRandomNumbers(void *threadID)										// Producer threads
{
	unsigned producerID = (unsigned)threadID;									// Get producer ID number

	int randomNumber;

	printf("P%d: Producing %d values.\n", producerID, eachProducer);

	for(int i = 0; i < eachProducer; i++)
	{
		randomNumber = (rand() % 10) + 1;										// Produce a new random number
		monitor_write(randomNumber, producerID);								// Write it to the shared buffer properly
	}

	printf("P%d: Exiting.\n", producerID);										// Done producing all this producer thread is responsible for
	pthread_exit(NULL);
}

void *ConsumeRandomNumbers(void *threadID)										// Consumer threads
{
	unsigned consumerID = (unsigned)threadID;

	if (consumerID == (numOfConsumers - 1))										// Last consumer thread created is responsible for consuming more
	{
		printf("C%d: Consuming %d values.\n", consumerID, lastConsumer);
		for(int i = 0; i < lastConsumer; i++)
			monitor_read(consumerID);											// Read integer from shared buffer properly
	}
	else
	{
		printf("C%d: Consuming %d values.\n", consumerID, eachConsumer);
		for(int i = 0; i < eachConsumer; i++)
			monitor_read(consumerID);
	}

	printf("C%d: Exiting.\n", consumerID);										// Done consuming all this consumer thread is responsbile for
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	if (argc < 4)																// For proper operation, must have three values passed from command line
	{
		printf("ERROR: Incorrect amount of command line inputs.\n");
		return 0;
	}

	bufferCapacity = atoi(argv[1]);
	numOfProducers = atoi(argv[2]);
	numOfConsumers = atoi(argv[3]);

	if (bufferCapacity <= 0 || numOfProducers <= 0 || numOfConsumers <= 0)		// Cannot have 0 buffer size, 0 producers, and/or consumers
	{
		printf("ERROR: Command line inputs must be non-zero and positive.\n");
		return 0;
	}

	monitor_init();																// Initialize locks, condition variables, and buffer (A.K.A. monitor)

	pthread_t producers[numOfProducers];										// Create array of threads for record keeping
	pthread_t consumers[numOfConsumers];

	pthread_attr_t attr;														// Thread attribute type to ensure proper thread joining

	int returnCode = 0;															// Initialize the thread_create return code

	srand(time(0));																// Set seed of random number generator to time 0

	eachProducer = (rand() % (10 - numOfConsumers)) + numOfConsumers; 			// Randomly choose how many integers each producer will make (numbers between the number of Consumers and 10)
	eachConsumer = (eachProducer * numOfProducers) / numOfConsumers;			// This ensures each consumer thread gets at least 1 integer to consume
	lastConsumer = eachConsumer;												// Evenly distribute the total amount of integers across all the consumer threads

	if ((eachProducer * numOfProducers) % numOfConsumers)						// The last consumer thread will get the remainder
		lastConsumer = ((eachProducer * numOfProducers) % numOfConsumers) + eachConsumer;


	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);				// Initialize attribute type to allow for thread joining at end of lifespans

	for (unsigned i = 0; i < numOfProducers; i++)
	{
		pthread_create(&producers[i], &attr, ProduceRandomNumbers, (void *)i);
		if (returnCode)
		{																				// Create the producer threads
			printf("ERROR: Return code from pthread_create() = %d.\n", returnCode);
			exit(-1);
		}

		printf("Main: started producer %d.\n", i);
	}

	for (unsigned i = 0; i < numOfConsumers; i++)
	{
		pthread_create(&consumers[i], &attr, ConsumeRandomNumbers, (void *)i);
		if (returnCode)
		{																				// Create the consumer threads
			printf("ERROR: Return code from pthread_create() = %d.\n", returnCode);
			exit(-1);
		}

		printf("Main: started consumer %d.\n", i);
	}

	for (unsigned i = 0; i < numOfProducers; i++)
	{
		returnCode = pthread_join(producers[i], NULL);
		if (returnCode)
		{																				// Join the producer threads when they are completed
			printf("ERROR: Return code from pthread_join() = %d.\n", returnCode);
			exit(-1);
		}

		printf("Main: producer %d joined.\n", i);
	}

	for (unsigned i = 0; i < numOfConsumers; i++)
	{																					// Join the consumer threads when they are completed
		returnCode = pthread_join(consumers[i], NULL);
		if (returnCode)
		{
			printf("ERROR: Return code from pthread_join() = %d.\n", returnCode);
			exit(-1);
		}

		printf("Main: consumer %d joined.\n", i);
	}


	monitor_destroy();																	// Destroy the monitor to avoid segmentation faults or memory leaks
	printf("Main: program completed\n");

	return 0;
}