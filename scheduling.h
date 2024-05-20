#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;
    int priority;
    int burst_time;
    int remaining_time;
    int arrival_time;
    int completion_time;
    int mem_block_id;  // 할당된 메모리 블록 ID
    size_t size;
} Process;


void round_robin(Process *processes, int num_processes, int quantum);
int schedule();

#endif
