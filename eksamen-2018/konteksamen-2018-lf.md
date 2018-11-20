# INF-2202, Concurrent and data-intensive programming 17. aug. 2018

## Question 1: U-SQL and Azure Data Lake

Below is a simple U-SQL script:

```SQL
@searchlog =
        EXTRACT UserId int,
                Started DateTime,
                Region string,
                Query string,
                Duration int?,
                Urls string
        FROM "SearchLog.tsv"
        USING Extractors.Tsv();

@rs1 =
    SELECT Started, Region, Duration
    FROM @searchlog
    WHERE Region == "en-gb";

OUTPUT @rs1 
        TO "SearchLog-transform-rowsets.csv"
        USING Outputter.Csv();
```

### 1a - What does the above script do?

The script extracts the Started, Region and Duration columns from the @searchlog variable where Region equals "en-gb" and puts it into a CSV file.

### 1b - Explain what the following key concepts (rowset variable, extract keyword, select keyword)

1. Each query expression that produces a rowset can be assigned to a variable, see @searchlog
2. The EXTRACT keyword reads data from a file and defines the schema on read
3. The SELECT keyword is used to transform rowsets

### 1c - Running U-SQL scripts

> When you develop a script like the one above, you want to be able to run and debug it locally and in the cloud.
> Explain briefly how you run and debug this script in Visual Studio and how you run the script on Azure?

The script can be run locally by using Azure Data Lake extensions for Visual Studio, simply choose the "Run locally" option in the IDE.
The script can also be deployed to Azure in the same IDE or uploading it manually in the portal.

### 1d - Cost estimation

> How would you estimate the cost for the script execution on Azure Data Lake? What is a realistic price (in UDS)
> for these type of cloud jobs? We don't want the exact price and calculations. but the principles and some price estimation based on these.

This depends on the size of the data, complexity of the scripts and the available computing power.

## Question 2 - Bloom filter

### 2a - What is a bloom filter?

A bloom filter is a probabilistic data structure for testing membership in sets, 
it can either tell you if a value is probably in the set or definitely not in the set.

The bloom filter is an array of _m_ bits, all initialized to 0. It also needs _k_ hash functions.

__Inserting:__

When adding an element, it is run through all the k hash functions. 
The output of each hash function denotes a position in the m-bit array. 
All these positions are then set to 1.

__Querying:__

To check if an element is in the set, run it through the hash functions, like when inserting. 
If any of the array positions returned by the hash functions contain a zero, the element is definitely not in the set. 
If all the bits are 1, then the element may be in the set (or they have all just been set to 1 by accident).

### 2b - Thread safe Bloom filter

> Implement, in pseudo code, a Bloom filter. The bloom filter should be thread safe and it should also support multiple readers.

> Bloom filter add
> k <- Hash functions 
> for i to k:
>       Bloomfilter[h_i(value)] = 1
> end 

> Bloom filter contains
> k <- Hash functions
> for i to k: 
>       if Bloomfilter[h_i(value)] is 0
>               return false
> return true

A bloom filter is threadsafe. If two threads want to alter the state of the bloom filter at the same time, the result would not create a race condition. An index can only be set to 1 by a thread and a value is never deleted and therfore you don't need to use any synchronization primitives in a bloom filter to make it thread safe. 
### 2c - Scrubbing logs

> How would you use the Bloom filter to transform the initial schemas of the input file to an encrypted equivalent suitable for _cold storage_?

To make the initial schema suitable for cold storage we would have encrypted all values in the schema. To support GDPR if a user want to be deleted from all log lines containing him/she we would have asked the bloom filter if the given user is present in a log line. By doing this we would get a definitive no from the bloom filter if the user is not present and if it says yes it may be a false positive. However, we would save a lot of cost by not decrypting every log but only the logs where the bloom filter says that a user may be present. 

## Question 3 - Reactive programming (RP)

### 3a - Reactive/Thread based programming

> When would you use reactive programming instead of thread base programming?

When working with data streams or events it is better to use reactive libraries. Instead of creating a thread that blocks when waiting for a value, you simply subscribe to an observable that would push the value to you when it's ready. The working thread would then wake up your code with the callback function and you can react on the value. 

### 3b - Transforming streams

> How would you use RxPY to transform the stream of GEO events into _the time to destination_ (-25.385, 16.423)
> based on average speed? _(Hint: this is related to mandatory assignment 2)_

Not relevant

### 3c - Implement model

> How would you implememnt the _TopN_ people model (ordered collection of people) that are most relevant to a given rider?
> _(Hint: this is related to what you did in mandatory assignment 2)

### 3d - Unit testing

> How would you unit test RxPY code? How, and why do you unit test in virtual time?

You should test it by using virtual-time scheduling, so that running the unit-tests doesn't take a long time.
This is done by creating a test scheduler and specifying the time of each event the observable sequence emits, you can
then subscribe to these events (after filtering for example) and subscribe to the events on the specified time.

## Question 4 - Performance

### 4a - Web performance

> What are relevant performance metrics for a web application?


### 4b - How do you measure 3 of these?
