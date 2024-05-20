#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "memory_management.h"  // 이 파일에는 dyna_alloc 및 dyna_free 함수가 정의되어 있어야 합니다.
#include "scheduling.h"

void round_robin(Process *processes, int num_processes, int quantum) {
    int current_time = 0;
    int completed_processes = 0;

    char *gantt_chart[1000];
    for (int i = 0; i < 1000; i++) {
    gantt_chart[i] = (char *)malloc(100 * sizeof(char)); // 충분한 크기의 문자열을 저장할 메모리 할당
    }
    int count = 0; // 저장된 문자열 개수
    char buffer[100];
    
    //print time units
    //strcpy(gantt_chart[count], "\n");
    //count++;
    //for (int t = 0; t <= 50; t += 5) {
    	//strcpy(gantt_chart[count], "|");
    	//count++;
    	//sprintf(buffer, "%d", t);
    	//strcpy(gantt_chart[count], buffer);
    	//count++;
    	//strcpy(gantt_chart[count], "s      ");
    	//count++;
    //}
    //strcpy(gantt_chart[count], "\n");
    //count++;
    
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
                
                
                // Print spaces until the process starts
                for (int t = 0; t < current_time; ++t) {
                    strcpy(gantt_chart[count], "  ");
    		    count++;
                }
    		
    		// Print the burst time
                for (int j = 0; j < execute_time; ++j) {
                    strcpy(gantt_chart[count], "||");
    		    count++;
    		}
    		
    		strcpy(gantt_chart[count], "(");
    		count++;
    		
    		sprintf(buffer, "%d", current_time + execute_time);
    		strcpy(gantt_chart[count], buffer);
    		count++;
    		
    		strcpy(gantt_chart[count], "s)");
    		count++;
    		
    		strcpy(gantt_chart[count], " P");
    		count++;
    		
    		sprintf(buffer, "%d", processes[i].pid);
    		strcpy(gantt_chart[count], buffer);
    		count++;   

                current_time += execute_time;
                processes[i].remaining_time -= execute_time;

                if (processes[i].remaining_time == 0) {
                    //printf("completed");
                    
    		    strcpy(gantt_chart[count], " completed\n");
    		    count++;
    		    
                    processes[i].completion_time = current_time;
                    completed_processes++;
                    if (completed_processes == num_processes) {
                    	//print time units
                    	printf("\n");
                    	int scale = current_time + current_time % 5;
    			for (int t = 0; t <= scale; t += 5) {
    				printf("|%ds      ", t);
    			}
    			printf("\n");
                    }
                    dyna_free(processes[i].mem_block_id);  // 메모리 해제
                }
                else {
                    strcpy(gantt_chart[count], " /");
    		    count++;
    		    sprintf(buffer, "%d", processes[i].remaining_time);
    		    strcpy(gantt_chart[count], buffer);
    		    count++;
    		    strcpy(gantt_chart[count], "s remaining\n");
    		    count++;
                }
            }
        }
    }
    
    // 배열에 저장된 문자열 출력
    for (int i = 0; i < count; i++) {
        printf("%s", gantt_chart[i]);
    }
    // gantt_chart 배열에 할당된 메모리 해제
    for (int i = 0; i < 1000; i++) {
        free(gantt_chart[i]);
    }

}

int schedule() {
    int num_processes;
    int quantum;

    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    Process *processes = (Process *)malloc(num_processes * sizeof(Process));

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

    printf("Enter time quantum for Round Robin: ");
    scanf("%d", &quantum);

    round_robin(processes, num_processes, quantum);
    
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

    free(processes);
    return 0;
}
