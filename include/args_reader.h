/*
 * Utility functions for reading the command line arguments to the
 * csim program
 * Author - Joshua Leath
 * Last updated - 6/3/17
 */

#ifndef ARGS_READER_H
#define ARGS_READER_H
#include <stdbool.h>

#define OPT_STR "hvs:b:E:t:"
#define USAGE_STR "Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>"

/* 
 * A data structure to store the args passed in on the command line.
 *  s - the number of bits used for the set index in the cache simulator
 *  b - the number of bits used for the block offest in the cache simulator
 *  E - the number of lines in a set in the cache simulator
 *  verbose - should the program print verbose output
 *  ref_filename - the path to the valgrind reference input file
 */
typedef struct {
    int s, b, E;
    bool verbose;
    char* ref_filename;
} program_args;

/* populate the fileds in ARGS with values from the command line */
int get_args(program_args* args, int argc, char** argv);

/* print useful info to the user if their input was not correct */
void report_failure(void);

#endif
