CFLAGS=-std=c++11 -I/usr/local/include/aquila/
LDFLAGS=-L/usr/local/lib/ -lAquila
.PHONY: run

all: dtw
	@echo "DONE"

dtw: dtw.cpp
	g++ -Wall dtw.cpp $(CFLAGS) $(LDFLAGS) -o dtw 


run:
	./test.sh
