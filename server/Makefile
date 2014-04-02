CFLAGS= -std=c++11 -Iinclude/ -I../feature_extractor/include  -O3
LDFLAGS=-pthread -lsndfile ../feature_extractor/lib/*.o -lfftw3 -lfftw3f

.PHONY: all clean

all: lib/json_parser.o bin/server

bin/server: lib/json_parser.o src/server.cpp include/server.hpp 
	g++ $(CFLAGS) -I../feature_extractor/include src/server.cpp lib/json_parser.o $(LDFLAGS) -o bin/server

lib/json_parser.o: src/json_parser.cpp include/json_parser.hpp
	g++ $(CFLAGS) src/json_parser.cpp -c $(LDFLAGS) -o lib/json_parser.o

clean:
	rm -f bin/*
	rm -f lib/*
