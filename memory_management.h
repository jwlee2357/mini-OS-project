#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "scheduling.h"

#define PARTITION_SIZE 1024
#define MAX_MEM_BLOCKS 20

typedef struct {
    void *addr; 
    size_t size;
    int block_id; //id
    int in_use_flag; //사용중
    Process process;
} MemoryBlock;

typedef struct {
    int start_idx; //여유 공간의 시작 인덱스
    size_t free_size; //연속적인 여유 블록의 수
} FreeSpace;

void init_partitions();
void* fixed_allocate_memory();
void fixed_free_memory(void *memory);
void print_memory_blocks();
int dyna_alloc(size_t size, Process process);
void dyna_free(int free_id);
void update_free_spaces();

#endif