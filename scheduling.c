#include "memory_management.h"
#include "scheduling.h"

Process createDefaultProcess() {
    Process p = {
        .pid = 0,
        .priority = 0,
        .burst_time = 0,
        .remaining_time = 0,
        .arrival_time = 0,
        .completion_time = 0,
        .mem_block_id = -1,  // 예: -1로 초기화하여 메모리 블록 ID가 아직 할당되지 않았음을 표시
        .size = 0
    };
    return p;
}

int current_time = 0;

void round_robin(Process *processes, int num_processes, int quantum, char (*gantt_chart)[100], int *count) {
    int completed_processes = 0;
    char buffer[100];

    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].mem_block_id == 0) {  // 메모리 할당 시도
                    processes[i].mem_block_id = dyna_alloc(processes[i].size, processes[i]);
                    if (processes[i].mem_block_id == -1) {
                        printf("Memory allocation failed for Process %d\n", processes[i].pid);
                        continue;  // 메모리 할당 실패시 다음 기회에 시도
                    }
                }
                print_memory_blocks();
                int execute_time = (processes[i].remaining_time > quantum) ? quantum : processes[i].remaining_time;
                
                for (int t = 0; t < current_time; ++t) {
                    strcpy(gantt_chart[*count], "  ");
                    (*count)++;
                }
                
                for (int j = 0; j < execute_time; ++j) {
                    strcpy(gantt_chart[*count], "||");
                    (*count)++;
                }
                
                strcpy(gantt_chart[*count], "(");
                (*count)++;
                
                sprintf(buffer, "%d", current_time + execute_time);
                strcpy(gantt_chart[*count], buffer);
                (*count)++;
                
                strcpy(gantt_chart[*count], "s)");
                (*count)++;
                
                strcpy(gantt_chart[*count], " P");
                (*count)++;
                
                sprintf(buffer, "%d", processes[i].pid);
                strcpy(gantt_chart[*count], buffer);
                (*count)++;   

                current_time += execute_time;
                processes[i].remaining_time -= execute_time;

                if (processes[i].remaining_time == 0) {
                    strcpy(gantt_chart[*count], " completed\n");
                    (*count)++;
                    
                    processes[i].completion_time = current_time;
                    completed_processes++;
                    dyna_free(processes[i].mem_block_id);  // 메모리 해제
                }
                else {
                    strcpy(gantt_chart[*count], " /");
                    (*count)++;
                    sprintf(buffer, "%d", processes[i].remaining_time);
                    strcpy(gantt_chart[*count], buffer);
                    (*count)++;
                    strcpy(gantt_chart[*count], "s remaining\n");
                    (*count)++;
                }
            }
        }
    }
    print_memory_blocks();
    
    //print the statistics for scheduling
    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    float total_response_time = 0;
    
    for (int i = 0; i < num_processes; ++i) {
        total_turnaround_time += processes[i].completion_time - processes[i].arrival_time;
        total_waiting_time += processes[i].completion_time - processes[i].arrival_time - processes[i].burst_time;
        total_response_time += processes[i].completion_time - processes[i].arrival_time;
    }

    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    float avg_response_time = total_response_time / num_processes;

    float variance = 0;
    for (int i = 0; i < num_processes; ++i) {
        float response_time = processes[i].completion_time - processes[i].arrival_time;
        variance += pow(response_time - avg_response_time, 2);
    }
    variance /= num_processes;

    printf("\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("Average Response Time: %.2f\n", avg_response_time);
    printf("Variance of Response Time: %.2f\n", variance);
    printf("\n");
}

void fcfs(Process *processes, int num_processes, char (*gantt_chart)[100], int *count) {
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completed_processes = 0;
    char buffer[100];

    while (completed_processes < num_processes) {
        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (processes[i].mem_block_id == 0) {
                    processes[i].mem_block_id = dyna_alloc(processes[i].size, processes[i]);
                    if (processes[i].mem_block_id == -1) {
                        printf("Memory allocation failed for Process %d\n", processes[i].pid);
                        continue;
                    }
                }
                print_memory_blocks();
                
                //statistics calculation
                int waiting_time = current_time - processes[i].arrival_time;
        	if (waiting_time < 0)
            	    waiting_time = 0;
        	int turnaround_time = waiting_time + processes[i].burst_time;
        	total_waiting_time += waiting_time;
        	total_turnaround_time += turnaround_time;
                    
                int execute_time = processes[i].burst_time;

                for (int t = 0; t < current_time; ++t) {
                    strcpy(gantt_chart[*count], "  ");
                    (*count)++;
                }

                for (int j = 0; j < execute_time; ++j) {
                    strcpy(gantt_chart[*count], "||");
                    (*count)++;
                }

                strcpy(gantt_chart[*count], "(");
                (*count)++;

                sprintf(buffer, "%d", current_time + execute_time);
                strcpy(gantt_chart[*count], buffer);
                (*count)++;

                strcpy(gantt_chart[*count], "s)");
                (*count)++;

                strcpy(gantt_chart[*count], " P");
                (*count)++;

                sprintf(buffer, "%d", processes[i].pid);
                strcpy(gantt_chart[*count], buffer);
                (*count)++;

                current_time += execute_time;
                processes[i].remaining_time = 0;
                processes[i].completion_time = current_time;
                completed_processes++;
                dyna_free(processes[i].mem_block_id);
                strcpy(gantt_chart[*count], " completed\n");
                (*count)++;
            }
        }
    }
    print_memory_blocks();
    
    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    printf("\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("\n");
}

void sjf(Process *processes, int num_processes, char (*gantt_chart)[100], int *count) {
    int completed_processes = 0;
    char buffer[100];

    while (completed_processes < num_processes) {
        int shortest_index = -1;
        int shortest_time = INT_MAX;

        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].burst_time < shortest_time) {
                shortest_index = i;
                shortest_time = processes[i].burst_time;
            }
        }

        if (shortest_index == -1) {
            current_time++;
            continue;
        }

        if (processes[shortest_index].mem_block_id == 0) {
            processes[shortest_index].mem_block_id = dyna_alloc(processes[shortest_index].size, processes[shortest_index]);
            if (processes[shortest_index].mem_block_id == -1) {
                printf("Memory allocation failed for Process %d\n", processes[shortest_index].pid);
                continue;
            }
        }
        print_memory_blocks();
        int execute_time = processes[shortest_index].burst_time;

        for (int t = 0; t < current_time; ++t) {
            strcpy(gantt_chart[*count], "  ");
            (*count)++;
        }

        for (int j = 0; j < execute_time; ++j) {
            strcpy(gantt_chart[*count], "||");
            (*count)++;
        }

        strcpy(gantt_chart[*count], "(");
        (*count)++;

        sprintf(buffer, "%d", current_time + execute_time);
        strcpy(gantt_chart[*count], buffer);
        (*count)++;

        strcpy(gantt_chart[*count], "s)");
        (*count)++;

        strcpy(gantt_chart[*count], " P");
        (*count)++;

        sprintf(buffer, "%d", processes[shortest_index].pid);
        strcpy(gantt_chart[*count], buffer);
        (*count)++;

        current_time += processes[shortest_index].burst_time;
        processes[shortest_index].remaining_time = 0;
        processes[shortest_index].completion_time = current_time;
        completed_processes++;
        dyna_free(processes[shortest_index].mem_block_id);
        strcpy(gantt_chart[*count], " completed\n");
        (*count)++;
    }
    print_memory_blocks();
    
    // Calculate completion time
    int current_time_for_calc = 0;
    for (int i = 0; i < num_processes; ++i)
    {
        if (current_time_for_calc < processes[i].arrival_time)
        {
            current_time_for_calc = processes[i].arrival_time;
        }
        processes[i].completion_time = current_time_for_calc + processes[i].burst_time;
        current_time_for_calc = processes[i].completion_time;
    }

    int total_turnaround_time = 0;
    int total_waiting_time = 0;

    for (int i = 0; i < num_processes; ++i)
    {
        total_turnaround_time += processes[i].completion_time - processes[i].arrival_time;
        total_waiting_time += (processes[i].completion_time - processes[i].arrival_time - processes[i].burst_time);
    }

    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;

    printf("\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("\n");
}

void srtf(Process *processes, int num_processes, char (*gantt_chart)[100], int *count) {
    int completed_processes = 0;
    char buffer[100];
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int temp = -1;
    int index;

    while (completed_processes < num_processes) {
        int shortest_index = -1;
        int shortest_time = INT_MAX;

        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].remaining_time < shortest_time) {
                shortest_index = i;
                shortest_time = processes[i].remaining_time;
            }
        }

        if (shortest_index == -1) {
            current_time++;
            continue;
        }

        if (processes[shortest_index].mem_block_id == 0) {
            processes[shortest_index].mem_block_id = dyna_alloc(processes[shortest_index].size, processes[shortest_index]);
            if (processes[shortest_index].mem_block_id == -1) {
                printf("Memory allocation failed for Process %d\n", processes[shortest_index].pid);
                continue;
            }
        }
        temp++;
        if (temp == 0)
           index = shortest_index;
        print_memory_blocks();
        
        
        
        

        

        /*strcpy(gantt_chart[*count], "(");
        (*count)++;

        sprintf(buffer, "%d", current_time + 1);
        strcpy(gantt_chart[*count], buffer);
        (*count)++;

        strcpy(gantt_chart[*count], "s)");
        (*count)++;

        strcpy(gantt_chart[*count], " P");
        (*count)++;

        sprintf(buffer, "%d", processes[shortest_index].pid);
        strcpy(gantt_chart[*count], buffer);
        (*count)++;*/

        if (processes[index].remaining_time == 0) {
            processes[index].completion_time = current_time;
            dyna_free(processes[index].mem_block_id);
            
            strcpy(gantt_chart[*count], "(");
            (*count)++;

            sprintf(buffer, "%d", current_time);
            strcpy(gantt_chart[*count], buffer);
            (*count)++;

            strcpy(gantt_chart[*count], "s)");
            (*count)++;

            strcpy(gantt_chart[*count], " P");
            (*count)++;

            sprintf(buffer, "%d", processes[index].pid);
            strcpy(gantt_chart[*count], buffer);
            (*count)++;
            
            strcpy(gantt_chart[*count], " completed\n");
            (*count)++;
            for (int t = 0; t < current_time; ++t) {
            	strcpy(gantt_chart[*count], "  ");
            	(*count)++;
            }
            //statistics calculation
            int waiting_time = current_time - processes[index].arrival_time - processes[index].burst_time;
            total_waiting_time += waiting_time;
            int turnaround_time = current_time - processes[index].arrival_time;
            total_turnaround_time += turnaround_time;
        } else {
            if (index != shortest_index) {
            	strcpy(gantt_chart[*count], "(");
                (*count)++;

                sprintf(buffer, "%d", current_time);
                strcpy(gantt_chart[*count], buffer);
                (*count)++;

                strcpy(gantt_chart[*count], "s)");
                (*count)++;

                strcpy(gantt_chart[*count], " P");
                (*count)++;

                sprintf(buffer, "%d", processes[index].pid);
                strcpy(gantt_chart[*count], buffer);
                (*count)++;
                
            	strcpy(gantt_chart[*count], " /");
            	(*count)++;
            	sprintf(buffer, "%d", processes[index].remaining_time);
            	strcpy(gantt_chart[*count], buffer);
            	(*count)++;
            	strcpy(gantt_chart[*count], "s remaining\n");
            	(*count)++;
            	for (int t = 0; t < current_time; ++t) {
            	    strcpy(gantt_chart[*count], "  ");
                    (*count)++;
                }
            }
        }
        current_time++;
        processes[shortest_index].remaining_time--;
        for (int j = 0; j < 1; ++j) {
            strcpy(gantt_chart[*count], "||");
            (*count)++;
        }
        if (processes[shortest_index].remaining_time == 0)
           completed_processes++;
        
        index = shortest_index;
    }
    processes[index].completion_time = current_time;
    completed_processes++;
    dyna_free(processes[index].mem_block_id);
    print_memory_blocks();
            
    strcpy(gantt_chart[*count], "(");
    (*count)++;

    sprintf(buffer, "%d", current_time);
    strcpy(gantt_chart[*count], buffer);
    (*count)++;

    strcpy(gantt_chart[*count], "s)");
    (*count)++;

    strcpy(gantt_chart[*count], " P");
    (*count)++;

    sprintf(buffer, "%d", processes[index].pid);
    strcpy(gantt_chart[*count], buffer);
    (*count)++;
            
    strcpy(gantt_chart[*count], " completed\n");
    (*count)++;
    for (int t = 0; t < current_time; ++t) {
        strcpy(gantt_chart[*count], "  ");
        (*count)++;
    }
    //statistics calculation
    int waiting_time = current_time - processes[index].arrival_time - processes[index].burst_time;
    total_waiting_time += waiting_time;
    int turnaround_time = current_time - processes[index].arrival_time;
    total_turnaround_time += turnaround_time;
            
    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    printf("\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("\n");
}

void priority_scheduling(Process *processes, int num_processes, char (*gantt_chart)[100], int *count) {
    int completed_processes = 0;
    char buffer[100];

    while (completed_processes < num_processes) {
        int highest_priority = INT_MAX;
        int highest_index = -1;

        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0 && processes[i].priority < highest_priority) {
                highest_priority = processes[i].priority;
                highest_index = i;
            }
        }

        if (highest_index == -1) {
            current_time++;
            continue;
        }

        if (processes[highest_index].mem_block_id == 0) {
            processes[highest_index].mem_block_id = dyna_alloc(processes[highest_index].size, processes[highest_index]);
            if (processes[highest_index].mem_block_id == -1) {
                printf("Memory allocation failed for Process %d\n", processes[highest_index].pid);
                continue;
            }
        }
        print_memory_blocks();
        int execute_time = processes[highest_index].burst_time;

        printf("\n");
        for (int t = 0; t < current_time; ++t) {
            strcpy(gantt_chart[*count], "  ");
            (*count)++;
        }

        for (int j = 0; j < execute_time; ++j) {
            strcpy(gantt_chart[*count], "||");
            (*count)++;
        }

        strcpy(gantt_chart[*count], "(");
        (*count)++;

        sprintf(buffer, "%d", current_time + execute_time);
        strcpy(gantt_chart[*count], buffer);
        (*count)++;

        strcpy(gantt_chart[*count], "s)");
        (*count)++;

        strcpy(gantt_chart[*count], " P");
        (*count)++;

        sprintf(buffer, "%d", processes[highest_index].pid);
        strcpy(gantt_chart[*count], buffer);
        (*count)++;

        current_time += processes[highest_index].burst_time;
        processes[highest_index].remaining_time = 0;
        processes[highest_index].completion_time = current_time;
        completed_processes++;
        dyna_free(processes[highest_index].mem_block_id);
        strcpy(gantt_chart[*count], " completed\n");
        (*count)++;
    }
    print_memory_blocks();
    printf("\n");
}

int schedule() {
    int num_processes;
    int optimization_option;
    int quantum = 0;
    current_time = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    Process *processes = (Process *)malloc(num_processes * sizeof(Process));
    char gantt_chart[10000][100];
    int count = 0;

    printf("Enter process details:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d:\n", i + 1);
        printf("PID: ");
        scanf("%d", &processes[i].pid);
        printf("Priority: ");
        scanf("%d", &processes[i].priority);
        printf("Process Size: ");
        scanf("%zu", &processes[i].size);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burst_time);
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrival_time);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].mem_block_id = 0;  // 초기 메모리 할당 ID는 0 (미할당 상태)
    }

    printf("Select optimization option:\n");
    printf("1 -> Maximize throughput\n");
    printf("2 -> Minimize turnaround time\n");
    printf("3 -> Minimize waiting time\n");
    printf("4 -> Minimize response time variance\n");
    printf("5 -> Respect priority\n");
    printf("Selection: ");
    scanf("%d", &optimization_option);

    switch (optimization_option) {
        case 1:
            printf("Selected CPU scheduler: First-Come, First-Served (FCFS)\n");
            fcfs(processes, num_processes, gantt_chart, &count);
            break;
        case 2:
            printf("Selected CPU scheduler: Shortest Job First (SJF)\n");
            sjf(processes, num_processes, gantt_chart, &count);
            break;
        case 3:
            printf("Selected CPU scheduler: Shortest Remaining Time First (SRTF)\n");
            srtf(processes, num_processes, gantt_chart, &count);
            break;
        case 4:
            printf("Selected CPU scheduler: Round Robin (RR)\n");
            printf("Enter time quantum for Round Robin: ");
            scanf("%d", &quantum);
            round_robin(processes, num_processes, quantum, gantt_chart, &count);
            break;
        case 5:
            printf("Selected CPU scheduler: Priority Scheduling (PS)\n");
            priority_scheduling(processes, num_processes, gantt_chart, &count);
            break;
        default:
            printf("Invalid option\n");
            break;
    }
    
    //print time unit
    int scale = current_time;
    if (current_time % 5 != 0)
       scale += (5 - current_time % 5);
    for (int t = 0; t <= scale; t += 5) {
    	printf("|%ds      ", t);
    }
    printf("\n");

    for (int i = 0; i < count; i++) {
        printf("%s", gantt_chart[i]);
    }
    printf("\n");

    free(processes);
    return 0;
}
