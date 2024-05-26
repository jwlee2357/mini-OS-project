#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

Process createDefaultProcess();

void round_robin(Process *processes, int num_processes, int quantum, char (*gantt_chart)[100], int *count);
void fcfs(Process *processes, int num_processes, char (*gantt_chart)[100], int *count);
void sjf(Process *processes, int num_processes, char (*gantt_chart)[100], int *count);
void srtf(Process *processes, int num_processes, char (*gantt_chart)[100], int *count);
void priority_scheduling(Process *processes, int num_processes, char (*gantt_chart)[100], int *count);

int schedule();

#endif
