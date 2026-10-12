/*
 * Name: William Cady
 * ID: 1017671200
 * Date: 10/11/2023
 *
 * Problem 2: Multi-Level Feedback Queue (MLFQ)
 *
 * Summary:
 *   - Simulate the MLFQ algorithm
 *   - Read in Q, N, and N jobs (pid, arrival_time, run_time)
 *   - Simulate one time unit per loop pass, until all jobs are finished
 *   - Print a Gantt chart and its time markers
 *   - Assume 1 <= Q <= 10, 1 <= N <= 20 from the project spec
 *   - First job in the input is the first to arrive, and all jobs have unique pids
 *   - If a job has run for its entire run_time, it is removed from the queue
 *   - If a job has run for its time slice, it is demoted to the next lower queue (unless it's already at the lowest queue)
 *   - Once a job is moved to the next queue, it is placed at the back of that queue
 *   - Gantt chart highlights the job that ran in each time unit
 * 
 * 
 *
 * Challenges:
 *   - Indexing of head[]
*    - Keeping track of the current job and its level
*    - Keeping track of the total number of segments
*    - time units management
*    - design choices beyond notes
 *
 * Solutions:
 *   - Double checking my indexing and logic for head[] and current job
 *   - Using a while loop to find the first non-empty queue and its front job
 *   - Using a totalNum variable to keep track of the number of segments in the Gantt
 *   - Defining time units as a variable t and incrementing it each loop pass to simulate passing time, each loop pass is one time unit
 *   - Using a break statement to exit the loop when all jobs are finished
 *   - As for design choices, I tried to follow the notes and the project spec as closely as possible
 *     But I also chose to do doubling for the time slices and some finagling with additional variables to keep track info for chart and MLFQ
 *     Specifically: arrival_time, run_time, used, and totalNum were additions to class notes on struct
 * 
 */

// Libraries used in Lab 5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Lab 5 p.7 (struct node_t), p.20 (extra counter field, like wins)
struct node_t {
    int pid;    // which job it is 
    int arrival_time; // when it arrives
    int run_time; // how long it needs to run (or still needs)
    int used; // CPU time used at level
    struct node_t *next;
};

// Lab 5 p.8 (head); one per queue. Level Q-1 is the top queue, level 0 the bottom (MLFQ p.6)
struct node_t *head[10] = {NULL};

// Note on input, assumed 1<=Q<=10, 1<=N<=20

/*
 * insert
 *
 * Reference:
 *   Lab 5 p.9 (insert); MLFQ p.4 (Rule 2)
 * Input:
 *   level: queue to be joined
 *   newNode: job to be added
 * Output:
 *   none
 * Function:
 *   appends newNode to the back of head[level] -> RR Queue
 */
void insert(int level, struct node_t *newNode) { // passes level and node
    struct node_t *curr = head[level];

    newNode->next = NULL; // newNode will always be at the end of the list, so its next will be NULL

    if (curr == NULL) {  // Actually an edge case, empty list where curr would be Null, meaning curr->next would be a bad bad
        head[level] = newNode;
        return;
    }

    while (curr->next) { // this will keep going until curr->next is NULL.. last node
        curr = curr->next;
    }

    curr->next = newNode; // insert at the end
}

/*
 * print_gantt
 *
 * Reference:
 *   Lab 5 p.10 (print_list)
 * Input:
 *   pids: pid of each chart segment (-1 = IDLE)
 *   starts: start time
 *   ends: end time
 *   totalNum: total number of chart segments
 * Output:
 *   Gantt chart and its time markers
 * Function:
 *   prints one line of segment labels, then one line of start times
 *   followed by the final end time
 */
void print_gantt(int pids[], int starts[], int ends[], int totalNum) {
    int i = 0;
    // loop to print all existing pids
    while (i < totalNum) {
        if (pids[i] < 0)
            printf("| IDLE ");
        else
            printf("| P%d ", pids[i]);
        i++;
    }

    printf("|\n"); // break

    int j = 0;
    // loop to print pids start times
    while (j < totalNum) {
        printf("%d ", starts[j]);
        j++;
    }

    printf("%d\n", ends[totalNum - 1]); // final end time (forgor to -1 originally because we index from 0)
}

/*
 * allocProcess
 *
 * Reference:
 *   Lab 5 p.9 (malloc + assert); Lab 6 p.10 (malloc)
 * Input:
 *   size: most segments the chart can need
 *   pids, starts, ends: addresses of the three chart arrays
 * Output:
 *   none (the arrays are set through the pointers)
 * Function:
 *   allocates the three chart arrays with room for size entries
 */
void allocProcess(int size, int **pids, int **starts, int **ends) {
    *pids = malloc(size * sizeof(int));
    *starts = malloc(size * sizeof(int));
    *ends = malloc(size * sizeof(int));
    assert(*pids != NULL && *starts != NULL && *ends != NULL);
}

/*
 * freeProcess
 *
 * Reference:
 *   Lab 6 p.9 (vector_free)
 * Input:
 *   pids, starts, ends: addresses of the three chart arrays
 * Output:
 *   none
 * Function:
 *   frees the three chart arrays and sets them to NULL
 */
void freeProcess(int **pids, int **starts, int **ends) {
    free(*pids);
    free(*starts);
    free(*ends);
    *pids = NULL;
    *starts = NULL;
    *ends = NULL;
}

/*
 * main
 *
 * Reference:
 *   Lab 5 p.6 (main), p.14 (loop); MLFQ p.4, p.7, p.13, p.14
 *   (Rules 1, 3, 4; time slice)
 * Input:
 *   stdin: Q, N, then N lines of "pid arrival_time run_time"
 * Output:
 *   Gantt chart and its time markers on stdout
 * Function:
 *   simulates MLFQ one time unit per loop pass
 */
int main(int argc, char *argv[]) {

    int Q, N; // number of queues, number of jobs

    if (scanf("%d %d", &Q, &N) != 2) { // read in Q and N (scanf returns 2 when both were read), exit with error code 1 if not
        return 1;
    }

    // array of jobs, project sepcifies 1 <= N <= 20
    struct node_t job[20];
    int i = 0;
    int sum = 0; // total run time of all jobs

    // loop to read in every job
    while (i < N) {
        scanf("%d %d %d", &job[i].pid, &job[i].arrival_time, &job[i].run_time);
        job[i].used = 0;
        sum += job[i].run_time;
        i++;
    }

    // Params for the Gantt chart, worst case is one segment per time unit
    // run plus one IDLE gap per job
    int *pids, *starts, *ends;
    allocProcess(sum + N, &pids, &starts, &ends);
    int totalNum = 0;

    // Params for the MLFQ
    int t = 0; // current time
    int finished = 0;
    int level = 0;
    struct node_t *current = NULL; // job that ran last time


    // loop for each time unit until all jobs are finished
    while (1) {

        // MLFQ p.7, Rule 3: Arrivals (arrival time = current time)
        // jobs that arrive enter at the back of the highest queue (Q-1)
        int j = 0;
        while (j < N) {
            if (job[j].arrival_time == t) {
                insert(Q - 1, &job[j]);
            }
            j++;
        }

        // MLFQ p.4, Rule 1 and p.13, Rule 4: Settling (what happens to job that ran last time)
        // job that ran last time unit: finished, used its slice, or preempted
        if (current) {
            int slice = 1 << (Q - 1 - level);   // MLFQ p.14.. time slice, doubling for every lower queue 
            int higher = 0; // effective bool for if there is a job waiting in a higher queue


            int k = level + 1;
            // check for any job waiting in a higher queue
            while (k < Q) {
                if (head[k]) {
                    higher = 1; // process in higher q found, so preempt current job
                }
                k++;
            }

            if (current->run_time == 0 || current->used >= slice || higher) { // finished, used its slice (MLFQ p.13, Rule 4), or preempted (MLFQ p.4, Rule 1)
                head[level] = current->next; // take it off the front

                // if it's finished, count it. Otherwise, if it used its slice, demote it to the next lower queue (unless it's already at the lowest queue)
                if (current->run_time == 0) {
                    finished++;
                } else {
                    if (current->used >= slice) {   // MLFQ p.13, Rule 4
                        current->used = 0;
                        if (level > 0) {
                            level--; // demote: the insert below puts it in the lower queue
                        }
                    }
                    insert(level, current); // back of its queue -> RR
                }
            }
        }

        // Just checks if all processes are finished
        if (finished == N) {
            break;
        }

        // Pick a job to run, run it for one time unit, and then update chart
        // MLFQ p.4, Rule 1: highest non-empty queue, front job runs (Rule 2)
        current = NULL; // reset
        level = Q - 1; // highest queue
        // this loop goes top to bottom queues to find first non-empty and takes first job in it
        while (level >= 0) {
            if (head[level]) { // head[level] is not NULL, so there is a job waiting in this queue
                current = head[level];
                break;
            }
            level--; // next lower queue
        }

        // run one time unit (pid -1 = IDLE)
        int pid = -1; // initialized to IDLE
        // if there is a job to run, decrement its run time and increment its used time
        if (current) {
            current->run_time--;
            current->used++;
            pid = current->pid; // set pid to the job that ran
        }

        // extend the last chart segment, or start a new one
        if (totalNum > 0 && pids[totalNum - 1] == pid) { // if the last segment is the same as the current one, extend it
            ends[totalNum - 1] = t + 1;
        } else { // otherwise, start a new segment and increment totalNum
            pids[totalNum] = pid;
            starts[totalNum] = t;
            ends[totalNum] = t + 1;
            totalNum++;
        }
        t++; // next time unit
    }

    print_gantt(pids, starts, ends, totalNum); // prints chart with helper method
    freeProcess(&pids, &starts, &ends); // frees existing

    
    return 0; // solid completion of program
}
