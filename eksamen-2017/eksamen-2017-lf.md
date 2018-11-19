# INF-2202, Concurrent and data-intensive programming 23.11.2017

## Question 1: Event class

_Not relevant in the newer course_ 

## Question 2: Reactive programming

### 2a - What is reactive programming?

### 2b - What is the relantionship ,similarities and difference between iterables and observables?

### 2c - What is the output of the following Python program

```python
from rx import Observable
Observable.range(1, 10).filter(lambda x: x < 5).map(lambda x: x*10).subscribe(print)
```

### 2d -  Implementing slice method for observables

_Not relevant in the newer course_

## Question 3: Programming for cloud

### 3a - Logging

> Describe what is meant by cold, warm and hot path in the context of logging/monitoring of cloud scale software systems,
> what characterizes the different kinds of data that we typically find in these containers? You should cover traits like 
> retention, latency before data becomes available, data types, etc.

### 3b - Guids binary/string representation

> Guids are often used for identifying sessions, users, or anything else that needs to be globally unique/distinguishable.
> In binary form a Guid is 16  bytes, while in string form it has the following format: "1fa0f09d-1bab-4d3d-b60f-e314c3252ab9"

1. In .NET strings are 2 bytes per character, what is the size difference between string/binary representation of Guids?
    Answer

2. List pros and cons of using binary and string representations of Guids in logs.
    Answer

3. Describe why even small increases in performance/decreases in storage/memory consumption can have large consequences in cloud computing?
    Answer

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