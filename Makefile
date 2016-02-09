CXX= gcc
CXXFLAGS=
LDFLAGS = -lm -lpthread 

all: serial_ocean omp_ocean

pthreads_ocean: pthreads_ocean_main.c
	$(CXX) $(CXXFLAGS) pthreads_ocean_main.c -o pthreads_ocean $(LDFLAGS)

clean:
	rm -f pthreads_ocean
