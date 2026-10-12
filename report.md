# Problem 3: Multiprocessor Scheduling with Work Stealing

## 1. System Overview

This program simulates a multiprocessor CPU scheduler with work stealing using multiple independent ready queues.

### Key Rules and Design:
- **System Setup:** The system has M processors (CPU 0 to CPU M-1) and N processes.
- **Per-CPU Ready Queues:** Instead of a single shared queue, each CPU has its own private ready queue. This reduces contention and avoids bottlenecks.
- **Initial Assignment:** When a process arrives, it is assigned to a CPU using modular hashing:
  `CPU = pid % M`
- **Local Scheduling:** Each CPU runs processes from its own queue non-preemptively using First-Come, First-Served (FCFS).
- **Work Stealing:** When a CPU finishes its current job and its ready queue is empty, it becomes idle and tries to steal work from other CPUs.
- **Stealing Rules:**
  - The idle CPU checks other CPUs in round-robin order: `(c + 1) % M`, `(c + 2) % M`, and so on.
  - It can only steal from a target CPU if that CPU has **more than one** process waiting in its ready queue (`queue.count > 1`).
  - If a steal is allowed, the process is stolen from the **tail** (back) of the target queue and placed into the idle CPU's queue.

---

## 2. Implementation Details

The simulation runs in a discrete time loop tick by tick (`current_time = 0, 1, 2, ...`).

### Data Structures:
- **Process:** Stores process ID (pid), arrival time, run time, completion time, waiting time, and turnaround time.
- **ReadyQueue:** An array-based queue that supports `enqueue`, `dequeue` (from the front), and `steal_from_tail` (from the back).
- **CPU:** Stores the CPU's ready queue, the currently running process ID, the time when it will be done (`busy_until`), and a log of execution intervals for the Gantt chart.

### Step-by-Step Loop at Each Clock Tick:
1. **Arrivals:** Any process arriving at or before the current time is placed into its assigned CPU's ready queue using `pid % M`.
2. **Completions:** Any CPU whose current job reaches its completion time (`busy_until == current_time`) marks the process as finished, calculates waiting and turnaround times, and becomes idle.
3. **Work Stealing & Dispatch:**
   - If a CPU is idle and its queue is empty, it probes other CPUs in order. If a CPU has `count > 1`, it steals the process at the tail. If no steal succeeds but other queues still have runnable jobs, an unsuccessful steal is counted.
   - If a CPU is idle and has work in its queue, it dequeues the process from the front, starts executing it, and sets `busy_until = current_time + run_time`.
4. The clock advances by 1 (`current_time++`) until all N processes have completed.

---

## 3. Results & Output

We tested the program using the project test input:
- M = 3 CPUs
- N = 5 processes

### Input Data:
- Process 1: PID 1, Arrival Time = 0, Run Time = 8 -> assigned to CPU 1 (1 % 3 = 1)
- Process 2: PID 2, Arrival Time = 0, Run Time = 4 -> assigned to CPU 2 (2 % 3 = 2)
- Process 3: PID 3, Arrival Time = 0, Run Time = 9 -> assigned to CPU 0 (3 % 3 = 0)
- Process 4: PID 4, Arrival Time = 1, Run Time = 5 -> assigned to CPU 1 (4 % 3 = 1)
- Process 5: PID 5, Arrival Time = 2, Run Time = 3 -> assigned to CPU 2 (5 % 3 = 2)

### Gantt Charts by CPU:
- **CPU 0:** [P3: 0-9]
- **CPU 1:** [P1: 0-8] [P4: 8-13]
- **CPU 2:** [P2: 0-4] [P5: 4-7]

### Process Metrics:
- **Process 1:** Arrival = 0, Run Time = 8, Completion Time = 8, Waiting Time = 0, Turnaround Time = 8
- **Process 2:** Arrival = 0, Run Time = 4, Completion Time = 4, Waiting Time = 0, Turnaround Time = 4
- **Process 3:** Arrival = 0, Run Time = 9, Completion Time = 9, Waiting Time = 0, Turnaround Time = 9
- **Process 4:** Arrival = 1, Run Time = 5, Completion Time = 13, Waiting Time = 7, Turnaround Time = 12
- **Process 5:** Arrival = 2, Run Time = 3, Completion Time = 7, Waiting Time = 2, Turnaround Time = 5

### Summary Statistics:
- **Average Waiting Time:** 1.80
- **Average Turnaround Time:** 7.60
- **Number of Successful Steals:** 0
- **Number of Unsuccessful Steals:** 1

---

## 4. Stealing Behavior Analysis

In this run, there were 0 successful steals and 1 unsuccessful steal attempt:

- At time **t = 4**, CPU 2 finishes P2. CPU 2's own queue already has P5 waiting, so CPU 2 immediately runs P5 without needing to steal.
- At time **t = 7**, CPU 2 finishes P5 and becomes idle with an empty queue. It begins probing other CPUs:
  - It checks CPU 0: CPU 0 is busy running P3, but its ready queue has 0 waiting jobs.
  - It checks CPU 1: CPU 1 is busy running P1, and its ready queue has 1 waiting job (P4).
- Because the rule requires **strictly more than 1 process** (`count > 1`), CPU 2 cannot steal P4 from CPU 1.
- Since runnable work (P4) still exists in the system on CPU 1, this failed attempt is recorded as 1 unsuccessful steal.
- At time **t = 8**, CPU 1 completes P1 and immediately starts running P4. From this point forward, all ready queues in the system remain empty, so no further steals can occur.

### Why the `count > 1` Rule Matters:
If CPU 2 were allowed to steal P4 at t = 7:
- CPU 2 would start running P4.
- Just one time unit later (at t = 8), CPU 1 would finish P1 and find its queue empty.
- CPU 1 would then become idle and attempt to steal work back.
This would cause unnecessary task migration and cache invalidation. Requiring `count > 1` ensures that a CPU only gives up work when it has enough jobs to keep itself busy.

---

## 5. Challenges & Design Takeaways

1. **Stealing from the Tail:** Local scheduling takes from the front (head) of the queue, while steals take from the back (tail). The newest task at the tail is the least likely to have warm cache on the victim CPU, making it the best candidate to migrate.
2. **Preventing Thrashing:** The condition `count > 1` prevents tasks from ping-ponging back and forth between processors.
3. **Deterministic Execution:** Handling events at each tick in a strict sequence (Arrivals -> Completions -> Steals -> Dispatches) avoids race conditions and ensures repeatable, reliable simulation results.
