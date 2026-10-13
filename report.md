# Problem 1: Classic CPU Scheduling Algorithms

## 1. Solution Requirements and Design

### Background

For this section, we wanted to visualize several of the fundamental CPU scheduling algorithms, specifically
- First-Come, First-Served (FCFS)
- Shortest Job First (SJF)
- Shortest Time-to-Completion First (STCF)
- Round Robin (RR)

#### First-Come, First-Served (FCFS)

First-Come, First-Served is the most straight forward of the list, and behaves as it sounds. When a process arrives to the CPU, the CPU will process it to completion, then move onto the next process that arrived chronologically.

For example,

| PID | Arrival Time | Process Order|
| --- | --- | --- |
| 123 | 0 | 1 |
| 124 | 5 | 2 |
| 125 | 5 | 3 |

First-Come, First-Served is the simplest of these algorithms, but also the least optimized.

#### Shortest Job First (SJF)

Shortest Job First builds on the logic of FCFS, but adds a layer of complication by prioritizing the shorter tasks first if there are multiple tasks in the queue to be processed. The CPU will process the shortest task at any period, process it to completion, then it will move onto the next shortest task that has arrived.

For example,

| PID | Arrival Time | Length | Process Order|
| --- | --- | --- | --- |
| 123 | 0 | 20 | 3 |
| 124 | 0 | 5 | 2 |
| 125 | 0 | 3 | 1 |

The additional complexity that Shortest Job First adds allows for a faster turnaround for the shorter tasks, and ensures they don't have to wait until a much larger task completes first, like would be the case with FCFS. However, this algorithm falls short when the shorter tasks arrive just seconds after the long task.

#### Shortest Time-to-Completion First (STCF)

Shortest Time-to-Completion First builds on SJF, but will handle the shortcomings of arrival order by pausing processes to complete the shortest at anytime. This allows shorter tasks that arrive later to be prioritized over one long-running process that comes earlier. The long-running process will run up until the shorter tasks arrivals, then the long task will be paused until the shorter one has finished.

This additional complexity makes Shortest Time-to-Completion First one of the better algorithms for single CPU scheduling. However, this algorithm does fall short because it can result in certain long tasks never being processed to completion, if other shorter tasks keep arriving before it can complete.

#### Round Robin (RR)

Round Robin takes a different approach to task balancing, having the CPU work on all tasks that need to be worked on for a short period of time, then alternating between them all. The short period, called a scheduling quantum, will define specifically how long each task should get worked on.

The benefit to this approach is that no task will spend too long without being worked on some, thus maximizing turnaround.

### Algorithm Visualization

Now that we understand the behavior of the algorithms, we need to understand how we are going to visualize their performance. 

#### Getting Processes as Input

First, we are going to need a way to choose the processes we are using to visualize these algorithms. In this solution we are using user input, specifically the number of processes to visualize, and the specific PIDs, arrival times and processing times of each process to get our data to visualize. 

#### Performance Metrics as Output

Next, we are going to need a way to visually understand how each algorithm is processing our tasks, and a way to compare the algorithms for performance. To do this we are using a Gantt chart (a graphical chart showing over time what tasks are being worked on when), as well as comparing metrics such as waiting time and turnaround time. 

## 2. Implementation Details

### Approach

The solution I have taken is to get all our input, process it using a user-selected algorithm, then output the Gantt chart and other metrics to the user so they can analyze performance.

### Structs

Two structs were defined as part of this solution. One responsible for storing process specific information, and the other responsible for storing the individual segments that would make up the final Gantt chart.

The code snippets for each can be seen below.

``` c
struct Process {
    // user-specified process data
    int pid;            // specific ID for process
    int arrival_time;   // delta time process arrives to CPU
    int run_time;       // total time required to run process

    // simulation-related process data
    int remaining_time;     // time left to finish processing 
    int completion_time;    // delta time when task is completed
    int waiting_time;       // total time the process spent not running but arrived
    int turnaround_time;    // total time from arrival to completion that it took to process
};
```

``` c
struct GanttSegment {
    int start_time;     // start time of segment
    int end_time;       // end time of segment
    int pid;            // specific process ID for segment
};
```

The first three values of the `Process` struct are taken from the user input, and the remaining `Process` values and all the `GanttSegment` values are computed by the program.

### Algorithm Decision

The algorithm is decided by user input *(1 for FCFS, 2 for SJF, 3 for STCF, or 4 for Round Robin)*, and then a `switch-case` is used to call a specific function that corresponds to each algorithm.

### Algorithm Approach

Each algorithm has its own intricacies, but have shared approaches to representation. They all start by creating a tick to keep track of time, an allocated list of `GanttSegment`, and a variable to keep track of how many processes are completed. Then, there is the main while loop, which will run as long as there are uncompleted processes (`completed_processes < num_processes`). 

#### Within the Loop for FCFS, SJF & STCF
For these three algorithms, we first identify which process to run next *(FCFS being next in order based on arrival time and SJF & STCF being the shortest runtime available)*. Then we run each process, FCFS & SJF running to completion of that job, and STCF running on a second-by-second basis to allow for process pausing & switching as shorter processes arrive. 

After processing each section, we update the Gantt chart segments to reflect the run state.

If a process completes, we increment the number of completed processes, and continue until everything has been processed.

#### Within the Loop for Round Robin
For Round Robin, process selection behaves a little differently, we rotate through the process indexes, starting at 0, then increment every time quantum within the loop. If a process has not yet arrived, we simply increment to the next index, and continue processing.

We run each process for the dedicated quantum, update the Gantt chart, then continue to the next process.

If a process completes, we increment the number of completed processes, and continue until everything has been processed.

> One note to make with the Round Robin implementation is that we opted for a hard-coded quantum of 1 second, as outlined by the lecture slides.

#### Wrapping Up the Algorithm

After any of the algorithms run, we call our defined Gantt chart visualization, which utilizes our built Gantt segment list to print out the over all process.

Finally we free the memory that was being utilized to store the Gantt segments.

### Wrapping Up the Visualization

At the end of our visualization, we output some key metrics including waiting time and turnaround time for each PID and overall average waiting time and turnaround time for the entire algorithm. This gives us a better understanding of how each algorithm performed with our given processes.

Finally we free the memory that was being utilized to store the Processes.

## 3. Results

### FCFS with 3 Processes and Idle Time

```
Enter the number of processes: 3
Enter PID, arrival time, and run time for process 1: 1 1 2
Enter PID, arrival time, and run time for process 2: 2 2 3
Enter PID, arrival time, and run time for process 3: 3 3 4
Enter 1 for FCFS, 2 for SJF, 3 for STCF, or 4 for Round Robin: 1

Gantt Chart:
| Idle | P1   | P2   | P3   |
0      1      3      6      10     

Process Summaries:
PID: 1
        Waiting Time: 0
        Turnaround Time: 2
PID: 2
        Waiting Time: 1
        Turnaround Time: 4
PID: 3
        Waiting Time: 3
        Turnaround Time: 7

Process Average Summaries:
Average Waiting Time: 1.33
Average Turnaround Time: 4.33
```

This run shows three processes arriving after time 0, meaning that the CPU is idle until the first one arrives. The FCFS algorithm is running correctly, as processes are ran to completion based on their arrival time.

### SJF with 3 Processes Arriving at Time 0

```
Enter the number of processes: 3
Enter PID, arrival time, and run time for process 1: 1 0 4
Enter PID, arrival time, and run time for process 2: 2 0 3
Enter PID, arrival time, and run time for process 3: 3 0 2
Enter 1 for FCFS, 2 for SJF, 3 for STCF, or 4 for Round Robin: 2

Gantt Chart:
| P3   | P2   | P1   |
0      2      5      9      

Process Summaries:
PID: 1
        Waiting Time: 5
        Turnaround Time: 9
PID: 2
        Waiting Time: 2
        Turnaround Time: 5
PID: 3
        Waiting Time: 0
        Turnaround Time: 2

Process Average Summaries:
Average Waiting Time: 2.33
Average Turnaround Time: 5.33
```

This run shows the SJF algorithm running correctly, with each process being prioritized based on its length to completion.

### STCF with 1 Long Process Arriving Before 2 Short Processes

```
Enter the number of processes: 3
Enter PID, arrival time, and run time for process 1: 1 0 15
Enter PID, arrival time, and run time for process 2: 2 5 2
Enter PID, arrival time, and run time for process 3: 3 5 3
Enter 1 for FCFS, 2 for SJF, 3 for STCF, or 4 for Round Robin: 3

Gantt Chart:
| P1   | P2   | P3   | P1   |
0      5      7      10     20     

Process Summaries:
PID: 1
        Waiting Time: 5
        Turnaround Time: 20
PID: 2
        Waiting Time: 0
        Turnaround Time: 2
PID: 3
        Waiting Time: 2
        Turnaround Time: 5

Process Average Summaries:
Average Waiting Time: 2.33
Average Turnaround Time: 9.00
```

This run shows STCF running correctly, with the long process 1 being paused once processes 2 & 3 arrive, then process 1 resuming once processes 2 & 3 are completed.

### Round Robin with 3 Processes

```
Enter the number of processes: 3
Enter PID, arrival time, and run time for process 1: 1 0 4
Enter PID, arrival time, and run time for process 2: 2 0 4
Enter PID, arrival time, and run time for process 3: 3 0 4
Enter 1 for FCFS, 2 for SJF, 3 for STCF, or 4 for Round Robin: 4

Gantt Chart:
| P1   | P2   | P3   | P1   | P2   | P3   | P1   | P2   | P3   | P1   | P2   | P3   |
0      1      2      3      4      5      6      7      8      9      10     11     12     

Process Summaries:
PID: 1
        Waiting Time: 6
        Turnaround Time: 10
PID: 2
        Waiting Time: 7
        Turnaround Time: 11
PID: 3
        Waiting Time: 8
        Turnaround Time: 12

Process Average Summaries:
Average Waiting Time: 7.00
Average Turnaround Time: 11.00
```

This run shows Round Robin running correctly, with each of the three processes alternating every quantum (1 second), until they are completed.

## 4. Challenges

There were three main challenges I faced when implementing this solution.

The first was the initial process selection algorithm. Since it is not guaranteed that user input will be sorted by arrival time (for FCFS) or shortest run time (for SJF & STCF), and there is the possibility for idle time between each process, there are a lot of situations that need to be accounted for within the actual process selection logic. Being sure to cover each case required a lot of testing to ensure that everything was up to the expectation.

The second was having to reallocate the Gantt segment list for STCF and Round Robin. Unlike with FCFS and SJF where once a process is selected it will be ran to completion, STCF and Round Robin have the possibility of having more Gantt segments than there are actual processes. Because of this, we had to either arbitrarily create padding (which is not a very robust approach), or reallocate the segments. I opted for reallocating, and basing on an algorithm I had heard of for ArrayLists (if I remember correctly). When we are approaching the limit of what we can store in memory (one less than the limit in this implementation), double the size of allocated memory for the list. This allows us to ensure we will never have a memory leak within our program. This was a great opportunity to apply some of what we have been talking about in class, but it is still very new to me so it was definitely a challenge to overcome.

The final challenge I faced was implementing the Gantt chart visualization. I wanted to have the bars be dynamically sized based on runtime, but that proved to be much harder than I had time to feasibly develop. Instead I opted for fixed padding for each segment, to ensure a consistent but aesthetically pleasing visualization.

Despite these challenges, I am very proud of the end product and think it is the best I could produce within the restraints of this assignment.