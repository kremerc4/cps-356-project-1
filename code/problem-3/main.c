#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_CPUS 8
#define MAX_N 30
#define MAX_INTERVALS 100

typedef struct {
    int pid;
    int arrival_time;
    int run_time;
    int remaining_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    bool completed;
} Process;

typedef struct {
    int pid;
    int start;
    int end;
} ExecutionInterval;

typedef struct {
    int items[MAX_N];
    int front;
    int rear;
    int count;
} ReadyQueue;

void init_queue(ReadyQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

bool is_empty(ReadyQueue *q) {
    return q->count == 0;
}

void enqueue(ReadyQueue *q, int val) {
    q->rear++;
    q->items[q->rear] = val;
    q->count++;
}

int dequeue(ReadyQueue *q) {
    int val = q->items[q->front];
    q->front++;
    q->count--;
    return val;
}

/* Steal from the tail/back of the victim's ready queue */
int steal_from_tail(ReadyQueue *q) {
    int val = q->items[q->rear];
    q->rear--;
    q->count--;
    return val;
}

typedef struct {
    ReadyQueue queue;
    int current_running_pid; // -1 if idle
    int busy_until;
    ExecutionInterval intervals[MAX_INTERVALS];
    int interval_count;
} CPU;

int main(void) {
    int M, N;
    if (scanf("%d", &M) != 1) return 0;
    if (scanf("%d", &N) != 1) return 0;

    Process proc[MAX_N];
    for (int i = 0; i < N; i++) {
        scanf("%d %d %d", &proc[i].pid, &proc[i].arrival_time, &proc[i].run_time);
        proc[i].remaining_time = proc[i].run_time;
        proc[i].completed = false;
    }

    CPU cpus[MAX_CPUS];
    for (int i = 0; i < M; i++) {
        init_queue(&cpus[i].queue);
        cpus[i].current_running_pid = -1;
        cpus[i].busy_until = 0;
        cpus[i].interval_count = 0;
    }

    int successful_steals = 0;
    int unsuccessful_steals = 0;
    int completed_processes = 0;
    int current_time = 0;

    bool assigned[MAX_N] = { false };

    while (completed_processes < N) {
        // 1. Check process arrivals at current_time and distribute round-robin
        for (int i = 0; i < N; i++) {
            if (!assigned[i] && proc[i].arrival_time <= current_time) {
                int target_cpu = proc[i].pid % M; // Initial assignment policy
                enqueue(&cpus[target_cpu].queue, i);
                assigned[i] = true;
            }
        }

        // 2. Process completions on CPUs
        for (int c = 0; c < M; c++) {
            if (cpus[c].current_running_pid != -1 && cpus[c].busy_until == current_time) {
                int p_idx = cpus[c].current_running_pid;
                proc[p_idx].completion_time = current_time;
                proc[p_idx].turnaround_time = proc[p_idx].completion_time - proc[p_idx].arrival_time;
                proc[p_idx].waiting_time = proc[p_idx].turnaround_time - proc[p_idx].run_time;
                proc[p_idx].completed = true;
                completed_processes++;
                cpus[c].current_running_pid = -1;
            }
        }

        if (completed_processes == N) break;

        // 3. Dispatch or attempt work-stealing for idle CPUs
        for (int c = 0; c < M; c++) {
            if (cpus[c].current_running_pid == -1) {
                // If local queue is empty, attempt work stealing
                if (is_empty(&cpus[c].queue)) {
                    // Check other CPUs to steal
                    bool stole = false;
                    for (int step = 1; step < M; step++) {
                        int victim = (c + step) % M;
                        // Requirement: Target CPU must have > 1 runnable process in its queue
                        if (cpus[victim].queue.count > 1) {
                            int stolen_idx = steal_from_tail(&cpus[victim].queue);
                            enqueue(&cpus[c].queue, stolen_idx);
                            successful_steals++;
                            stole = true;
                            break;
                        }
                    }
                    if (!stole) {
                        // Count as an unsuccessful steal attempt if there were any runnable jobs anywhere
                        bool jobs_exist = false;
                        for (int k = 0; k < M; k++) {
                            if (cpus[k].queue.count > 0) jobs_exist = true;
                        }
                        if (jobs_exist) {
                            unsuccessful_steals++;
                        }
                    }
                }

                // If a process is ready in the local queue, schedule it
                if (!is_empty(&cpus[c].queue)) {
                    int next_p = dequeue(&cpus[c].queue);
                    cpus[c].current_running_pid = next_p;
                    cpus[c].busy_until = current_time + proc[next_p].run_time;

                    // Record execution interval
                    int ic = cpus[c].interval_count++;
                    cpus[c].intervals[ic].pid = proc[next_p].pid;
                    cpus[c].intervals[ic].start = current_time;
                    cpus[c].intervals[ic].end = cpus[c].busy_until;
                }
            }
        }

        current_time++;
    }

    // Output Specification
    printf("Multy-Processor Work-Stealing Results\n");

    printf("Gantt Charts by CPU\n");
    for (int c = 0; c < M; c++) {
        printf("CPU %d: ", c);
        if (cpus[c].interval_count == 0) {
            printf("IDLE\n");
            continue;
        }
        for (int i = 0; i < cpus[c].interval_count; i++) {
            printf("[P%d: %d-%d] ", cpus[c].intervals[i].pid,
                   cpus[c].intervals[i].start, cpus[c].intervals[i].end);
        }
        printf("\n");
    }

    printf("Process Execution Intervals & Completion Time\n");
    printf("%-6s %-14s %-10s %-16s %-16s %-16s\n",
           "PID", "Arrival Time", "Run Time", "Completion Time", "Waiting Time", "Turnaround Time");

    double total_wt = 0;
    double total_tat = 0;
    for (int i = 0; i < N; i++) {
        total_wt += proc[i].waiting_time;
        total_tat += proc[i].turnaround_time;
        printf("%-6d %-14d %-10d %-16d %-16d %-16d\n",
               proc[i].pid, proc[i].arrival_time, proc[i].run_time,
               proc[i].completion_time, proc[i].waiting_time, proc[i].turnaround_time);
    }

    printf("Average Waiting Time:           %.2f\n", total_wt / N);
    printf("Average Turnaround Time:        %.2f\n", total_tat / N);
    printf("Number of Successful Steals:    %d\n", successful_steals);
    printf("Number of Unsuccessful Steals:  %d\n\n", unsuccessful_steals);

    return 0;
}