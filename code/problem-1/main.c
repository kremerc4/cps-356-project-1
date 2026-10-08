#include <stdio.h>
#include <stdlib.h>

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
            // FCFS scheduling
            break;
        case 2:
            // SJF scheduling
            break;
        case 3:
            // STCF scheduling
            break;
        case 4:
            // Round Robin scheduling
            break;
        default:
            printf("Invalid scheduling algorithm selected.\n");
            return 1;
    }

    return 0;
}
