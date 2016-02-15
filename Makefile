MCC= /usr/bin/mpiCC
CXX= gcc
CXXFLAGS=
LDFLAGS = -lm -lpthread 
OPTFLAGS =
MCCFLAGS=

all: pthreads_ocean mpi_ocean_main

pthreads_ocean: pthreads_ocean.c
		$(CXX) $(CXXFLAGS) pthreads_ocean_main.c pthreads_ocean.c -o pthreads_ocean $(LDFLAGS)

mpi_ocean_main: mpi_ocean_main.c
		$(MCC) $(OPTFLAGS) -o mpi_ocean_main mpi_ocean_main.c -lm

clean:
	rm -f pthreads_ocean
	/bin/rm -f *.o *~ mpi_ocean_main

