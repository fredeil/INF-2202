### _INF-2202 (Fall 2018)_
# Assignment #1: CONCURRENT B+TREES

_Date given: 24.08.2018_

_Date due: 06.09.2018_

Your task is to transform the B+ tree program in the code given as `precode/bpt.c` into a concurrent B+ tree.

You will find lecture notes related to B+ tree in the `btrees-notes` directory and papers related to concurrent B+ tree in `concurrent-btrees-papers`.

## About the precode (`bpt.c`)

This file contains the single-threaded and complete B+tree implementation in C language. What you need to do for this assignment is to implement concurrency control that will enable concurrent search, insert, and delete operations within this B+tree implementation.

A multi-threaded benchmark and a correctness test have been provided in the precode. If your concurrency control implementation is correct, the benchmark and correctness test should run without any errors when using multiple threads (e.g., no segmentation faults, all inserted keys are searchable). Note that the unmodified program benchmark and correctness test will always succeed if only using a single thread.

### Compiling and running

You can use any modern C (C99 and up) compiler to compile the code. Run the compiled program with `-h` to display help and the list of accepted running parameters.

```term
$ gcc -g bpt.c -o bpt -lpthread -lm
$ ./bpt -h

Concurrent BTree
===============

Use -h switch for help.

Accepted parameters
-r <NUM>    : Range size
-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates
-i <NUM>    : Initial tree size (inital pre-filled element count)
-t <0 / 1>  : Test mode (for correctness). 0: NO / 1: YES.
-n <NUM>    : Number of threads
-s <NUM>    : Random seed. 0 = using time as seed
-h          : This help

Benchmark output format:
"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)"
```

Use the `-t 1` switch to run the sequential and multi-threaded correctness test. Please note that to use more than one thread, you will have to add the `-n <#threads>` parameter.

```term
$ ./bpt -t 1

Concurrent BTree
===============

Use -h switch for help.

Parameters:
- Range size r:		 5000000
- Update rate u:	 10%
- Number of threads n:	 1
- Initial tree size i:	 1023
- Random seed s:	 0
- Test mode:		 1
Node size: 2708 bytes
Now doing correctness test
Sequential test
Inserting 5000000 (Random) elements...
insert time : 8618317 usec
search time : 4522180 usec
PASSED!


Parallel test
id:0, s:0, r:5000000, e:5000000
PASSED!
```
> NOTE:

> If you encounter a "Segmentation fault" error early in the Sequential test, then most likely you need to increase your stack limit:
> `$ ulimit -s unlimited`

## Requirements and limitations

1. Modify the B+ tree Search, Insert, and Delete operations in the code to support concurrency. You may use any known techniques or invent one yourself.
2. You are allowed to use all POSIX Thread API (pthreads.h) functions available (i.e., `pthread_spin_lock`, etc.) for concurrency. Please contact your TA first if you plan to implement the concurrency using other methods
3. You are not allowed to change the benchmark and test functions that are supplied in the code
4. The resulting concurrent B+ trees must be run without errors for any number of threads
5. Please also prepare a report on your method for achieving concurrency on B+tree and also the parallel performance analysis (e.g., speedup, efficiency) after conducting benchmarks using different combination of update ratios (`-u`) and thread numbers (`-n`)

> NOTE:

> If test mode is not used, the benchmark time performance can be found in the last line of program output. It is located after the last comma (in msec). For example:

> `0: 5000000, 5.00, 5.00, 2,  250267, 250276, 4499457, 5858, 5894, 103604, 1359`

> indicates the benchmark took 1359 miliseconds to complete

## Grading
A simple PASS or FAIL grade will be given based on your submitted code and report. Make sure to adhere to the aforementioned requirements and limitations.

## Disclaimer
Please do not publicize or share your solution anywhere without our permission.

Since this is an individual assignment, please refrain from copying any code from other students. However, both group discussions and brainstorming for ideas are strongly encouraged.
