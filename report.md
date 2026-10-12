# Problem 2: Multi-Level Feedback Queue (MLFQ)

**Name:** William Cady  **ID:** 1017671200  **Course:** CPS 356

## 1. Design Choices

The project defines the input as: Q (Layers), N(Number of Jobs), and N lines of pid (Job Process ID), arrival_time (time it enters the system), and run_time (CPU time the job needs). The project specifies a Gantt chart output, but it leaves the time quantum, boost period, preemption, tie-breaking, idle handling and chart looks pretty open ended. I chose the following informed primarily by Lecture 5 Scheduling Multi-level Feedback Queue and Lab 5 powerpoints. For additional context I read a padding around the primarily material, Lectures 4, 6, and 7 on Scheduling Introduction, Scheduling Proportional Share, and Multiprocessor Scheduling. This gave me a good baseline to jump into my design choices for the MLFQ:

### Time Slices
 I chose to have the slice double for each additional lower level. 1 at the highest, then 2, then 4, and so on. This follows the tuning slide (MLFQ p.14), where high queues get short slices and low queues longer ones, I based the doubling implementation on the example slices being 10ms, then 20ms, then 40ms. In summary, the shorter jobs finish at the top, and long jobs switch less the lower the go down.

### Queues
 Queue Q-1 is the highest priority and queue 0 the lowest, matching the slides' convention (MLFQ p.6). My indexing convention is due to indexing at zero, unlike the example which starts at 1.. therefore, to have 5 Queues is to index at 0, 1, 2, 3, and 4. Keeping track of that logic was important in correcting my indexing errors.

### Preemption and Tie Breakers
 Preemption means I am taking the CPU away from a job that hasn't completed yet. The interupted job is put back into the queue while another runs, and in this implementation is immediate. If a job is waiting in a higher queue, the running job leaves the CPU at the end of the current time unit (when I loop around to check again). It keeps its level and the time it has already used, and goes to the back of its queue. If an arrival and a requeue happen at the same time (A Tie Situation), the new arrival joins its queue first. When there are mutliple jobs on the same level (same queue), selection is RR. To maintain RR, a job is always inserted at the back of its queue, and the front job runs until it finishes, uses its full slice, or is preempted. Only a job that uses its full slice is demoted. A preempted job stays at its level, and at the bottom queue a job that uses its slice goes to the back of the same queue.


### Output
 Output is my attempt at a terminal printed Gantt Chart. A time unit without a job is shown as IDLE. The output is one line of segments I bifracated with pipes, then one line of time markers with the start of each segment, then the final end time. If the same job runs consecutively, the units of that job merge into one segment.


### Boosting 
  The project specified input has no boost period S, so I did not implement Rule 5 (MLFQ p.12). I compared a version with a boost against my output for the spec's example input, and adding a boost changes the chart unless the boost period is larger than the example (every S from 1 to 21 changes the chart, S >= 22 leaves it unchanged because it never fires in time). For example, with S=10 the chart first differs at t=10. I used rules 1-4 of the refined rule set (MLFQ p.16). It uses the rewritten Rule 4, where a job is demoted once it uses its allotment "regardless of how many times it has given up the CPU" (MLFQ p.13). It does not use the original Rule 4a/4b, which a job can game (MLFQ p.11).




## 2. Solution Explanation

I keep one linked list per level in head[], in the style of the struct from Lab 5. But, I also added arrival_time, run_time and used to the node for the goal of ultimately creating the chart (run_time decrements and used increments with every loop pass, which I refer to as a time unit). The chart is stored in pids (process name array), starts (start times array), and ends (array of end times) from allocProcess. allocProcess is my function that allocates the space for them, and freeProcess releases them. The main loop makes one pass per time unit in five steps: 
1) look at new arrivals
2) settle the job that just ran (finished, demoted or preempted)
3) stop if every job is finished
4) otherwise pick the next job
5) run it for one unit and update the chart.

These steps seen in the main loop are derived from:

**Rule 3 (MLFQ p.7)**: New jobs enter at the top. Each time unit, every job with arrival_time equal to the current time unit "t" is inserted into head[Q-1], the highest queue/level.

**Rule 1 (MLFQ p.4)**: Take top jobs first. Selection walks head[] from Q-1 down and takes the first non-empty queue. In the settling step, the variable, higher, indicates when any queue above the job's level is non-empty, and that preempts the job running.

**Rule 2 (MLFQ p.4)**: Round robin at every level. The front job of the chosen queue runs. The meat of this depends on the insert() method. It always appends at the back. A job that leaves the CPU unfinished is removed from the front and reinserted at the back, so every level, especially the bottom, is First In First Out round robin.

**Rule 4 (MLFQ p.13)**: Demoting long jobs. The previously mentioned variable, used, counts CPU time at its current level. Slices are defined as 1 << (Q - 1 - level), which shifts 1 left by (Q-1-level) places, giving 2^(Q-1-level): 1 at the top queue, doubling at each level below it. This was apart of my design choices based on the lecture example. Effectively:

```c
if (current->used >= slice) {   // MLFQ p.13, Rule 4
    current->used = 0;          // Reset for next time
    if (level > 0) level--;     // level 0 stays at 0, nothing less than bottom
}
insert(level, current);         // back of its queue -> RR
```

It is important that used is reset ONLY on demotion, never on preemption. This prevents avoiding demotion by giving up early.

## 3. Results

**Case A: the spec's example** (Q=5; P1 start 0 for 8, P2 start 1 for 4, P3 start 2 for 9, P4 start 3 for 5):

```
| P1 | P2 | P3 | P4 | P1 | P2 | P3 | P4 | P1 | P2 | P3 | P4 | P1 | P3 |
0 1 2 3 4 6 8 10 12 16 17 21 23 24 26
```

This matches what I would expect given the slices. At the highest level the slice is only one, therefore each job runs once, 0-4 time units. After being demoted to the next queue with slice 2, they run for the 4-12 time units. Demoted again, now with slice 4, P1 runs 12-16, P2 finishes at 17, P3 runs 17-21 and P4 finishes at 23. demoted for the third time with slice 8, P1 finishes at 24 and P3 at 26. across the queues from top to bottom, the jobs used 4 (q4), 8 (q3), 11 (q2) and 3 (q1) time units, totaling 26. we finished at t=26, therefore, there was no IDLE time in the example.

To better visualize the output as a markdown table by queue:

| Queue | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| q4 | P1 | P2 | P3 | P4 |  |  |  |  |  |  |  |  |  |
| q3 |  |  |  |  | P1 | P1 | P2 | P2 | P3 | P3 | P4 | P4 |  |
| q2 |  |  |  |  |  |  |  |  |  |  |  |  | P1 |
| q1 |  |  |  |  |  |  |  |  |  |  |  |  |  |
| q0 |  |  |  |  |  |  |  |  |  |  |  |  |  |

| Queue | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 |
|---|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
| q4 |  |  |  |  |  |  |  |  |  |  |  |  |  |
| q3 |  |  |  |  |  |  |  |  |  |  |  |  |  |
| q2 | P1 | P1 | P1 | P2 | P3 | P3 | P3 | P3 | P4 | P4 |  |  |  |
| q1 |  |  |  |  |  |  |  |  |  |  | P1 | P3 | P3 |
| q0 |  |  |  |  |  |  |  |  |  |  |  |  |  |

We see that all start at the highest queue and run for a single slice, then double that time in the next queue, and again in the following till the run their total amount of time units.

## 4. Analysis

For case A, I define turnaround time as completion minus arrival, and response time as first run minus arrival (scheduling intro lecture).

| Job | Arrival | Completion | Turnaround | Response |
|-----|---------|------------|------------|----------|
| P1 | 0 | 24 | 24 | 0 |
| P2 | 1 | 17 | 16 | 0 |
| P3 | 2 | 26 | 24 | 0 |
| P4 | 3 | 23 | 20 | 0 |

The average turnaround is (24+16+24+20)/4 = 21 and every response time is 0 by definition.

My results support the claim made in MLFQ lecture page 7 that the MLFQ approximates SJF without knowing run times. P2, the shortest job, finishes first (time units=17) even though it arrived second, while the longest jobs, P3 and P1, sink furthest (to q1) and finish last. A job that keeps using its slice must be long, thus the scheduler demotes it. This response is also good because every job starts in the top queue, every job's response time is 0: it runs at the moment it gets there, because new jobs enter the top queue (Rule 3, MLFQ p.7) and preempt anything below them.

The true cost is switches. Four jobs need 14 chart segments in case A, because short slices at the top interleave the jobs before they settle into longer slices. A real system would pay overhead on each switch, something my solution does not account for. To measure the trade-off, I ran the same spec example with a fixed slice of 1 at every level. It produces 25 segments (24 switches), while my doubling slices produce 14 (13 switches). Both finish at t=26. Doubling cuts switches by almost half without delaying completion, which is the trade-off on the tuning slide (MLFQ p.14).

An additional core weakness stems from having no boost. The enhanced verisions of the MLFQ look to fullfil Rule 5 and its fight against starvation (MLFQ p.11). A long job that has sunk to a low queue only runs when every queue above it is empty, so if short jobs keep arriving at the top, it can wait a long time without getting any work done. I measured this with a starvation test case where Q=3, N=12; P1 arrives at 0 with run_time 20, and P2 through P12 arrive every two time units with run_time 2 each. I found that P1 runs only at t=0-2 and 3-4, then waits 21 time units (t=4 to 25) while the short jobs take turns, and finishes at t=42.

A boost every S time units (MLFQ p.12) would move such a job back to the top and guarantee it some CPU time, at the price of an extra parameter in the project input. I tested some boost implementations as discussed in the design section. However, I wanted to keep graders test cases in mind as well, any boost I arbitrarily choose to hardcode fundamentally changes the gantt chart output that would be expected. Thus, I understand how boost functions and the problem it sets out to solve, however, with at most 20 jobs the starvation is bounded, so I left Rule 5 out. With continuous new processes, longer ones sitting in the lowest priority queue defined by input Q would starve, greatly increasing that job's turnaround time.

Per my code implementation, each iteration scans all N jobs for arrivals, up to Q queues for preemption and selection, and up to N list nodes in the insert function. The run is therefore O((N+Q)*T) for T total time units, which is more than fine for the projects bounded N and Q. Some final limitations of the project are that all times are integers expressed as time units, and there is no boost hardcoded because of uncertainty in arrival runtime range and finite number of jobs as a whole.
