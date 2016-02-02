#include <stdio.h>

void ocean (int **grid[2], int xdim, int ydim, int timesteps)
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
	int i,j,temp;
	int **temp_grid;

	while (timesteps-- >= 0)
	{	
		for (i = 1; i < xdim - 1; i++)
		{
			for (j = 1; j < ydim - 1 ; j++)
			{
				//calculating the value and storing it in the new grid
				grid[1][i][j] = 0.2 * (grid[0][i][j] + grid[0][i][j-1] + grid[0][i-1][j] + grid[0][i][j+1] + grid[0][i+1][j]);
			}
		}
		//swap the grid now
		temp_grid = grid[0];
		grid[0] = grid[1];
		grid[1] = temp_grid;
	}
}
