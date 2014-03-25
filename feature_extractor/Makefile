LIBS=-lfftw3f -lfftw3 -lsndfile -lm 
SRC=src/voice_control.cpp
CFLAGS=-Wall -Iinclude/ -O3 -std=c++11 

.PHONY: all clean plot

all: extraction mfcc

extraction: support.o processing.o
	$(CXX) $(CFLAGS) src/extraction.cpp lib/support.o lib/processing.o $(LIBS) -o bin/extraction

mfcc: support.o processing.o
	$(CXX) $(CFLAGS) -c src/mfcc.cpp $(LIBS) -o lib/mfcc
processing.o: src/processing.cpp include/processing.hpp
	$(CXX) $(CFLAGS) -c src/processing.cpp $(LIBS) -o lib/processing.o
support.o: src/support.cpp include/support.hpp
	$(CXX) $(CFLAGS) -c src/support.cpp -lsndfile -o lib/support.o

plot:
	gnuplot -p -e 'plot "-"'

clean:
	rm lib/*
	rm *.data
