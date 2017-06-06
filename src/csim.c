/*
 * csim.c
 * Author: Josh Leath
 * Last Updated: 6/5/17
 */
#include "../include/cachelab.h"
#include "../include/args_reader.h"
#include "../include/cache_simulator.h"
#include "../include/instruction_reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

/* outputs the results of an operation */
void print_result(op_state state);
void partition_address(address_info* addr, int tag_len, int index_len,
                       int offset_len, uint64_t address);

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
        printf("Unable to allocate memory for "
               "cache simulator -- aborting.\n\n");
        return EXIT_FAILURE;
    }

    // read through the instructions and process them
    op_state result1, result2;
    instruction instr;
    address_info addr;
    
    for (int inst_no = 0; read_instruction(ref_file, &instr); inst_no++) {
        /* Fill address_info */
        partition_address(&addr, cache->tag_len, args.s, args.b, instr.address);
        
        /* If the op is a modify, we know that the second cache check with be a hit. */
        if (instr.op == 'M') {
            result2 = CACHE_HIT;
            inst_no += 1;
            cache->hit_count += 1;
        }
        
        /* test the cache */
        result1 = check_cache(cache, &addr, inst_no);
        
        /* Print results */
        if (args.verbose) {
            printf("%c ", instr.op);
            printf("%" PRIx64 , instr.address);
            printf(",%x", instr.size);
            print_result(result1);
            if (instr.op == 'M')
                print_result(result2);
            printf("\n");
        }
    }
    fclose(ref_file);
    // print the results
    printSummary(cache->hit_count, cache->miss_count, cache->eviction_count);
    destroy_simulator(cache);

    return EXIT_SUCCESS;
}

/* Extract the address tag, offset index, and offset. */
void partition_address(address_info* addr, int tag_len, int index_len,
                       int offset_len, uint64_t address)
{
    int tag_mask = ~(-1 << (tag_len));
    int offset_mask = ~(-1 << index_len);
    /* Get the tag bits from address */
    addr->tag = (address >> (offset_len + index_len)) & tag_mask;
    /* Get the set index bits from address */
    addr->set_index = (address >> offset_len) & offset_mask;
    /* Get the byte offset bits from address */
    addr->offset = address & offset_mask;
}

/* Prints whether a cache op resulted in a hit, miss or eviction. */
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
