# CPUSimulator
Simulate Process Scheduling and Memory Management of a CPU

It takes process of different sizes; 
loads them into memory when required, using one of three different algorithms 
and when needed, swaps processes out to create a suffciently large hole for a new process to come into memory. 
It also takes care of scheduling processes currently in memory using a Round Robin algorithm with quantum q.

For bringing a process from disk into memory, we use one of three different algorithms: first fit, best
fit and worst fit. Assume that memory is partitioned into contiguous segments, where each segment is
either occupied by a process or is a hole (a contiguous area of free memory).

The free list is a list of all the holes. Holes in the free list are kept in descending order of memory
address. Adjacent holes in the free list should be merged into a single hole.
The algorithms to be used for placing a process in memory are:

1. First fit: First fit starts searching the free list from the beginning (highest address), and uses the
first hole large enough to satisfy the request. If the hole is larger than necessary, it is split, with
the process occupying the higher address range portion of the hole and the remainder being put
on the free list.

2. Best fit: Chooses the smallest hole from the free list that will satisfy the request. If multiple
holes meet this criterion, choose the highest address one in the free list. If the hole is larger than
necessary, it is split, with the process occupying the higher address range portion of the hole and
the remainder being put on the free list.

3. Worst fit: Chooses the largest hole from the free list that will satisfy the request. If multiple
holes meet this criterion, choose the highest address one in the free list. If the hole is larger than
necessary, it is split, with the process occupying the higher address range portion of the hole and
the remainder being put on the free list.

Example Command Lines used to run the program:
./swap -f input.txt -a first -m 1000 -q 7 > output.txt
./swap -f testFirst1 -a first -m 1000 -q 7
./swap -f testBest1 -a best -m 1000 -q 7
./swap -f testWorst1 -a worst -m 1000 -q 7
