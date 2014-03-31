CFLAGS=-std=c++11 -Wall -O3 -Iinclude/

all: lib/dtw.o bin/compute_dtw

lib/dtw.o: src/dtw.cpp include/dtw.hpp
	g++ $(CFLAGS) src/dtw.cpp -c -o lib/dtw.o -lm

bin/compute_dtw: lib/dtw.o src/compute_dtw.cpp include/dtw.hpp 
	g++ $(CFLAGS) src/compute_dtw.cpp lib/dtw.o -o bin/compute_dtw 

clean:
	rm -f bin/compute_dtw
	rm -f lib/*.o


