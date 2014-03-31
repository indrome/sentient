LDFLAGS=-lfftw3f -lfftw3 -lsndfile feature_extractor/lib/processing.o  feature_extractor/lib/mfcc.o feature_extractor/lib/support.o
CFLAGS=-std=c++11 -Iserver/include -Lfeature_extractor/lib

.PHONY: server feature_extractor recognizer

all: feature_extractor recognizer server
	mkdir -p bin
	ln -sf $(PWD)/server/bin/server $(PWD)/bin/sentient
	@echo "Finished build"
	
cortex: feature_extractor recognizer
	g++ $(CFLAGS) src/cortex.cpp $(LDFLAGS) -o bin/sentient

server: 
	$(MAKE) -C ./server
feature_extractor:
	$(MAKE) -C ./feature_extractor
recognizer:
	$(MAKE) -C ./recognizer/dtw
