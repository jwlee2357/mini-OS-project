#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // For INT_MAX
#include <math.h>   // For variance calculation

// Struct for a process
typedef struct
{
    int pid;
    int priority;
    int burst_time;
    int remaining_time;
    int arrival_time;
    int completion_time;
} Process;

// Function prototypes
void maximize_throughput(Process *processes, int num_processes);
void minimize_turnaround_time(Process *processes, int num_processes);
void minimize_waiting_time(Process *processes, int num_processes);
void minimize_response_time_variance(Process *processes, int num_processes, int quantum);

// CPU scheduling algorithms
void fcfs(Process *processes, int num_processes);
void sjf(Process *processes, int num_processes);
void srtf(Process *processes, int num_processes);
void rr(Process *processes, int num_processes, int quantum);
void priority_scheduling(Process *processes, int num_processes);
void multilevel_queue(Process *processes, int num_processes);
void multilevel_feedback_queues(Process *processes, int num_processes);

void context_switching()
{
    int num_processes;
    int optimization_option;

    printf("\n");
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    Process *processes = (Process *)malloc(num_processes * sizeof(Process));

    printf("Enter process details:\n");
    printf("\n");
    for (int i = 0; i < num_processes; i++)
    {
        printf("Process %d:\n", i + 1);
        printf("PID: ");
        scanf("%d", &processes[i].pid);
        printf("Priority: ");
        scanf("%d", &processes[i].priority);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burst_time);
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("\n");
        processes[i].remaining_time = processes[i].burst_time;
    }

    printf("Select optimization option:\n");
    printf("1 -> Maximize throughput\n");
    printf("2 -> Minimize turnaround time\n");
    printf("3 -> Minimize waiting time\n");
    printf("4 -> Minimize response time variance\n");
    printf("Selection: ");
    scanf("%d", &optimization_option);

    switch (optimization_option)
    {
    case 1:
        maximize_throughput(processes, num_processes);
        break;
    case 2:
        minimize_turnaround_time(processes, num_processes);
        break;
    case 3:
        minimize_waiting_time(processes, num_processes);
        break;
    case 4:
    {
        printf("Selected CPU scheduler: Round Robin (RR)\n");
        printf("Enter time quantum for Round Robin: ");
        int quantum;
        scanf("%d", &quantum);
        minimize_response_time_variance(processes, num_processes, quantum);
        break;
    }
    default:
        printf("Invalid option\n");
        break;
    }

    free(processes);
}

// Optimization Options

void maximize_throughput(Process *processes, int num_processes)
{
    printf("Selected CPU scheduler: First-Come, First-Served (FCFS)\n");
    fcfs(processes, num_processes);
}

void minimize_turnaround_time(Process *processes, int num_processes)
{
    printf("Selected CPU scheduler: Shortest Job First (SJF)\n");

    sjf(processes, num_processes);

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

void minimize_waiting_time(Process *processes, int num_processes)
{
    printf("Selected CPU scheduler: Shortest Remaining Time First (SRTF)\n");
    srtf(processes, num_processes);
}

void minimize_response_time_variance(Process *processes, int num_processes, int quantum)
{

    int total_turnaround_time = 0;
    int total_waiting_time = 0;
    float total_response_time = 0;

    rr(processes, num_processes, quantum);

    for (int i = 0; i < num_processes; ++i)
    {
        total_turnaround_time += processes[i].completion_time - processes[i].arrival_time;
        total_waiting_time += processes[i].completion_time - processes[i].arrival_time - processes[i].burst_time;
        total_response_time += processes[i].completion_time - processes[i].arrival_time;
    }

    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    float avg_response_time = total_response_time / num_processes;

    float variance = 0;
    for (int i = 0; i < num_processes; ++i)
    {
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

// Scheduling Algorithms

void fcfs(Process *processes, int num_processes)
{
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    printf("\nProcess\tWaiting Time\tTurnaround Time\n");

    int current_time = 0;
    for (int i = 0; i < num_processes; i++)
    {
        int waiting_time = current_time - processes[i].arrival_time;
        if (waiting_time < 0)
            waiting_time = 0;
        int turnaround_time = waiting_time + processes[i].burst_time;
        total_waiting_time += waiting_time;
        total_turnaround_time += turnaround_time;

        printf("%d\t%d\t\t%d\n", processes[i].pid, waiting_time, turnaround_time);
        current_time += processes[i].burst_time;
    }

    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    printf("\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("\n");
}

void sjf(Process *processes, int num_processes)
{
    // Sort processes by burst time
    for (int i = 0; i < num_processes - 1; i++)
    {
        for (int j = 0; j < num_processes - i - 1; j++)
        {
            if (processes[j].burst_time > processes[j + 1].burst_time ||
                (processes[j].burst_time == processes[j + 1].burst_time && processes[j].arrival_time > processes[j + 1].arrival_time))
            {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    // Calculate completion time
    int current_time = 0;
    for (int i = 0; i < num_processes; ++i)
    {
        if (current_time < processes[i].arrival_time)
        {
            current_time = processes[i].arrival_time;
        }
        processes[i].completion_time = current_time + processes[i].burst_time;
        current_time = processes[i].completion_time;
    }
}

void srtf(Process *processes, int num_processes)
{
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    printf("\n");
    printf("\nProcess\tWaiting Time\tTurnaround Time\n");

    int current_time = 0;
    int completed_processes = 0;
    while (completed_processes < num_processes)
    {
        int shortest_index = -1;
        int smallest_remaining_time = INT_MAX;

        for (int i = 0; i < num_processes; i++)
        {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0)
            {
                if (processes[i].remaining_time < smallest_remaining_time)
                {
                    shortest_index = i;
                    smallest_remaining_time = processes[i].remaining_time;
                }
            }
        }

        if (shortest_index == -1)
        {
            current_time++;
            continue;
        }

        processes[shortest_index].remaining_time--;
        current_time++;

        if (processes[shortest_index].remaining_time == 0)
        {
            completed_processes++;
            int waiting_time = current_time - processes[shortest_index].arrival_time - processes[shortest_index].burst_time;
            total_waiting_time += waiting_time;
            int turnaround_time = current_time - processes[shortest_index].arrival_time;
            total_turnaround_time += turnaround_time;

            printf("\n");
            printf("%d\t%d\t\t%d\n", processes[shortest_index].pid, waiting_time, turnaround_time);
        }
    }

    float avg_waiting_time = (float)total_waiting_time / num_processes;
    float avg_turnaround_time = (float)total_turnaround_time / num_processes;
    printf("\n");
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
    printf("\n");
}

void rr(Process *processes, int num_processes, int quantum)
{
    int *remaining_time = (int *)malloc(num_processes * sizeof(int));
    int *completion_time = (int *)malloc(num_processes * sizeof(int));

    int current_time = 0;
    int *visited = (int *)calloc(num_processes, sizeof(int));
    int completed_processes = 0;

    // Print time units
    printf("\n");
    for (int t = 0; t <= 50; t += 5) {
        printf("|%ds", t);
        printf("      ");
    }
    printf("\n");
    
    while (completed_processes < num_processes)
    {
        for (int i = 0; i < num_processes; ++i)
        {
            if (visited[i] == 0 && processes[i].arrival_time <= current_time)
            {
                int execute_time = (processes[i].remaining_time > quantum) ? quantum : processes[i].remaining_time;
                
                // Print spaces until the process starts
                for (int t = 0; t < current_time; ++t)
                    printf("  ");

                // Print the burst time
                for (int j = 0; j < execute_time; ++j)
                    printf("||");
                printf("(%ds)", current_time + execute_time);
                printf(" P%d ", processes[i].pid);
                    
                current_time += execute_time;
                processes[i].remaining_time -= execute_time;

                if (processes[i].remaining_time == 0)
                {
               	    printf("completed");
                    completion_time[i] = current_time;
                    visited[i] = 1;
                    completed_processes++;
                }
                else
                    printf("/%ds remaining", processes[i].remaining_time);
                printf("\n");
            }
        }
    }

    for (int i = 0; i < num_processes; ++i)
    {
        processes[i].completion_time = completion_time[i];
    }

    free(remaining_time);
    free(completion_time);
    free(visited);
}

void priority_scheduling(Process *processes, int num_processes)
{
    // Sort processes by priority
    for (int i = 0; i < num_processes - 1; i++)
    {
        for (int j = 0; j < num_processes - i - 1; j++)
        {
            if (processes[j].priority > processes[j + 1].priority)
            {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    fcfs(processes, num_processes);
}

void multilevel_queue(Process *processes, int num_processes)
{
    // Assuming two levels: High priority and Low priority

    Process high_priority_queue[num_processes];
    Process low_priority_queue[num_processes];
    int high_priority_count = 0;
    int low_priority_count = 0;

    // Separate processes into high and low priority queues
    for (int i = 0; i < num_processes; i++)
    {
        if (processes[i].priority < 5)
        { // Change the priority threshold as needed
            high_priority_queue[high_priority_count++] = processes[i];
        }
        else
        {
            low_priority_queue[low_priority_count++] = processes[i];
        }
    }

    // Sort both queues by burst time
    for (int i = 0; i < high_priority_count - 1; i++)
    {
        for (int j = 0; j < high_priority_count - i - 1; j++)
        {
            if (high_priority_queue[j].burst_time > high_priority_queue[j + 1].burst_time)
            {
                Process temp = high_priority_queue[j];
                high_priority_queue[j] = high_priority_queue[j + 1];
                high_priority_queue[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < low_priority_count - 1; i++)
    {
        for (int j = 0; j < low_priority_count - i - 1; j++)
        {
            if (low_priority_queue[j].burst_time > low_priority_queue[j + 1].burst_time)
            {
                Process temp = low_priority_queue[j];
                low_priority_queue[j] = low_priority_queue[j + 1];
                low_priority_queue[j + 1] = temp;
            }
        }
    }

    printf("\n");
    printf("\nHigh Priority Queue:\n");
    fcfs(high_priority_queue, high_priority_count);

    printf("\nLow Priority Queue:\n");
    fcfs(low_priority_queue, low_priority_count);
}

void multilevel_feedback_queues(Process *processes, int num_processes)
{
    // Assuming three levels: Level 0, Level 1, and Level 2

    Process level0_queue[num_processes];
    Process level1_queue[num_processes];
    Process level2_queue[num_processes];
    int level0_count = 0;
    int level1_count = 0;
    int level2_count = 0;

    int time_quantum[] = {2, 4}; // Quantum for Level 0 and Level 1

    for (int i = 0; i < num_processes; i++)
    {
        if (processes[i].priority == 0)
        {
            level0_queue[level0_count++] = processes[i];
        }
        else if (processes[i].priority == 1)
        {
            level1_queue[level1_count++] = processes[i];
        }
        else
        {
            level2_queue[level2_count++] = processes[i];
        }
    }

    printf("\n");
    printf("\nLevel 0 Queue:\n");
    rr(level0_queue, level0_count, time_quantum[0]);

    printf("\nLevel 1 Queue:\n");
    rr(level1_queue, level1_count, time_quantum[1]);

    printf("\nLevel 2 Queue:\n");
    fcfs(level2_queue, level2_count);
}
