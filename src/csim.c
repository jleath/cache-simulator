/*
 * csim.c
 * Author: Josh Leath
 * Last Updated: 6/4/17
 */
#include "../include/cachelab.h"
#include "../include/args_reader.h"
#include "../include/cache_simulator.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

/* A type for an instruction read from a valgrind reference file. */
typedef struct {
    /* a letter indicating the operation. */
    char op;
    /* A 64 bit memory address. */
    uint64_t address;
    /* The number of bytes the operation should act on. */
    unsigned size;
} instruction;

/* 
 * Read an instruction from file and store the details in inst.
 * Returns nonzero if an instruction was read successfully, else returns 0.
 */
int read_instruction(FILE* file, instruction* inst);
/* outputs the results of an operation */
void print_result(op_state state);

int main(int argc, char** argv)
{
    // get command line arguments
    program_args args;
    args.s = args.b = args.E = 0;
    args.verbose = false;
    args.ref_filename = NULL;
    if (! get_args(&args, argc, argv)) {
        report_failure();
        return EXIT_FAILURE;
    }

    // load valgrind reference file
    FILE* ref_file = fopen(args.ref_filename, "r");
    if (! ref_file) {
        printf("ERROR: Failed to open reference file: %s\n", args.ref_filename);
        return EXIT_FAILURE;
    }

    // build cache
    cache_simulator* cache = build_simulator(args.b, args.s, args.E);
    if (! cache) {
        printf("Unable to allocate memory for cache simulator -- aborting.\n\n");
        return EXIT_FAILURE;
    }

    // read through the instructions and process them
    int inst_no = 0;
    op_state result1, result2;
    instruction instr;
    address_info addr;
    while (read_instruction(ref_file, &instr)) {
            get_address_info(instr.address, &addr, cache);
            if (args.verbose) {
                printf("%c ", instr.op);
                printf("%" PRIx64 , instr.address);
                printf(",%x", instr.size);
            }
            switch (instr.op) {
                /* Fall through */
                case 'L': case 'S':
                    result1 = check_cache(cache, &addr, inst_no);
                    if (args.verbose) {
                        print_result(result1);
                    }
                    break;
                case 'M':
                    result1 = check_cache(cache, &addr, inst_no);
                    if (args.verbose) {
                        print_result(result1);
                    }
                    result2 = check_cache(cache, &addr, inst_no);
                    if (args.verbose) {
                        print_result(result2);
                    }
                    break;
                default:
                    printf("Error in instruction: %d\n", inst_no);
                    printf("invalid op: %d\n", instr.op);
                    printf("Invalid operation in reference file -- aborting.\n");
                    return EXIT_FAILURE;
            }
        if (args.verbose && instr.op != 'I') 
            printf("\n");
        inst_no += 1;
    }

    fclose(ref_file);

    // print the results
    printSummary(cache->hit_count, cache->miss_count, cache->eviction_count);

    destroy_simulator(cache);

    return EXIT_SUCCESS;
}

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

void print_result(op_state state)
{
    switch (state) {
        case CACHE_HIT:
            printf(" hit");
            break;
        case CACHE_MISS:
            printf(" miss");
            break;
        case CACHE_EVICTION:
            printf(" miss eviction");
            break;
    }
}
