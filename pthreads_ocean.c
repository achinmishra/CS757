#include <stdio.h>

typedef struct _payload
{
	int **grid[2];
	int xdim;
	int lower_ydim;
	int upper_ydim;
}payload;

void *ocean (void *arguments)
{

	/********************************************************
	 * algorithm
	 *
	 * Two grids are passed in through the grid argument.
	 * Each grid itself is an int**, which is a pointer to
	 * an array of pointers (see the initialization code).
	 *
	 * Iterate over the grid[0], performing the Ocean
	 * algorithm (described in wiki). Write the result to
	 * the other grid. Then, swap the grids for the next
	 * iteration.
	 ******************************************************/
	payload* args = (payload*)arguments;

	int i,j;
	int **temp_grid;

	for (i = args->lower_ydim; i <= args->upper_ydim; i++)
	{
		for (j = 1; j < args->xdim - 1 ; j++)
		{
			//calculating the value and storing it in the new grid
			args->grid[1][j][i] = 0.2 * (args->grid[0][j][i] + args->grid[0][j][i-1] + args->grid[0][j-1][i] + args->grid[0][j][i+1] + args->grid[0][j+1][i]);
		}
	}
}
