# INF-2202, Concurrent and data-intensive programming 17. aug. 2018

## Question 1

### 1a - What does the above script do?

The script extracts the Started, Region and Duration columns from the @searchlog variable where Region equals "en-gb".

### 1b - Explain what the following key concepts (rowset variable, extract keyword, select keyword)

1. Each query expression that produces a rowset can be assigned to a variable, see @searchlog
2. The EXTRACT keyword reads data from a file and defines the schema on read
3. The SELECT keyword is used to transform rowsets

### 1c

The script can be run locally by using Azure Data Lake extensions for Visual Studio, simply choose the "Run locally" option in the IDE.
The script can also be deployed to Azure in the same IDE or uploading it manually in the portal.

### 1d

This depends on the size of the data, complexity of the scripts and the available computing power.

## Question 2

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

