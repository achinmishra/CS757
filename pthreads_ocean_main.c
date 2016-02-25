#include <stdio.h>
#include <stdlib.h>
#include "sys/mman.h"
#include <pthread.h>

#include "hwtimer.h"

typedef struct _payload
{
	        int** grid[2];
	        int xdim;
	        int lower_ydim;
	        int upper_ydim;
}payload;

/* Implement this function in serial_ocean and omp_ocean */
extern void ocean (void * arguments);

void printGrid(int** grid, int xdim, int ydim)
{
	int i,j;
	for (i=0; i<ydim; i++)
	{
		printf("\n");
		for (j=0; j<xdim; j++)
		{
			printf("%d\t",grid[j][i]);
		}
	}
}

int main(int argc, char* argv[])
{
	int xdim,ydim,timesteps,num_of_threads;
	int** grid[2];
	int i,j,t,rc;

	hwtimer_t timer;
	initTimer(&timer);

	/********************Get the arguments correctly (start) **************************/
	/* 
	   Three input Arguments to the program
	   1. X Dimension of the grid
	   2. Y dimension of the grid
	   3. number of timesteps the algorithm is to be performed
	   */

	if (argc!=5) {
		printf("The Arguments you entered are wrong.\n");
		printf("./pthreads_ocean <x-dim> <y-dim> <timesteps> <number of threads>\n");
		return EXIT_FAILURE;
	} else {
		xdim = atoi(argv[1]);
		ydim = atoi(argv[2]);
		timesteps = atoi(argv[3]);
		num_of_threads = atoi(argv[4]);
	}
	///////////////////////Get the arguments correctly (end) //////////////////////////


	/*********************create the grid as required (start) ************************/
	/*
	   The grid needs to be allocated as per the input arguments and randomly initialized.
	   Remember during allocation that we want to guarantee a contiguous block, hence the
	   nasty pointer math.

	// TA NOTE: If you dislike this scheme and want to say, allocate a single contiguous
	// 1D array, and do the pointer arithmetic yourself during the iteration, you may do so.

	To test your code for correctness please comment this section of random initialization.
	*/
	grid[0] = (int**) malloc(ydim*sizeof(int*));
	grid[1] = (int**) malloc(ydim*sizeof(int*));
	int *temp = (int*) malloc(xdim*ydim*sizeof(int));
	int *other_temp = (int*) malloc(xdim*ydim*sizeof(int));

	pthread_t threads[num_of_threads]; //instantiating the number of threads inputted
	payload arguments[num_of_threads];
	void *status;
	int** temp_grid;

	// Force xdim to be a multiple of 64 bytes.
	for (i=0; i<ydim; i++) {
		grid[0][i] = &temp[i*xdim];
		grid[1][i] = &other_temp[i*ydim];
	}
	for (i=0; i<ydim; i++) {
		for (j=0; j<xdim; j++) {
			grid[0][i][j] = rand();
		}
	}

	//printGrid(grid[0], xdim, ydim);


	///////////////////////create the grid as required (end) //////////////////////////

	startTimer(&timer); // Start the time measurment here before the algorithm starts
	while(timesteps-- >= 0)
	{
		for (t=0; t<num_of_threads; t++)
		{
			arguments[t].grid[0] = grid[0];
			arguments[t].grid[1] = grid[1];
			arguments[t].xdim = xdim;
			arguments[t].lower_ydim = ((t * (ydim - 2))/num_of_threads) + 1;
			//arguments[t].upper_ydim = (t == num_of_threads - 1) ? ((((t + 1)* (ydim - 2))/num_of_threads) - 1): (((t + 1)* (ydim - 2))/num_of_threads);
			arguments[t].upper_ydim = (((t + 1)* (ydim - 2))/num_of_threads);
			rc = pthread_create(&threads[t], NULL, ocean, &arguments[t]);
			if(rc)
			{
				printf("Error! Thread creation failed\n");
				exit(0);
			}
		}
	
		for (t=0; t<num_of_threads; t++)
	        {
			rc = pthread_join(threads[t], &status);
			if(rc)
			{
				printf("Error! Thread joining failed\n");
				exit(0);
			}
		}
		//swap the grids
		temp_grid = grid[0];
		grid[0] = grid[1];
		grid[1] = temp_grid;
	}		

	stopTimer(&timer); // End the time measuremnt here since the algorithm ended

	//printf("New Matrix after Ocean wave\n");
	//printGrid(grid[0], xdim, ydim);

	//Do the time calcuclation
	printf("Total Execution time: %lld ns\n", getTimerNs(&timer));

	// Free the memory we allocated for grid
	free(temp);
	free(other_temp);
	free(grid[0]);
	free(grid[1]);

	return EXIT_SUCCESS;
}
