/*
 * instruction_reader.c
 * Author: Josh Leath
 * Last Updated: 6/5/17
 */
#include "../include/instruction_reader.h"
#include <inttypes.h>
#include <stdio.h>

/* 
 * Reads a valgrind reference instruction in the format
 * [op] [address],[size]
 * Returns 0 if a valid instruction was not found, otherwise
 * returns 1.
 */
int read_instruction(FILE* file, instruction* inst)
{
    /* move to the beginning of an instruction */
    int first_char = fgetc(file);    
    /* Return 0 if we didn't find a valid instruction. */
    if (first_char == EOF) {
        return 0;
    }
    while (first_char != ' ') {
        while ((first_char = fgetc(file)) != '\n')
            ;
        first_char = fgetc(file);
    }
    
    int op = fgetc(file);
    uint64_t addr;
    unsigned size;
    fscanf(file, "%" PRIx64 "", &addr);
    fscanf(file, ",%x", &size);
    
    /* Read to the beginning of the next instruction */
    while (first_char != '\n' && first_char != EOF) {
        first_char = fgetc(file);
    }
    
    inst->op = op;
    inst->address = addr;
    inst->size = size;
    return 1;
}