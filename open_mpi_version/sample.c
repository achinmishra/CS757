#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define BUFLEN 512

int main(int argc, char* argv[])
{
	int i, j, myid, numprocs, next, rc, namelen;
	char buffer[BUFLEN], processor_name[MPI_MAX_PROCESSOR_NAME];
	double startwtime, endwtime;
	int** temp_grid;

	MPI_Status status;

	/* Initialize the MPI execution environment */
	MPI_Init(&argc,&argv);

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

	lower_ydim = ((myid * (ydim - 2))/numprocs) + 1;
	upper_ydim = (((myid + 1)* (ydim - 2))/numprocs);
	
	while(timesteps-- >=0)
	{
		for (i = lower_ydim; i <= upper_ydim; i++)
		{
			for (j = 1; j < xdim - 1 ; j++)
			{
				//calculating the value and storing it in the new grid
				grid[1][j][i] = 0.2 * (grid[0][j][i] + grid[0][j][i-1] + grid[0][j-1][i] + grid[0][j][i+1] + grid[0][j+1][i]);
			}
		}
		temp_grid = grid[0];
		grid[0] = grid[1];
		grid[1] = temp_grid;
	}

	if (myid == 0)
	{
		printf("%d sending '%s' \n",myid,buffer);

		/* 
		 * Returns an elapsed time on the calling processor 
		 */
		startwtime = MPI_Wtime();

		/* 
		 * Performs a basic send 
		 *
		 *   int MPI_Send( buf, count, datatype, dest, tag, comm )
		 *   void  *buf; //  initial address of send buffer (choice) 
		 *   int  count; // number of elements in send buffer 
		 *               // (nonnegative integer)
		 *   int  dest;  //  rank of destination (integer) 
		 *   MPI_Datatype  datatype; // datatype of each send buffer 
		 *                           // element (handle) 
		 *   int  tag; //  message tag (integer) 
		 *   MPI_Comm  comm; //communicator (handle) 
		 */
		MPI_Send(buffer, strlen(buffer)+1, MPI_CHAR, next, 99, MPI_COMM_WORLD);

		printf("%d receiving \n",myid);

		/* 
		 * Basic receive 
		 *
		 *   int MPI_Recv( buf, count, datatype, source, tag, comm, status )
		 *   void  *buf; //initial address of receive buffer (choice) 
		 *   int count; // maximum number of elements in receive buffer 
		 *              // (integer) 
		 *   MPI_Datatype   datatype; //  datatype of each receive buffer 
		 *                            // element (handle) 
		 *   int  source; // rank of source (integer)
		 *   int  tag; // message tag (integer) 
		 *   MPI_Comm  comm; // communicator (handle)
		 *   MPI_Status       *status; // status object (Status) 
		 */
		MPI_Recv(buffer, BUFLEN, MPI_CHAR, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
				&status);
		printf("%d received '%s' \n",myid,buffer);
	}
	else
	{
		printf("%d receiving  \n",myid);
		MPI_Recv(buffer, BUFLEN, MPI_CHAR, MPI_ANY_SOURCE, 99, MPI_COMM_WORLD,
				&status);
		printf("%d received '%s' \n",myid,buffer);
		MPI_Send(buffer, strlen(buffer)+1, MPI_CHAR, next, 99, MPI_COMM_WORLD);
		printf("%d sent '%s' \n",myid,buffer);
	}

	/* 
	 * Blocks until all process have reached this routine. 
	 *
	 *   int MPI_Barrier ( comm )
	 *   MPI_Comm comm;  //  communicator (handle) 
	 */
	MPI_Barrier(MPI_COMM_WORLD);

	if (myid == 0)
	{
		endwtime = MPI_Wtime();
		printf("wall clock time = %f\n",
				endwtime-startwtime);           
	}

	/*
	 * Terminates MPI execution environment
	 *
	 *   int MPI_Finalize()
	 */
	MPI_Finalize();
}
