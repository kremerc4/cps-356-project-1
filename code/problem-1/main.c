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
    printf("\nGantt Chart:\n");

    // construct top half of chart

    // print initial idle segment if the first process doesn't start at time 0
    if (segments[0].start_time > 0) {
        printf("| Idle ");
    }

    for (int i = 0; i < num_segments; i++) {
        // print idle segment if there is a gap between the current and previous process
        if (i > 0 && segments[i].start_time > segments[i - 1].end_time) {
            printf("| Idle ");
        }

        // print the current process segment
        printf("| P%-3d ", segments[i].pid); // ensure all Gantt chart segments are printed with consistent spacing
    }

    // print a newline to finish the top half of the Gantt chart
    printf("|\n");

    // construct bottom half of chart

    for (int i = 0; i < num_segments; i++) {
        // print idle spacing at start time
        if (i == 0 && segments[i].start_time > 0) {
            printf("%-7d", 0);
        }

        // print idle spacing between processes if there is a gap
        if (i > 0 && segments[i].start_time > segments[i - 1].end_time) {
            printf("%-7d", segments[i - 1].end_time);
        }

        // print the start time of the current process segment  
        printf("%-7d", segments[i].start_time);
    }
    
    // print the end time of the last process segment
    printf("%-7d", segments[num_segments - 1].end_time);

    // print a newline to finish the Gantt chart display
    printf("\n");
}

void print_process_stats(struct Process *processes, int num_processes) {
    // create variables to store total waiting and turnaround times for average calculation
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    // print individual process summaries
    printf("\nProcess Summaries:\n");
    for (int i = 0; i < num_processes; i++) {
        printf("PID: %d\n", processes[i].pid);
        printf("\tWaiting Time: %d\n", processes[i].waiting_time);
        printf("\tTurnaround Time: %d\n", processes[i].turnaround_time);
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    // print average waiting and turnaround times
    printf("\nProcess Average Summaries:\n");
    printf("Average Waiting Time: %.2f\n", (float)total_waiting_time / num_processes);
    printf("Average Turnaround Time: %.2f\n", (float)total_turnaround_time / num_processes);

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
    // create a counter for the current time
    int current_time = 0;

    // create an array to store Gantt chart segments
    struct GanttSegment *gantt_segments = (struct GanttSegment *)malloc(num_processes * sizeof(struct GanttSegment));
    int gantt_segment_count = 0;
    // create a variable to track the current capacity of the Gantt chart array
    int gantt_capacity = num_processes;

    // keep track of how many processes have completed
    int completed_processes = 0;

    // set the scheduling quantum for Round Robin
    int scheduling_quantum = 1; // example quantum from class

    // create variable to keep track of the current process index in the Round Robin queue
    int current_process_index = 0;

    // main loop for Round Robin scheduling
    while (completed_processes < num_processes) {
        if (current_process_index >= num_processes) {
            current_process_index = 0;
        }

        // if the current process can not be executed, move to the next process
        if (processes[current_process_index].arrival_time > current_time || processes[current_process_index].remaining_time == 0) {
            current_process_index++;
            continue;
        }
        
        // execute the current process for the scheduling quantum
        int execution_time = (processes[current_process_index].remaining_time > scheduling_quantum) ? scheduling_quantum : processes[current_process_index].remaining_time;
        processes[current_process_index].remaining_time -= execution_time;
        current_time += execution_time;

        // double size of the Gantt chart array if approaching the current capacity (adding a new segment would reach capacity)
        if ((gantt_segment_count + 1) >= gantt_capacity) {
            gantt_capacity *= 2;
            gantt_segments = (struct GanttSegment *)realloc(gantt_segments, gantt_capacity * sizeof(struct GanttSegment));
        }

        // add a Gantt segment for the current process
        gantt_segments[gantt_segment_count].start_time = current_time - execution_time;
        gantt_segments[gantt_segment_count].end_time = current_time;
        gantt_segments[gantt_segment_count].pid = processes[current_process_index].pid;
        gantt_segment_count++;

        // check if the current process has completed
        if (processes[current_process_index].remaining_time == 0) {
            processes[current_process_index].completion_time = current_time;
            processes[current_process_index].turnaround_time = processes[current_process_index].completion_time - processes[current_process_index].arrival_time;
            processes[current_process_index].waiting_time = processes[current_process_index].turnaround_time - processes[current_process_index].run_time;
            completed_processes++;
        }

        // move to the next process in the Round Robin queue
        current_process_index++;
    }

    // print the Gantt chart
    print_gantt_chart(gantt_segments, gantt_segment_count);

    // free the allocated memory for Gantt chart segments
    free(gantt_segments);

}

int main() {
    // get the number of processes from the user
    int num_processes;
    printf("Enter the number of processes: ");
    scanf("%d", &num_processes);

    // exit for invalid number of processes
    if (num_processes < 1 || num_processes > 20) {
        printf("Invalid number of processes.\n");
        return 1;
    }

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

    // execute the selected scheduling algorithm
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

    print_process_stats(processes, num_processes);

    free(processes);

    return 0;
}
