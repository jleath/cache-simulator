/*
 * instruction_reader.c
 * Author: Josh Leath
 * Last Updated: 6/4/17
 */
#include "../include/instruction_reader.h"
#include <inttypes.h>
#include <stdio.h>

int read_instruction(FILE* file, instruction* inst)
{
    int first_char = fgetc(file);    
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
    
    while (first_char != '\n' && first_char != EOF) {
        first_char = fgetc(file);
    }
    
    inst->op = op;
    inst->address = addr;
    inst->size = size;
    return 1;
}