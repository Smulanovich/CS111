#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s program1 [program2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Number of programs
    int num_programs = argc - 1;

    // Create an array to store file descriptors for pipes
    int pipe_fds[num_programs - 1][2];

    // Fork processes and set up pipes
    for (int i = 0; i < num_programs - 1; ++i) {
        if (pipe(pipe_fds[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (child_pid == 0) {
            // Child process

            // Close write end of the previous pipe
            close(pipe_fds[i][1]);

            // Redirect standard input to the read end of the pipe
            if (dup2(pipe_fds[i][0], STDIN_FILENO) == -1) {
                perror("dup2");
                exit(EXIT_FAILURE);
            }

            // Close the read end of the pipe
            close(pipe_fds[i][0]);

            // Execute the program
            execlp(argv[i + 1], argv[i + 1], (char *)NULL);

            // If execlp fails
            perror("execlp");
            exit(EXIT_FAILURE);
        } else {
            // Parent process

            // Close the read end of the previous pipe
            close(pipe_fds[i][0]);
        }
    }

    // Fork the last process
    pid_t last_child_pid = fork();

    if (last_child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (last_child_pid == 0) {
        // Child process

        // Redirect standard output to the write end of the last pipe
        if (dup2(pipe_fds[num_programs - 2][1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Close the write end of the last pipe
        close(pipe_fds[num_programs - 2][1]);

        // Execute the last program
        execlp(argv[num_programs], argv[num_programs], (char *)NULL);

        // If execlp fails
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process

        // Close the write end of the last pipe
        close(pipe_fds[num_programs - 2][1]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < num_programs; ++i) {
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
