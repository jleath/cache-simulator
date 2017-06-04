#ifndef CACHE_SIMULATOR_H
#define CACHE_SIMULATOR_H
#include <stdbool.h>
#include <inttypes.h>

typedef enum { CACHE_HIT, CACHE_EVICTION, CACHE_MISS } op_state;

typedef struct {
    unsigned tag;
    bool valid_bit;
    int last_instruction;
} line;

typedef struct {
    line* lines;
    int lines_per_set;
    int hit_count, miss_count, eviction_count;
    int tag_len, offset_len, index_len;
    int num_lines;
} cache_simulator;

typedef struct {
    unsigned tag, set_index, offset;
} address_info;

cache_simulator* build_simulator(int b, int s, int E);
op_state check_cache(cache_simulator* cache, address_info* addr, int inst_no);
void destroy_simulator(cache_simulator* cache);
void get_address_info(uint64_t address, address_info* addr,
        cache_simulator* cache);

#endif
