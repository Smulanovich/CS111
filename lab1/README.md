## UID: 705913985

# Pipe Up

## One Sentence Description

Pipe Up is a C program that simulates the behavior of the shell pipe (|) operator, allowing users to execute multiple programs in a pipeline, with proper input and output redirection.

## Building

To build the program, use the following command since Makefile is being used:

    make


## Running

./pipe_up ls cat wc

This command executes the ls command (lists the files in the current directory), pipes its output to cat, and then pipes the result to wc. The output should be the same as running ls | cat | wc in a shell.


## Cleaning up

To clean up the following command can be used since Makefile is being used:

    make clean

