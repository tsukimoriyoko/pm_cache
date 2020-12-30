.PHONY: all clean
.DEFAULT_GOAL := all

LIBS=-lrt -lm -lpthread -lvmem -lpmem 
INCLUDES=-I./include
CFLAGS=-O3 -std=c++11 

all: pmtest

pmtest:
	g++ $(CFLAGS) -o pm_test test.cpp -lvmem -lpmem 

clean: 
	rm pm_test
