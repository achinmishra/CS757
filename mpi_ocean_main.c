#include <stdio.h>
#include <stdlib.h>
#include "sys/mman.h"
#include <mpi.h>

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
	int xdim,ydim,lower_ydim, upper_ydim,timesteps;
	int** grid[2];
	int** temp_grid;
	int i,j,k;
	
	int myid, numprocs, next, rc, namelen;
    	char processor_name[MPI_MAX_PROCESSOR_NAME];
    	double startwtime, endwtime;
    	MPI_Status status;
	
	/* Initialize the MPI execution environment */
    	MPI_Init(NULL,NULL);

    	/*
     * Determines the size of the group associated
     * with a communicator 
     *
     *     int MPI_Comm_size ( comm, size )
     *     MPI_Comm comm; //  communicator (handle) 
     *     int *size; // number of processes in the group of comm (integer)
     */
    	MPI_Comm_size(MPI_COMM_WORLD,&numprocs);

    /*
     * Determines the rank of the calling process in the
     * communicator
     * 
     *      int MPI_Comm_rank ( comm, rank )
     *      MPI_Comm  comm; //  communicator (handle) 
     *      int      *rank; //  rank of the calling process in group of 
     *                      //  comm (integer) 
     */
    	MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    /* 
     * Gets the name of the processor 
     *
     *      int MPI_Get_processor_name( name, resultlen )
     *      char *name;  //  A unique specifier for the actual 
     *                   //  (as opposed to virtual) node. 
     *      int *resultlen; //  Length (in characters) of the name 
     */
    	MPI_Get_processor_name(processor_name,&namelen);


	/********************Get the arguments correctly (start) **************************/
	/* 	   Three input Arguments to the program
	   1. X Dimension of the grid
	   2. Y dimension of the grid
	   3. number of timesteps the algorithm is to be performed
	   */

	if (argc!=4) {
		printf("The Arguments you entered are wrong.\n");
		printf("./pthreads_ocean <x-dim> <y-dim> <timesteps>\n");
		return EXIT_FAILURE;
	} else {
		xdim = atoi(argv[1]);
		ydim = atoi(argv[2]);
		timesteps = atoi(argv[3]);
	}

	grid[0] = (int**) malloc(ydim*sizeof(int*));
	grid[1] = (int**) malloc(ydim*sizeof(int*));
	int *temp = (int*) malloc(xdim*ydim*sizeof(int));
	int *other_temp = (int*) malloc(xdim*ydim*sizeof(int));

	// Force xdim to be a multiple of 64 bytes.
	for (i=0; i<ydim; i++) {
		grid[0][i] = &temp[i*xdim];
		grid[1][i] = &other_temp[i*ydim];
	}


	lower_ydim = ((myid * (ydim - 2))/numprocs);
	upper_ydim = (((myid + 1)* (ydim - 2))/numprocs) + 1;
	for (i=0; i<xdim; i++) {
		for (j=0; j<ydim; j++) {
			grid[0][i][j] = rand();
		}
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
	if(myid == 0)
		startwtime = MPI_Wtime();

	while(timesteps-- >= 0) {
		if(myid == 0)
		{	
			for(i = 1; i < numprocs; i++)
			{
				lower_ydim = ((i * (ydim - 2))/numprocs);
			        upper_ydim = (((i + 1)* (ydim - 2))/numprocs) + 1;	
				MPI_Send(grid[0][lower_ydim], (upper_ydim - lower_ydim + 1)*xdim, MPI_INT, i, 99, MPI_COMM_WORLD);
			}

			lower_ydim = 0;
			upper_ydim = ((ydim - 2)/numprocs) + 1;

			for (i = lower_ydim + 1; i < upper_ydim; i++)
			{
				for (j = 1; j < xdim - 1 ; j++)
				{
					//calculating the value and storing it in the new grid
					grid[1][i][j] = 0.2 * (grid[0][i][j] + grid[0][i][j-1] + grid[0][i-1][j] + grid[0][i][j+1] + grid[0][i+1][j]);
				}
			}

		}	
		///////////////////////create the grid as required (end) //////////////////////////

		if(myid > 0)
		{
			MPI_Recv(grid[0][lower_ydim], (upper_ydim - lower_ydim + 1)*xdim, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);

			for (i = lower_ydim + 1; i < upper_ydim; i++)
			{
				for (j = 1; j < xdim - 1 ; j++)
				{
					//calculating the value and storing it in the new grid
					grid[1][i][j] = 0.2 * (grid[0][i][j] + grid[0][i][j-1] + grid[0][i-1][j] + grid[0][i][j+1] + grid[0][i+1][j]);
				}
			}

			MPI_Send(grid[1][lower_ydim+1], (upper_ydim-lower_ydim)*xdim, MPI_INT, 0, 99, MPI_COMM_WORLD);	
		}

		if(myid == 0)
		{
			for(i = 1; i < numprocs; i++)
			{
				lower_ydim = ((i * (ydim - 2))/numprocs);
				upper_ydim = (((i + 1)* (ydim - 2))/numprocs) + 1;	
				MPI_Recv(grid[1][lower_ydim + 1], (upper_ydim-lower_ydim)*xdim, MPI_INT, i, 99, MPI_COMM_WORLD, &status);
				/*for(j = lower_ydim + 1 ;j<upper_ydim; j++){
					for( k = 1; k < xdim - 1; k++)
					{
						grid[1][k][j] = grid[0][k][j];
					}
				}*/
			}

			temp_grid = grid[0];
			grid[0] = grid[1];
			grid[1] = temp_grid;
		}
	}
	if (myid == 0)
	{
		endwtime = MPI_Wtime();
		//printGrid(grid[0], xdim, ydim);
		printf("wall clock time = %f\n",
				endwtime-startwtime);           
	}

	//Do the time calcuclation
	//printf("Total Execution time: %lld ns\n", getTimerNs(&timer));
	MPI_Finalize();

	// Free the memory we allocated for grid
	free(temp);
	free(other_temp);
	free(grid[0]);
	free(grid[1]);

	return EXIT_SUCCESS;
}
