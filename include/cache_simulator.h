/*
 * cache_simulator.h
 *
 * Data types and function prototypes for a cache simulator.
 * This simulates a cache that contains an arbitrary number of
 * cache sets, each with an arbitrary number of lines with an
 * arbitrary block size. These vales are passed into the build_simulator
 * function, which returns the simulator. The simulator tracks the number
 * of cache hits, cache misses, and cache evictions for a given set of instructions
 * which are given to the simulator via the check_cache function.
 * 
 * Author: Josh Leath
 * Last updated: 6/4/17
 *
 * TODO: change the underlying lines array to a multidimensional array, this will
 *       clarify the relationship between sets and lines.
 * TODO: Distinguish between different operations, currently the program operates
 *       the same for loads, stores and modifys.
 */

#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H
#include <stdbool.h>
#include <inttypes.h>

typedef enum { CACHE_HIT, CACHE_EVICTION, CACHE_MISS } op_state;

/*
 * A simple type for a single line in a cache set
 */
typedef struct {
    /* The tag of the block stored in this line */
    unsigned tag;
    /* A flag indicating whether the block in this line is meaningful */
    bool valid_bit;
    /* 
     * The number of the last instruction to touch this line. Used
     * for the LRU strategy of dealing with cache evictions.
     */
    int last_instruction;
} line;

/*
 * A type for the simulator. Contains an array of line objects
 * and various information about the state of the cache.
 */
typedef struct {
    /* The lines in the cache. */
    line* lines;
    /* The number of lines in each cache set. */
    int lines_per_set;
    /* various data about the performance of the cache. */
    int hit_count, miss_count, eviction_count;
    /* Information about how to partition addresses. */
    int tag_len, offset_len, index_len;
    /* The total number of lines in the cache. */
    int num_lines;
} cache_simulator;

/*
 * A type to store the partitions of an address
 */
typedef struct {
    unsigned tag, set_index, offset;
} address_info;

/*
 * Construct and return a cache simulator
 * b - the number of bits used to store the block offset.
 * s - the number of bits used to store the set index.
 * E - the number of lines per cache set.
 */
cache_simulator* build_simulator(int b, int s, int E);

/*
 * checks if the block containing the memory represented by addr is in the cache
 * updates the internal state of the cache based on the check. Returns an op_state
 * signalling whether this check resulted in a cache hit, miss or eviction.
 */
op_state check_cache(cache_simulator* cache, address_info* addr, int inst_no);

/*
 * Free the resources used to construct the cache simulator.
 */
void destroy_simulator(cache_simulator* cache);

/*
 * Fill in the data of an address_info object by partitioning the address.
 */
void get_address_info(uint64_t address, address_info* addr,
        cache_simulator* cache);

#endif
