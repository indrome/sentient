LIBS=-lfftw3f -lfftw3 -lsndfile -lm 
SRC=src/voice_control.cpp
CFLAGS=-Wall -Iinclude/ -O3 

.PHONY: clean plot

all: extraction 
	@echo "DONE"

extraction: support.o processing.o
	g++ $(CFLAGS) src/extraction.cpp lib/support.o lib/processing.o $(LIBS) -o bin/extraction

processing.o: src/processing.cpp include/processing.hpp
	g++ $(CFLAGS) -c src/processing.cpp $(LIBS) -o lib/processing.o


support.o: src/support.cpp include/support.hpp
	g++ $(CFLAGS) -c src/support.cpp -lsndfile -o lib/support.o

plot:
	gnuplot -p -e 'plot "-"'

clean:
	rm lib/*
	rm *.data
