#include <stdio.h>
#include <stdlib.h>
//#include "sys/mman.h"
//#include <pthread.h>
#include <assert.h>
//#include "hwtimer.h"

#include <sys/types.h>
#include "m5op.h"
#include <sys/errno.h>


int xdim,ydim,timesteps,num_of_threads;
int** grid[2];


typedef struct _payload
{
	    
	        int lower_ydim;
	        int upper_ydim;
		int tid;
}payload;


pthread_mutex_t   SyncLock; /* mutex */
pthread_cond_t    SyncCV; /* condition variable */
int               SyncCount; /* number of processors at the barrier so far */

//pthread_mutex_t   ThreadLock; /* mutex */
//int               Count; /* the shared counter */

void swap()
{
	int** tempgrid;
	//pthread_mutex_lock(&ThreadLock);	
	tempgrid = grid[0];
	grid[0] = grid[1];
	grid[1] = tempgrid; 
	//pthread_mutex_unlock(&ThreadLock);
}

void Barrier()
{
	int ret;

	pthread_mutex_lock(&SyncLock); /* Get the thread lock */
	SyncCount++;
	//pthread_mutex_unlock(&SyncLock);

	//printf("in barrier, updated SyncCount: %d\n",SyncCount);
	if(SyncCount == num_of_threads) {
		//printf("Now resetting sync\n");
		swap();
		ret = pthread_cond_broadcast(&SyncCV);
		SyncCount = 0;
		assert(ret == 0);
		//swap();
	} else {
		//printf("releasing synclock here \r\n");
		ret = pthread_cond_wait(&SyncCV, &SyncLock);
		assert(ret == 0);
	}
	pthread_mutex_unlock(&SyncLock);
	//printf("back from barrier and synccnt:%d\n", SyncCount);
}


void *ocean (void *arguments)
{ 
        payload* args = (payload*)arguments;

        int i,j;
	//int iter = 0;
        //int **temp_grid;
        int k = timesteps;

	if(args->tid == 0)
		m5_reset_stats(0,0);
	Barrier();

	m5_work_begin(0,args->tid);
	while(k-- >= 0)
	{
		//printf("started computing for tid:%d\n",args->tid);
		for (i = args->lower_ydim; i <= args->upper_ydim; i++)
		{ 
			//printf("\n");
                        for (j = 1; j < xdim - 1 ; j++)
                        {
                                //calculating the value and storing it in the new grid
                                grid[1][j][i] = 0.2 * (grid[0][j][i] + grid[0][j][i-1] + grid[0][j-1][i] + grid[0][j][i+1] + grid[0][j+1][i]);
				//printf("\t%d",grid[1][j][i]);
                        }
                }
         //need to barrier and swapcode here
         Barrier();
	// iter++;
         }
	m5_work_end(0,args->tid);
}
                                





/* Implement this function in serial_ocean and omp_ocean */
//extern void ocean (void * arguments);

void printGrid()
{
	int i,j;
	for (i=0; i<ydim; i++)
	{
		printf("\n");
		for (j=0; j<xdim; j++)
		{
			printf("%d\t",grid[1][j][i]);
		}
	}
}

int main(int argc, char* argv[])
{
	//int xdim,ydim,timesteps,num_of_threads;
	//int** grid[2];
	pthread_attr_t attr;
	int i,j,t,rc,ret;

	//hwtimer_t timer;
	//initTimer(&timer);

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
		//printf("arguments are %d %d %d %d",xdim,ydim,timesteps,num_of_threads);
	}
	///////////////////////Get the arguments correctly (end) //////////////////////////

	/* Initialize thread attribute */
	pthread_attr_init(&attr);
	//pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); // sys manages contention

	/* Initialize mutexs */
	ret = pthread_mutex_init(&SyncLock, NULL);
	assert(ret == 0);
	//ret = pthread_mutex_init(&ThreadLock, NULL);
	//assert(ret == 0);

	/* Init condition variable */
	ret = pthread_cond_init(&SyncCV, NULL);
	assert(ret == 0);
	SyncCount = 0;

	//Count = 0;


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
	//int** temp_grid;

	// Force xdim to be a multiple of 64 bytes.
	for (i=0; i<ydim; i++) {
		grid[0][i] = &temp[i*xdim];
		grid[1][i] = &other_temp[i*ydim];
	}
	for (i=0; i<ydim; i++) {
		for (j=0; j<xdim; j++) {
			if (i == 0 || j == 0 || i == ydim - 1 || j == xdim - 1)
                        {
                                grid[0][j][i] = 1000;
                                grid[1][j][i] = 1000;
                        } else {
                                grid[0][j][i] = 500;
                                grid[1][j][i] = 500;
                        }

			//grid[0][i][j] = rand();
			//grid[1][i][j] = rand();
			//grid[0][i][j] = 1000;
                        //grid[1][i][j] = 1000;
		}
	}

	//printGrid(grid[0], xdim, ydim);
	//printGrid();

	///////////////////////create the grid as required (end) //////////////////////////

	//startTimer(&timer); // Start the time measurment here before the algorithm s
	for (t=0; t<num_of_threads; t++)
	{
		//arguments[t].grid[0] = grid[0];
		//arguments[t].grid[1] = grid[1];
		//arguments[t].xdim = xdim;
		arguments[t].tid = t;
		arguments[t].lower_ydim = ((t * (ydim - 2))/num_of_threads) + 1;
		arguments[t].upper_ydim = (((t + 1)* (ydim - 2))/num_of_threads);
		//arguments[t].timesteps = timesteps;
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
		

	//stopTimer(&timer); // End the time measuremnt here since the algorithm ende

	printf("New Matrix after Ocean wave\n");
	//printGrid(grid[0], xdim, ydim);
	//printGrid();

	//Do the time calcuclation
	//printf("Total Execution time: %lld ns\n", getTimerNs(&timer));

	pthread_mutex_destroy(&SyncLock);
	pthread_cond_destroy(&SyncCV);
	//pthread_attr_destroy(&attr);


	// Free the memory we allocated for grid
	free(temp);
	free(other_temp);
	free(grid[0]);
	free(grid[1]);

	return EXIT_SUCCESS;
}
