# INF-2202, Concurrent and data-intensive programming 21.11.2018

> A new group of students are studying for an exam. As usual, the students can only study while eating pizza.
> If a student finds that there is no more pizza, the students goes to sleep until more pizza arrives. The
> first student to discover that the group is out of pizza orders another pizza before going to sleep.

## Question 1 - Even more pizza eating

### 1a - Synchronize threads

> Write code to synchronize the student threads and the pizza delivery thread using mutexes and condition variables.
> Your solution should avoid deadlock and phone the pizza place exactly once each time a pizza is exhausted.
> No piece of pizza may be consumed by more than one student.

### 3c - Round-robin scheduling

> Round-robin schedulers maintain a ready list or run queue of all runnable threads (or processes),
> with each thread listed at most once in the list. What can happen if a  thread appears twice in the list?
> Explain how this could cause programs to break on a uniprocessor.

If a thread appears more than once in the ready list it will of course get more than its fair share of resources, but the bigger problem isthat of synchronization. Imagine thread A as the sneaky bastard:

```
A-B-C-A-D-E-F-A-B-C-A-D- and so on
```

Imagine if thread A grabs a lock to modify some shared memory. B and C also wish to modify this, so they try to grab the lock but areblocked. A gets another turn and releases the lock, but before B and C get a chance to wake up A gets another turn! A doesn't knowhe's been scheduled before his time, so he grabs the lock again. This leads to the starvation of threads B and C.
