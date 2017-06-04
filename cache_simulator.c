#include "cache_simulator.h"
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#define WORD_SIZE 64

int lru(cache_simulator*, int set_index, int num_lines);
void update_line(cache_simulator*, int, unsigned, int);
bool equal_tags(cache_simulator*, int, unsigned);
bool is_valid(cache_simulator* cache, int line_index);
unsigned extract_tag(uint64_t address, int tag_len, int offset_len, int index_len);
unsigned extract_block_offset(uint64_t address, int offset_len);
void get_address_info(uint64_t, address_info*, cache_simulator*);


cache_simulator* build_simulator(int b, int s, int e)
{
    int total_num_lines = s * s * e;
    cache_simulator* cache = malloc(sizeof(cache_simulator));
    cache->lines = calloc(total_num_lines, sizeof(line));
    if (! cache->lines) {
        free(cache);
        return NULL;
    }
    cache->lines_per_set = e;
    cache->hit_count = cache->miss_count = cache->eviction_count = 0;
    cache->tag_len = WORD_SIZE - (s + b);
    cache->offset_len = b;
    cache->index_len = s;
    cache->num_lines = total_num_lines;
}

void destroy_simulator(cache_simulator* cache)
{
    free(cache->lines);
    free(cache);
}

op_state check_cache(cache_simulator* cache, address_info* addr, int inst_no)
{
    int lines_per_set = cache->lines_per_set;
    /* get the index of the first line in the correct set */
    int start_of_set = (addr->set_index * lines_per_set);
    /* get the index of the last line in the correct set */
    int end_of_set = (start_of_set + lines_per_set);
    unsigned tag = addr->tag;
    int open_line = -1;

    for (int curr_line = start_of_set; curr_line < end_of_set; ++curr_line) {
        if (equal_tags(cache, curr_line, tag) 
                && is_valid(cache, curr_line)) {
            /* cache hit */
            update_line(cache, curr_line, tag, inst_no);
            cache->hit_count += 1;
            return CACHE_HIT;
        } else if (! is_valid(cache, curr_line))
            /* remember the index of an open line */
            open_line = curr_line;
    }
    /* cache miss */
    cache->miss_count += 1;

    /* updated the open line we found if we found one */
    if (open_line != -1) {
        update_line(cache, open_line, tag, inst_no);
    } else {
        /* otherwise we have to replace an existing line */
        cache->eviction_count += 1;
        int line_to_replace = lru(cache, addr->set_index, lines_per_set);
        update_line(cache, line_to_replace, tag, inst_no);
        return CACHE_EVICTION;
    }
    return CACHE_MISS;
}

int lru(cache_simulator* cache, int set_index, int num_lines)
{
    line* curr = &(cache->lines[set_index*num_lines]);
    int smallest = curr->last_instruction;
    int index_of_smallest = 0;
    curr++;
    for (int i = 1; i < num_lines; ++i) {
        int curr_value = curr->last_instruction;
        if (curr_value < smallest) {
            smallest = curr_value;
            index_of_smallest = i;
        }
        curr++;
    }
    return (set_index*num_lines) + index_of_smallest;
}

void update_line(cache_simulator* cache, int line_index, unsigned tag, int inst_no)
{
    line* line_to_update = &(cache->lines[line_index]);
    line_to_update->tag = tag;
    line_to_update->valid_bit = true;
    line_to_update->last_instruction = inst_no;
}

bool equal_tags(cache_simulator* cache, int line_index, unsigned tag)
{
    return cache->lines[line_index].tag == tag;
}

bool is_valid(cache_simulator* cache, int line_index)
{
    return cache->lines[line_index].valid_bit;
}

unsigned extract_tag(uint64_t address, int tag_len, int offset_len, int index_len)
{
    unsigned mask = ~(-1 << tag_len);
    return mask & (address >> (offset_len + index_len));
}

unsigned extract_setindex(uint64_t address, int offset_len, int index_len)
{
    unsigned mask = ~(-1 << index_len);
    return mask & (address >> offset_len);
}

unsigned extract_block_offset(uint64_t address, int offset_len)
{
    return (~(-1 << offset_len)) & address;
}

void get_address_info(uint64_t address, address_info* addr,
        cache_simulator* cache)
{
    /* get tag bits */
    //printf("%" PRIx64 "\n", address);
    unsigned tag = extract_tag(address, cache->tag_len, 
            cache->offset_len, cache->index_len);
    //printf("%x ", tag);
    /* get index bits */
    unsigned set_index = extract_setindex(address, cache->offset_len,
                                          cache->index_len);
    //printf("%x ", set_index);
    /* get offset bits */
    unsigned offset = extract_block_offset(address, cache->offset_len);
    //printf("%x\n", offset);
    addr->tag = tag;
    addr->set_index = set_index;
    addr->offset = offset;
}
