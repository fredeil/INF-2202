# INF-2202, Concurrent and data-intensive programming 23.11.2017

## Question 1: Thread programming

_Not relevant in the newer course_ 

### 1a - Event class

__1.a.1__

> Implement an Event class using a single semaphore with no additional synchronization
> (Assume that a semaphore class with semaphore methods exists)

```python
__author__ = "Mads Johansen"
class Event:
    def __init__(self):
        self.semaphore = Semaphore()
        self.signaled = False

    def wait (self):
        if self.signaled:
            return
        self.semaphore.down()

    def signal(self):
        self.signaled = True
        self.semaphore.up()
```

__1.a.2__

> Implement an Event class using a mutex and condition variable.
> (Assume that lock and ConditionVariable classes with appropriate methods exist)

```python
__author__ = "Mads Johansen"
class Event:
    def __init__(self):
        self.lock = Lock()
        self.condition = ConditionVariable()
        self.signaled = False

    def wait (self):
        if self.signaled:
            return

    def signal(self):
        self.signaled = True
```

__1.a.3__

> Show how to use Event to implement the synchronization for a join() primitive for
processes or threads. Your solution should show how the event could be used by the code for thread/process exit() as well as by join().

```python
__author__ = "Mads Johansen"
def worker():
    do_work()
    thread_complete()

def join():
    incomplete()
```

### 1b - Pizza eating problem

> Write code to synchronize the student thread and the pizza delivery thread. Your solution should avoid deadlock
> and phone the pizza place exactly once each time a pizza is exhausted (eaten up).
> No piece of pizza may be consumed by more than one student.

```python
__author__ = "Mads Johansen"
pizza_slices = 8
pizza_lock = Lock()
phone_lock = Lock()
ordered = False

def study():
    # Sleeping is basically subconsicous studying
    sleep(random.randint(1000))

def student():
    with pizza_lock:
        while not pizza_slices:
            with phone_lock:
                if not ordered:
                    order_pizza()
                    ordered = True
        pizza_slices -= 1
    study()

def order_pizza():
    sleep(random.randint(250))
    deliver_pizza()

def deliver_pizza():
    pizza_slices = 8
```

### 3c - Round-robin scheduling

> Round-robin schedulers maintain a ready list or run queue of all runnable threads (or processes),
> with each thread listed at most once in the list. What can happen if a  thread appears twice in the list?
> Explain how this could cause programs to break on a uniprocessor.

If a thread appears more than once in the ready list it will of course get more than its fair share of resources, but the bigger problem isthat of synchronization. Imagine thread A as the sneaky bastard:

```
A-B-C-A-D-E-F-A-B-C-A-D- and so on
```

Imagine if thread A grabs a lock to modify some shared memory. B and C also wish to modify this, so they try to grab the lock but areblocked. A gets another turn and releases the lock, but before B and C get a chance to wake up A gets another turn! A doesn't knowhe's been scheduled before his time, so he grabs the lock again. This leads to the starvation of threads B and C.

## Question 2: Reactive programming

### 2a - What is reactive programming?

Reactive programming is programming with push based event streams and it is
about composition and having event streams as first-class objects.

### 2b - Iterables and Observables

> What is the relationship, similarities and differences between iterables and observables?

Iterables are collections that we can iterate through to get their values (pull based operations).
Some examples of iterables are lists, tuples, sets, and dictionaries. We can imagine pulling values from a bucket until it is empty.

Observables emit a stream of events/values as they occur in time.
Some things that we can consider push based streams include mouse and keyboard inputs, web requests and something else.
Instead of us pulling values from an Observable, it will push the values to us as they appear.

They are similar in that they both provide values in a sequential fashion that we can use in further computations.
They both have several functional methods in common such as map and filter.
The key difference is that they are distributed space versus time, we can transform an iterable into an observable and vice versa.

### 2c - What is the output of the following Python program

```python
from rx import Observable
Observable.range(1, 10).filter(lambda x: x < 5).map(lambda x: x*10).subscribe(print)
```

This creates an Observable that emits a stream containing the integer values 1 through 9. The values are then passed through a filter that excludes values greater than 5. The values that were filtered are then mapped through a function that multiplies them by ten. The events from this stream are passed to the print function through subscribe.

Sequence printed to console: `10 -> 20 -> 30 -> 40`

### 2d -  Implementing slice method for observables

_Not relevant in the newer course_

```python
___author___ = "Mads Johansen"

class Observable:
    def __getitem__(self, obj):
        source = self
        start = obj.start or 0
        stop = obj.stop or 0
        step = obj.step or 1

        if step < 0:
            raise ArgumentError("Cannot slice an Observable with negative step value")

        sliced = source.skip(start) if start >= 0 else source.take_last(abs(start))
        sliced = sliced.take(stop - start) if stop >= 0 else sliced.skip_last(abs(stop))
        sliced = sliced.skip(step - 1)

        return sliced

    def __getitem__(self, obj):
        easy_solution = self.toList().__getitem__(obj)
        return Observable._from(easy_solution)

```

## Question 3: Programming for cloud

### 3a - Logging

> Describe what is meant by cold, warm and hot path in the context of logging/monitoring of cloud scale software systems,
> what characterizes the different kinds of data that we typically find in these containers? You should cover traits like 
> retention, latency before data becomes available, data types, etc.

The hot, warm and cold paths are different "levels" of a logging/monitoring system. They all contain data from the same sources, butthe kinds of information they get are different.

The __hot path__ is used to get immediate feedback about the health of the system. Information shows up in the hot path within a minute of an event occurring, but there usually isn't enough information for a deep dive to find the problem. The information is usually in the form of aggregated values like response times (average and percentiles) and number of errors. This path gives more of an bird's eye view of the system being monitored so we can tell its general state at a glance.

Information in the __hot path__ is not stored for long periodsof time.The warm path is mostly used for debugging recent errors. The information in the warm path includes things like session logs for usersand stacktraces and is available within 5 to 15 minutes after the events have occurred. This information can be quite verbose and is only useful for a limited amount of time, so it is usually not stored for more than a week.

The __cold path__ is used for long term storage. This information is usually stored in aggregated form and is used to get a long term overview of the systems health. Information shows up after a day or more to account for differences in time zones and transient failures that can occur in data centers. This information is often stored for years.

### 3b - Guids binary/string representation

> Guids are often used for identifying sessions, users, or anything else that needs to be globally unique/distinguishable.
> In binary form a Guid is 16  bytes, while in string form it has the following format: "1fa0f09d-1bab-4d3d-b60f-e314c3252ab9"

1. In .NET strings are 2 bytes per character, what is the size difference between string/binary representation of Guids?

    A GUID in a string representation (with dashes) is 36 characters (72 bytes), so the difference is 56 bytes between string and binary representation.

2. List pros and cons of using binary and string representations of Guids in logs.

    String representation:
    - Con: Takes up much more space in memor
    - Con: Parsing overhead when creating in-memory objects
    - Pro: Human readable format
  
    Binary representation:
    - Con: Not human readable
    - Pro: Easily converted to in-memory objects
    - Pro: Takes up less space in memory

3. Describe why even small increases in performance/decreases in storage/memory consumption can have large consequences in cloud computing?

    In cloud computing small increases in performance can have massive effects, if we were to store large amount of GUIDs for example
    we would be spending alot of storage and processing power, hence the price would increase.

    If we were to store GUIDs in binary format we would use much less storage and processing power and most certainly save money.

### 3c - Hard/Soft/Hybrid Schemas in logs

> When logging it is commong to adhere to some sort of schema. At the ends of the spectrum are "hard" and "soft" schemas.

1. Describe what we mean with a (fully) soft schema. What are the characteristics of these?
   Answer

2. Describe what we mean with a (fully) hard schema. What are the characteristics of these?
   Answer

3. Describe what is meant with a hybrid schema. Name tradeoffs regarding extensibility, verbosity and performance?
   Answer

### 3d - Scrubbing of logs

> Scrubbing is a means of anonymizing or protecting sensitive parts of data. Discuss how
> scrubbers can be used to solve some of the issues related to 'compliance' and logs. Consider aspects
> such as hashing and encryption, and when these should be used. What are possible means of ensuring data is scrubbed, and
> what are pros/cons of these approaches? In cloud computing where do you think the scrubbing should happen, and why?