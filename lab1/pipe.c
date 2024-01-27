#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        perror("Not enough arguments");
        exit(EINVAL);
    }

    // Create an array to store file descriptors for pipes
    int pipe_fds[2];

    // Loop through the command line arguments (executable names)
    for (int i = 1; i < argc; i++) {
        // Check if it's not the last executable in the list
        if (i < argc - 1) {
            // Create a pipe for the current process
            if (pipe(pipe_fds) == -1) {
                perror("Pipe creation failed");
                exit(errno);
            }
        }

        // Create a child process
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Fork failed");
            exit(errno);
        }

        if (child_pid == 0) {
            // Child process

            // If it's not the first executable, set standard input to the read end of the pipe
            if (i > 1) {
                if (dup2(pipe_fds[0], STDIN_FILENO) == -1) {
                    perror("Dup2 failed");
                    exit(errno);
                }
                // Close the unused write end of the pipe
                close(pipe_fds[1]);
            }

            // If it's not the last executable, set standard output to the write end of the pipe
            if (i < argc - 1) {
                if (dup2(pipe_fds[1], STDOUT_FILENO) == -1) {
                    perror("Dup2 failed");
                    exit(errno);
                }
                // Close the unused read end of the pipe
                close(pipe_fds[0]);
            }

            // Execute the current program in the child process
            execlp(argv[i], argv[i], NULL);

            // If execlp fails, print an error message and exit
            perror("Exec failed");
            exit(errno);
        } else {
            // Parent process

            // If it's not the first executable, close the unused read end of the pipe
            if (i > 1) {
                close(pipe_fds[0]);
            }
            // If it's not the last executable, close the unused write end of the pipe
            if (i < argc - 1) {
                close(pipe_fds[1]);
            }

            // Wait for the child process to finish
            wait(NULL);
        }
    }

    return 0;
}
