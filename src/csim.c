/*
 * csim.c
 * Author: Josh Leath
 * Last Updated: 6/4/17
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
    op_state result1, result2;
    instruction instr;
    address_info addr;
    int s, b, tag_len;
    s = args.s;
    b = args.b;
    tag_len = cache->tag_len;
    int tag_mask = ~(-1 << tag_len);
    int offset_mask = ~(-1 << s);
    int other_bits = b+s;
    for (int inst_no = 0; read_instruction(ref_file, &instr); inst_no++) {
        /* Fill address_info */
        uint64_t address = instr.address;
        int op = instr.op;
        addr.tag = (address >> other_bits) & tag_mask;
        addr.set_index = (address >> b) & offset_mask;
        addr.offset = address & offset_mask;
        
        /* If the op is a modify, we know that the second cache check with be a hit. */
        if (op == 'M') {
            result2 = CACHE_HIT;
            inst_no += 1;
            cache->hit_count += 1;
        }
        
        /* test the cache */
        result1 = check_cache(cache, &addr, inst_no);
        
        /* Print results */
        if (args.verbose) {
            printf("%c ", op);
            printf("%" PRIx64 , address);
            printf(",%x", instr.size);
            print_result(result1);
            if (op == 'M')
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
