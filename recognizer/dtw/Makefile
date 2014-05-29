CC		=g++
SHELL 	=/bin/sh

FLAGS			=-std=c++11 -Iinclude/
CFLAGS			=-Wall
DEBUGFLAGS		=-O0 -g -D _DEBUG
RELEASEFLAGS 	=-O3 -D NDEBUG

LDFLAGS		=-L/usr/local/lib/ -lm

SRCS	=$(shell echo src/*.cpp)
OBJS 	= $(addprefix lib/,$(notdir $(SRCS:.cpp=.o)))

COMMON	=./include/dtw.hpp
TARGET	=json_dtw

CPUPROFILE?=/tmp/prof.out
LIBPROFILER_PATH=/usr/local/lib/libprofiler.so.0

all: $(TARGET) 
.PHONY: profile_run clean

$(TARGET) : $(OBJS) $(COMMON)	
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) $(LDFLAGS) -o ./bin/$(TARGET) $(SRCS)

release: $(OBJS) $(COMMON)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) $(LDFLAGS) -o ./bin/$(TARGET) $(SRCS)

lib/%.o : ./src/%.cpp $(HEADERS) $(COMMON)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -c -o $@ $<


profile_run: CPUPROFILE_FREQUENCY=1000
profile_run: 
	CPUPROFILE_FREQUENCY=1000 LD_PRELOAD=/usr/local/lib/libprofiler.so.0 ./bin/json_dtw samples/play.json samples/*
	#pprof --text ./bin/$(TARGET) | tee /tmp/profile.txt 

profile: LDFLAGS+=-L/usr/local/lib/ -lprofiler
profile: FLAGS+=-D _PROFILE

profile: $(TARGET) 

clean:
	rm -f bin/*
	rm -f lib/*

