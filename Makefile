MCC= /usr/bin/mpiCC
CXX= gcc
CXXFLAGS=
LDFLAGS = -lm -lpthread 
OPTFLAGS =
MCCFLAGS=

all: pthreads_ocean_main

pthreads_ocean: pthreads_ocean.c
		$(CXX) $(CXXFLAGS) pthreads_ocean_main.c -o pthreads_ocean $(LDFLAGS)
clean:
	rm -f pthreads_ocean_main

