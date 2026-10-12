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
    if (segments[0].start_time > 0) {
        printf("Idle: Start Time = 0, End Time = %d\n", segments[0].start_time);
    }

    for (int i = 0; i < num_segments; i++) {
        // TODO: update idle condition to account for time between segments
        // Use for loop that compares last end time to current start time
        printf("Process %d: Start Time = %d, End Time = %d\n", segments[i].pid, segments[i].start_time, segments[i].end_time);
    }
}

void fcfs(struct Process *processes, int num_processes) {
    // create a counter for the current time
    int current_time = 0;

    // create an array to store Gantt chart segments
    struct GanttSegment *gantt_segments = (struct GanttSegment *)malloc(num_processes * sizeof(struct GanttSegment));
    int gantt_segment_count = 0;
    
    // keep track of how many processes have completed
    int completed_processes = 0;

    // while there are uncompleted processes, continue scheduling
    while (completed_processes < num_processes) {
        // find the earliest arriving uncompleted process
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

        // no process is available to run, increment current time
        if (earliest_arrival_index == -1) {
            current_time++;
            continue;
        } 
        
        // update process data
        current_time += processes[earliest_arrival_index].remaining_time;
        processes[earliest_arrival_index].completion_time = current_time;
        processes[earliest_arrival_index].turnaround_time = processes[earliest_arrival_index].completion_time - processes[earliest_arrival_index].arrival_time;
        processes[earliest_arrival_index].waiting_time = processes[earliest_arrival_index].turnaround_time - processes[earliest_arrival_index].run_time;
        processes[earliest_arrival_index].remaining_time = 0;
        completed_processes++;

        // add a segment to the Gantt chart
        gantt_segments[gantt_segment_count].pid = processes[earliest_arrival_index].pid;
        gantt_segments[gantt_segment_count].start_time = current_time - processes[earliest_arrival_index].run_time;
        gantt_segments[gantt_segment_count].end_time = current_time;
        gantt_segment_count++;
    }

    // print the Gantt chart
    print_gantt_chart(gantt_segments, gantt_segment_count);

    // free the allocated memory for Gantt chart segments
    free(gantt_segments);
}

void sjf(struct Process *processes, int num_processes) {
    // create a counter for the current time
    int current_time = 0;

    // create an array to store Gantt chart segments
    struct GanttSegment *gantt_segments = (struct GanttSegment *)malloc(num_processes * sizeof(struct GanttSegment));
    int gantt_segment_count = 0;
    
    // keep track of how many processes have completed
    int completed_processes = 0;

    // while there are uncompleted processes, continue scheduling
    while (completed_processes < num_processes) {
        // find the shortest uncompleted process
        int shortest_index = -1;
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (shortest_index == -1 || processes[i].remaining_time < processes[shortest_index].remaining_time) {
                    shortest_index = i;
                }
            }
        }

        // no process is available to run, increment current time
        if (shortest_index == -1) {
            current_time++;
            continue;
        } 
        
        // update process data
        current_time += processes[shortest_index].remaining_time;
        processes[shortest_index].completion_time = current_time;
        processes[shortest_index].turnaround_time = processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
        processes[shortest_index].waiting_time = processes[shortest_index].turnaround_time - processes[shortest_index].run_time;
        processes[shortest_index].remaining_time = 0;
        completed_processes++;

        // add a segment to the Gantt chart
        gantt_segments[gantt_segment_count].pid = processes[shortest_index].pid;
        gantt_segments[gantt_segment_count].start_time = current_time - processes[shortest_index].run_time;
        gantt_segments[gantt_segment_count].end_time = current_time;
        gantt_segment_count++;
    }

    // print the Gantt chart
    print_gantt_chart(gantt_segments, gantt_segment_count);

    // free the allocated memory for Gantt chart segments
    free(gantt_segments);
}

void stcf(struct Process *processes, int num_processes) {
    // create a counter for the current time
    int current_time = 0;

    // create an array to store Gantt chart segments
    struct GanttSegment *gantt_segments = (struct GanttSegment *)malloc(num_processes * sizeof(struct GanttSegment));
    int gantt_segment_count = 0;
    // create a variable to track the current capacity of the Gantt chart array
    int gantt_capacity = num_processes;
    
    // keep track of how many processes have completed
    int completed_processes = 0;

    // while there are uncompleted processes, continue scheduling
    while (completed_processes < num_processes) {
        // find the shortest uncompleted process
        int shortest_index = -1;
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= current_time && processes[i].remaining_time > 0) {
                if (shortest_index == -1 || processes[i].remaining_time < processes[shortest_index].remaining_time) {
                    shortest_index = i;
                }
            }
        }

        // no process is available to run, increment current time
        if (shortest_index == -1) {
            current_time++;
            continue;
        }

        if (gantt_segment_count == 0 || processes[shortest_index].pid != gantt_segments[gantt_segment_count - 1].pid) {
            // process gantt sections
            if (gantt_segment_count > 0) {
                gantt_segments[gantt_segment_count - 1].end_time = current_time;
            }
            gantt_segments[gantt_segment_count].pid = processes[shortest_index].pid;
            gantt_segments[gantt_segment_count].start_time = current_time;
            gantt_segment_count++;
        }

        // update process on a second-by-second basis
        processes[shortest_index].remaining_time--;
        current_time++;

        // double size of the Gantt chart array if approaching the current capacity (adding a new segment would reach capacity)
        if ((gantt_segment_count + 1) >= gantt_capacity) {
            gantt_capacity *= 2;
            gantt_segments = (struct GanttSegment *)realloc(gantt_segments, gantt_capacity * sizeof(struct GanttSegment));
        }

        // if the process has completed, update its completion time
        if (processes[shortest_index].remaining_time == 0) {
            processes[shortest_index].completion_time = current_time;
            processes[shortest_index].turnaround_time = processes[shortest_index].completion_time - processes[shortest_index].arrival_time;
            processes[shortest_index].waiting_time = processes[shortest_index].turnaround_time - processes[shortest_index].run_time;
            completed_processes++;
        }

        // if all processes have completed, update the final Gantt segment end time
        if (completed_processes == num_processes) {
            gantt_segments[gantt_segment_count - 1].end_time = current_time;
        }
    }

    // print the Gantt chart
    print_gantt_chart(gantt_segments, gantt_segment_count);

    // free the allocated memory for Gantt chart segments
    free(gantt_segments);
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
            // exit with invalid scheduling algorithm selection and return error code
            printf("Invalid scheduling algorithm selected.\n");
            return 1;
    }

    return 0;
}
