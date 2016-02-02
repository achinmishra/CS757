CXX= gcc
CXXFLAGS=
LDFLAGS = -lm  

all: serial_ocean omp_ocean

serial_ocean: serial_ocean.c ocean_main.c
	$(CXX) $(CXXFLAGS) ocean_main.c serial_ocean.c -o serial_ocean $(LDFLAGS)

omp_ocean: omp_ocean.c ocean_main.c
	$(CXX) $(CXXFLAGS) -fopenmp omp_ocean.c ocean_main.c -o omp_ocean $(LDFLAGS)

clean:
	rm -f serial_ocean
	rm -f omp_ocean
