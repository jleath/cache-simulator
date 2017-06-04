/*
 * instruction_reader.h
 * a type and function prototype for reading and storing valgrind
 * reference instruction data.
 * Author: Josh Leath
 * Last Updated: 6/4/17
 */
#ifndef INSTRUCTION_READER_H
#define INSTRUCTION_READER_H
#include <stdio.h>
#include <inttypes.h>

/* A type for an instruction read from a valgrind reference file. */
typedef struct {
    /* a letter indicating the operation. */
    char op;
    /* a 64 bit memory address */
    uint64_t address;
    /* The number of bytes the operation should act on. */ 
    unsigned size;
} instruction;

/* 
 * Read an instruction from file and store the details in inst.
 * returns nonzero if an instruction was read successfully, else returns 0
 */ 
int read_instruction(FILE* file, instruction* inst);

#endif