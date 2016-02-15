##### User configurable options #####

CC          = /usr/bin/mpiCC
OPTFLAGS    = 
#OPTFLAGS    = -L/usr/lib64/openmpi 
#

### End User configurable options ###

CFLAGS	  = $(OPTFLAGS) 

mpi_ocean_main: mpi_ocean_main.o
	$(CC) $(OPTFLAGS) -o mpi_ocean_main mpi_ocean_main.o -lm

clean:
	/bin/rm -f *.o *~ mpi_ocean_main
.c.o:
	$(CC) $(CFLAGS) -c $*.c
