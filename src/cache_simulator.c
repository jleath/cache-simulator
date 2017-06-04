/*
 * cache_simulator.c
 * Author: Josh Leath
 * Last Updated: 6/4/17
 */
#include "../include/cache_simulator.h"
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

/* The size of addresses on this machine. */
#define WORD_SIZE 64

/* Return the number of the line that was least recently used. */
line* lru(cache_simulator* cache, int set_index, int num_lines);
/* Set the tag, valid bit, and instruction number values of a line. */
void update_line(line* line_to_update, unsigned tag, int inst_no);
/* Return the unsigned value representing the tag from address. */
unsigned extract_tag(uint64_t address, int tag_len, int offset_len, int index_len);
/* Return the unsigned value representing the block offset from address. */
unsigned extract_block_offset(uint64_t address, int offset_len);
/* Return the unsigned value representing the set index from the address. */
unsigned extract_setindex(uint64_t address, int offset_len, int index_len);
int get_line_index(int set_index, int line_no, int lines_per_set);
line* get_line(cache_simulator* cache, int line_no);

int get_line_index(int set_index, int line_no, int lines_per_set)
{
    return (set_index * lines_per_set) + line_no;
}

line* get_line(cache_simulator* cache, int line_no)
{
    return &(cache->lines[line_no]);
}

cache_simulator* build_simulator(int b, int s, int e)
{
    int total_num_lines = (int) pow(2, s) * e;
    cache_simulator* cache = malloc(sizeof(cache_simulator));
    if (cache == NULL) {
        return NULL;
    }
    line* lines = calloc(total_num_lines, sizeof(line));
    if (lines == NULL) {
        free(cache);
        return NULL;
    }
    cache->lines = lines;
    cache->lines_per_set = e;
    cache->hit_count = cache->miss_count = cache->eviction_count = 0;
    cache->tag_len = WORD_SIZE - (s + b);
    cache->offset_len = b;
    cache->index_len = s;
    cache->num_lines = total_num_lines;
    return cache;
}

void destroy_simulator(cache_simulator* cache)
{
    free(cache->lines);
    free(cache);
}

op_state check_cache(cache_simulator* cache, address_info* addr, int inst_no)
{
    int lines_per_set = cache->lines_per_set;
    int set_index = addr->set_index;
    //int start_of_set = get_line_index(set_index, 0, lines_per_set);
    //int end_of_set = get_line_index(set_index, lines_per_set, lines_per_set);
    unsigned tag = addr->tag;
    line* open_line = NULL;

    for (int i = 0; i < lines_per_set; ++i) {
        int line_no = get_line_index(set_index, i, lines_per_set);
        line* curr_line = get_line(cache, line_no);
        bool equal_tags = curr_line->tag == tag;
        bool valid_line = curr_line->valid_bit;
        if (equal_tags && valid_line) {
            /* cache hit */
            update_line(curr_line, tag, inst_no);
            cache->hit_count += 1;
            return CACHE_HIT;
        } else if (! valid_line)
            /* remember the index of an open line */
            open_line = curr_line;
    }
    /* cache miss */
    cache->miss_count += 1;

    /* updated the open line we found if we found one */
    if (open_line != NULL) {
        update_line(open_line, tag, inst_no);
    } else {
        /* otherwise we have to replace an existing line */
        cache->eviction_count += 1;
        line* line_to_replace = lru(cache, addr->set_index, lines_per_set);
        update_line(line_to_replace, tag, inst_no);
        return CACHE_EVICTION;
    }
    return CACHE_MISS;
}

line* lru(cache_simulator* cache, int set_index, int num_lines)
{
    line* curr = get_line(cache, get_line_index(set_index, 0, num_lines));
    int smallest = curr->last_instruction;
    line* result = curr;
    curr++;
    for (int i = 1; i < num_lines; ++i) {
        int curr_value = curr->last_instruction;
        if (curr_value < smallest) {
            smallest = curr_value;
            result = curr;
        }
        curr++;
    }
    return result;
}

void update_line(line* line_to_update, unsigned tag, int inst_no)
{
    line_to_update->tag = tag;
    line_to_update->valid_bit = true;
    line_to_update->last_instruction = inst_no;
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
