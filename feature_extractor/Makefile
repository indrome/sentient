LIBS=-lfftw3f -lfftw3 -lsndfile -lm 
SRC=src/voice_control.cpp
CFLAGS=-Wall -Iinclude/ -O3 -std=c++11 

.PHONY: all clean plot

all: lib/mfcc.o lib/processing.o lib/support.o

extraction: lib/support.o lib/processing.o
	mkdir -p bin
	$(CXX) $(CFLAGS) src/extraction.cpp lib/support.o lib/processing.o $(LIBS) -o bin/extraction

lib/mfcc.o: src/mfcc.cpp 
	$(CXX) $(CFLAGS) -c src/mfcc.cpp $(LIBS) -o lib/mfcc.o
processing: src/processing.cpp include/processing.hpp
	$(CXX) $(CFLAGS) -c src/processing.cpp $(LIBS) -o lib/processing.o
support: src/support.cpp include/support.hpp
	$(CXX) $(CFLAGS) -c src/support.cpp -lsndfile -o lib/support.o

plot:
	gnuplot -p -e 'plot "-"'

clean:
	rm -rf lib/*
	rm *.data
	rm -rf bin/
