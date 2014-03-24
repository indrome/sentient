CFLAGS= -std=c++11 -Iinclude/ -O3
LDFLAGS=-pthread -lsndfile

.PHONY: all clean

all: server

server: src/server.cpp 
	g++ $(CFLAGS) src/server.cpp $(LDFLAGS) -o bin/server
clean:
	rm -f bin/*
	rm -f lib/*
