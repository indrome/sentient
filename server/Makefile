CFLAGS= -std=c++11 -Iinclude/ -ggdb
LDFLAGS=-pthread -lsndfile

.PHONY: clean

all: server
	@echo "Build complete"


server: src/server.cpp 
	g++ $(CFLAGS) src/server.cpp $(LDFLAGS) -o bin/server

clean:
	rm -f bin/*
	rm -f lib/*
