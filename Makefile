#
# Student makefile for Cache Lab
# Note: requires a 64-bit x86-64 system 
#
CC = gcc
CFLAGS = -g -Wall -Werror -std=c99 -m64

all: csim test-trans tracegen

csim: src/csim.c cachelab cache_simulator args_reader instruction_reader
	$(CC) $(CFLAGS) -pg -o csim bin/instruction_reader.o bin/cache_simulator.o bin/cachelab.o bin/args_reader.o src/csim.c -lm

test-trans: src/test-trans.c trans cachelab
	$(CC) $(CFLAGS) -o test-trans src/test-trans.c src/cachelab.c bin/trans.o 

tracegen: src/tracegen.c trans cachelab
	$(CC) $(CFLAGS) -O0 -o tracegen src/tracegen.c bin/trans.o src/cachelab.c

trans: src/trans.c
	$(CC) $(CFLAGS) -O0 -o bin/trans.o -c src/trans.c

args_reader: src/args_reader.c include/args_reader.h
	$(CC) $(CFLAGS) -pg -O0 -o bin/args_reader.o -c src/args_reader.c

cache_simulator: src/cache_simulator.c include/cache_simulator.h
	$(CC) $(CFLAGS) -pg -O0 -o bin/cache_simulator.o -c src/cache_simulator.c

cachelab: src/cachelab.c include/cachelab.h
	$(CC) $(CFLAGS) -pg -o bin/cachelab.o -c src/cachelab.c

instruction_reader: src/instruction_reader.c include/instruction_reader.h
	$(CC) $(CFLAGS) -pg -o bin/instruction_reader.o -c src/instruction_reader.c

#
# Clean the src dirctory
#
clean:
	rm -rf bin/*.o
	rm -f *.tar
	rm -f csim
	rm -f test-trans tracegen
	rm -f trace.all trace.f*
	rm -f .csim_results .marker
	rm -f gmon.out
