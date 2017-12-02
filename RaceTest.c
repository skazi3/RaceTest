/* 
	compilation command : gcc -pthread -o RaceTest skazi3-lab9.c
*/

/*
Sarah Kazi
LAB 9
CS 361
11//17/2017
*/


/* 0. add proper library*/
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#define TRUE 1;
#define FALSE 0;

typedef struct T
{
	int nBuffers;
	int workerID;
	double sleepTime;
	int semID;
	int mutexID;
	int *buffers;
	int nReadErrors;
	
}T;

void *worker(void *vargp); // thread rountine prototype 

int main(int argc, char **argv){
/*===============================PARSE COMMAND LINE ARGUMENTS=====================================*/	
	int nBuffers;
	int nWorkers;
	int randSeed;
	int lock;
	int id;
	double sleepMin = 1.0;
	double sleepMax = 5.0;


	if(argc > 1){
		nBuffers = atoi(argv[1]);
		//check if prime also
		if(nBuffers <= 2 || nBuffers >= 32 || nBuffers % 2 == 0 || nBuffers % 3 == 0 || nBuffers % 5 == 0){
			printf("Incorrect nBuffers amount. Exiting...\n");
			exit(1);
		}
	}
	if(argc > 2){
		nWorkers = atoi(argv[2]);
		if(nWorkers > nBuffers || nWorkers < 0)
			nWorkers = nBuffers - 1;
	}
	if(argc > 3)
		sleepMin = atof(argv[3]);
	if(argc > 4)
		sleepMax = atof(argv[4]);
	if(argc > 5){
		randSeed = atoi(argv[5]);
		if(randSeed == 0)
			srand(time(NULL));	
		else
			srand(randSeed);
	}
	if(argc > 6){
		if(strcmp(argv[6], "-lock") == 0){
			lock = TRUE;
		}
		else if(strcmp(argv[6], "-nolock") == 0){
			lock = FALSE;
		}
	}

/*==================================create local array============================================*/
	int *buffers;	
	buffers = (int *)malloc(sizeof(int) * nBuffers);
	int i;
	for(i = 0; i < nBuffers; i++){
		buffers[i] = i;
	}

	T tArray[nWorkers];
	pthread_t tid[nWorkers];
	for(i = 0; i < nWorkers; i++){

		tArray[i].nBuffers = nBuffers;
		tArray[i].workerID = i+1;
		tArray[i].buffers = buffers;
		tArray[i].sleepTime = (sleepMin + (sleepMax - sleepMin) * rand()) / RAND_MAX;
		tArray[i].semID = -1;
		tArray[i].mutexID = -1;
		tArray[i].nReadErrors = 0;

	}

	for(i = 0; i < nWorkers; i++){
		int id;
		id = pthread_create(&tid[i], NULL, &worker, &tArray[i]);
	}


	for(i = 0; i < nWorkers; i++){
		if(pthread_join(tid[i], NULL) != 0){
			perror("tid join");
			exit(-1);
		}
	}
	for(i = 0; i < nWorkers; i++){
		if(pthread_detach(tid[i]) != 0){
			perror("tid detach");
			exit(-1);
		}
	}


	exit(0);

}

//thread rountine
void *worker(void *vargp){
	//dereference struct
	T info = *(T*)vargp;

	/*simplify the variables from struct*/
	int workerID = info.workerID;
	int nBuffers = info.nBuffers;
	int totalAccesses = 3 * nBuffers;
	double sleepTime = info.sleepTime;

	int temp;
	int i = 0;

	//start with buffers[ID] and increment by workerID
	int a = info.buffers[workerID];
	/*begin FOR loop*/
	for(i = 0; i < totalAccesses; i++){
		printf("i: %d, ", i);
		printf("a: %d\n", a);
		temp = a;
		//sleep for random assigned time
		usleep(sleepTime);
		//check if value changed and print msg if it did
		if(temp != a){
			printf("Worker %d encountered an error!\n", workerID);
			printf("Buffer[i]:\nInitial val: %d\n", temp);
			printf("Final val:%d", a);
		}


		//increment a by workerID and wrap around
		a = (a + workerID) % nBuffers;
	}/*end FOR loop*/


  
    return NULL;

	
}






