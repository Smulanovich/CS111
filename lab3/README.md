# Hash Hash Hash
This lab focuses on enhancing a serial hash table for concurrent use by implementing mutex locks for thread synchronization across two versions: hash-table-v1 and hash-table-v2. Each version employs a distinct locking strategy to ensure thread safety while preserving the hash table's separate chaining collision resolution mechanism. The exercise involves evaluating the correctness and performance impacts of these strategies against the base implementation.

## Building
```shell
cd path/to/lab3
make

```

## Running
```shell
 ./hash-table-tester -t [number_of_threads] -s [number_of_entries]

Sample Output 

    Generation: 64,515 usec
    Hash table base: 1,973,761 usec
    - 0 missing
    Hash table v1: 2,345,967 usec
    - 0 missing
    Hash table v2: 411,362 usec
    - 0 missing

```

## First Implementation
In the `hash_table_v1_add_entry` function, I added mutex-based locking to manage concurrent access to the hash table. This modification ensures that when multiple threads attempt to add or update entries simultaneously, the hash table's integrity is maintained by serializing these operations. By locking the entire hash table upon each write operation, the implementation effectively prevents data races, ensuring that each modification is fully completed before another begins. This approach, while simple and effective in preserving data consistency in a multithreaded context, introduces a trade-off between thread safety and performance due to the overhead of acquiring and releasing the mutex.

### Performance
```shell
 This is the test I used: ./hash-table-tester -t 8 -s 70000
     This were the results (used the UCLA Linux VM):

        Test Run 1:
        Generation: 52,132 usec
        Hash table base: 1,709,388 usec
        - 0 missing
        Hash table v1: 2,232,871 usec
        - 0 missing

        Test Run 2:
        Generation: 50,285 usec
        Hash table base: 1,986,991 usec
        - 0 missing
        Hash table v1: 1,925,341 usec
        - 0 missing

        Test Run 3:
        Generation: 68,311 usec
        Generation: 50,975 usec
        Hash table base: 1,845,002 usec
        - 0 missing
        Hash table v1: 2,092,998 usec
        - 0 missing

        Test Run 4:0
        Generation: 52,819 usec
        Hash table base: 1,744,937 usec
        - 0 missing
        Hash table v1: 1,999,386 usec
        - 0 missing


    As it can be seen, the results where overall pretty similar. v1 takes more time than base due to the overhead introduced by lock contention. 
    This is further explained below.

```
Version 1 is slower than the base version. This performance difference can be attributed to the added overhead of locking and unlocking the mutex for each add operation. While these locks are crucial for ensuring thread safety in concurrent environments, they introduce a delay as each thread must wait its turn to access the hash table for writing. The additional time spent managing lock contention and the inherent delay in thread synchronization contribute to the overall increase in execution time compared to the lock-free base version.

## Second Implementation
## Second Implementation

The `hash_table_v2_add_entry` function in the second implementation of the hash table introduces a significant enhancement by assigning a unique mutex to each entry within the hash table. This approach minimizes lock contention by allowing more granular locking compared to the first version, which employed a single mutex for the entire table. With this strategy, only the specific entry being accessed or modified is locked, rather than the entire table. This ensures that multiple threads can concurrently access or modify different entries without waiting for each other, thereby increasing concurrency and potentially improving performance. Additionally, by locking only the relevant entry, the implementation prevents race conditions and ensures data integrity, as only one thread can modify an entry at a time, thereby avoiding conflicts and inconsistencies in the hash table. 

### Performance
```shell
 I used the same test script: ./hash-table-tester -t 8 -s 70000
     This where the results (used the UCLA Linux VM):

        Test Run 1:
        Generation: 52,132 usec
        Hash table base: 1,709,388 usec
        - 0 missing
        Hash table v2: 1,193,345 usec
        - 0 missing

        Test Run 2:
        Generation: 50,285 usec
        Hash table base: 1,986,991 usec
        - 0 missing
        Hash table v2: 1,099,759 usec
        - 0 missing

        Test Run 3:
        Generation: 68,311 usec
        Generation: 50,975 usec
        Hash table base: 1,845,002 usec
        - 0 missing
        Hash table v2: 1,115,550 usec
        - 0 missing

        Test Run 4:0
        Generation: 52,819 usec
        Hash table base: 1,744,937 usec
        - 0 missing
        Hash table v2: 1,194,896 usec
        - 0 missing
```

In tests on a UCLA virtual machine with 2 cores, the `hash_table_v2` implementation showed a 30-35% speedup compared to the base version. This improvement is due to optimized concurrency mechanisms and reduced lock contention, aimed at enhancing multi-threaded performance. The efficiency gain in `hash_table_v2` highlights the success of these adjustments in utilizing the hardware, leading to improved performance and scalability.

## Cleaning up
```shell
cd path/to/lab3
make clean
```