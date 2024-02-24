# You Spin Me Round Robin

This document outlines the process for compiling, running, and cleaning up the round robin scheduler simulation. The simulation demonstrates how round robin scheduling operates, managing process execution in a fair, cyclic order based on a predefined quantum length. The scheduler ensures that each process receives an equal share of CPU time, preventing any single process from monopolizing the system resources.

## Building


```shell
# Navigate to the lab-02 directory where your Makefile is located
    cd path/to/lab-02

# Use the Makefile to compile the project
    make

## Running

cmd for running TODO
```shell
    # Syntax: 
        ./rr <path to processes file> <quantum length>
    # E.g.:
        ./rr processes.txt 3
```

results TODO
```shell
    Average waiting time: x.f
    Average response time: y.f

```

## Cleaning up

```shell
    # Use the Makefile to clean up the project directory
    make clean
```
