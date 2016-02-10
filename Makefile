CXX= gcc
CXXFLAGS=
LDFLAGS = -lm -lpthread 

all: serial_ocean

serial_ocean: serial_ocean.c
	$(CXX) $(CXXFLAGS) pthreads_ocean_main.c serial_ocean.c -o pthreads_ocean $(LDFLAGS)

clean:
	rm -f pthreads_ocean
