CXX= gcc
CXXFLAGS=
LDFLAGS = -lm -lpthread 

all: pthreads_ocean

pthreads_ocean: pthreads_ocean.c
	$(CXX) $(CXXFLAGS) pthreads_ocean_main.c pthreads_ocean.c -o pthreads_ocean $(LDFLAGS)

clean:
	rm -f pthreads_ocean
