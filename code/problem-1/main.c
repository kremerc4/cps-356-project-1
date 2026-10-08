#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// define structure to represent each process
struct Process {
    // user-specified process data
    int pid;
    int arrival_time;
    int run_time;

    // simulation-related process data
    int remaining_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
};

// define structure for easier Gantt chart representation
struct GanttSegment {
    int start_time;
    int end_time;
    int pid;
};

void print_gantt_chart(struct GanttSegment *segments, int num_segments) {
    for (int i = 0; i < num_segments; i++) {
        printf("Process %d: Start Time = %d, End Time = %d\n", segments[i].pid, segments[i].start_time, segments[i].end_time);
    }
}

void fcfs(struct Process *processes, int num_processes) {
    // create a counter for the current time
    int current_time = 0;

    // create an array to store Gantt chart segments
    struct GanttSegment *gantt_segments = (struct GanttSegment *)malloc(num_processes * sizeof(struct GanttSegment));
    int gantt_segment_count = 0;
    int completed_processes = 0;

    while (completed_processes < num_processes) {
        int earliest_arrival_index = -1;

        for (int i = 0; i < num_processes; i++) {
            // check that there are uncompleted processes
            if (processes[i].remaining_time > 0) {               
                // check if uncompleted process has arrived
                if (processes[i].arrival_time <= current_time) {
                    // see if candidate for next process to run
                    if (earliest_arrival_index == -1 || processes[i].arrival_time < processes[earliest_arrival_index].arrival_time) {
                        earliest_arrival_index = i;
                    }
                }

            }
        }

        if (earliest_arrival_index == -1) {
            printf("DEBUG: no process available at time %d\n", current_time);
            // no process is available to run, increment current time
            current_time++;
            continue;
        } 
        
        printf("DEBUG: currently running process %d at time %d\n", earliest_arrival_index, current_time);
        // run the earliest arrival process for one time unit
        processes[earliest_arrival_index].remaining_time--;
        if (processes[earliest_arrival_index].remaining_time == 0) {
            completed_processes++;
        }
        current_time++;
    }

    // print the Gantt chart
    // print_gantt_chart(gantt_segments, gantt_segment_count);

    // // free the allocated memory for Gantt chart segments
    // free(gantt_segments);
}

void sjf(struct Process *processes, int num_processes) {
    // SJF scheduling logic here
}

void stcf(struct Process *processes, int num_processes) {
    // STCF scheduling logic here
}

void round_robin(struct Process *processes, int num_processes) {
    // Round Robin scheduling logic here
}

int main() {
    // get the number of processes from the user
    int num_processes;
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    // create an array of processes
    struct Process *processes = (struct Process *)malloc(num_processes * sizeof(struct Process));

    // get the details of each process from the user
    for (int i = 0; i < num_processes; i++) {
        printf("Enter PID, arrival time, and run time for process %d: ", i + 1);
        scanf("%d %d %d", &processes[i].pid, &processes[i].arrival_time, &processes[i].run_time);
        processes[i].remaining_time = processes[i].run_time;
    }

    // get which scheduling algorithm to use from the user
    int scheduling_algorithm;
    printf("Enter 1 for FCFS, 2 for SJF, 3 for STCF, or 4 for Round Robin: ");
    scanf("%d", &scheduling_algorithm);

    switch (scheduling_algorithm) {
        case 1:
            fcfs(processes, num_processes);
            break;
        case 2:
            sjf(processes, num_processes);
            break;
        case 3:
            stcf(processes, num_processes);
            break;
        case 4:
            round_robin(processes, num_processes);
            break;
        default:
            printf("Invalid scheduling algorithm selected.\n");
            return 1;
    }

    return 0;
}
