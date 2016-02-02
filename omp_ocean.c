#include <stdio.h>
#include <omp.h>

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

}
