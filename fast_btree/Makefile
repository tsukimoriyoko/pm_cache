.PHONY: all clean
.DEFAULT_GOAL := all

LIBS=-lrt -lm -lpthread -lvmem -lpmem 
INCLUDES=-I./include
CFLAGS=-O3 -std=c++11 

all: btree

btree: 
	g++ $(CFLAGS) -o btree_ycsb ycsb.cpp -lrt -lm -lpthread -lvmem -lpmem  -DCONCURRENT 

clean: 
	rm btree_ycsb
