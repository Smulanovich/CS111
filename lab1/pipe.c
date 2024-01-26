#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error: No programs specified.\n");
        exit(EINVAL);
    }

    int pipefd[2];

    for (int i = 1; i < argc; ++i) {
        if (i < argc - 1) {
            // Create a pipe
            if (pipe(pipefd) == -1) {
                perror("Error in pipe");
                exit(errno);
            }
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Error in fork");
            exit(errno);
        } else if (pid == 0) {  // Child process
            if (i > 1) {
                // Check if the read end of the pipe is a valid file descriptor
                if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                    perror("Error in dup2");
                    exit(errno);
                }

                // Close the read end of the pipe
                close(pipefd[0]);
            }

            if (i < argc - 1) {
                // Check if the write end of the pipe is a valid file descriptor
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                    perror("Error in dup2");
                    exit(errno);
                }

                // Close the write end of the pipe
                close(pipefd[1]);
            }

            // Redirect standard error to the parent's standard error
            if (dup2(STDERR_FILENO, STDERR_FILENO) == -1) {
                perror("Error in dup2");
                exit(errno);
            }

            // Execute program using execlp
            execlp(argv[i], argv[i], (char *)NULL);

            // If execlp fails
            perror("Error in execlp");
            exit(errno);
        } else {
            // Parent process
            // Close both ends of the pipe in the parent
            if (i < argc - 1) {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            // Wait for the child process to terminate
            int status;
            waitpid(pid, &status, 0);

            // Check if child process exited successfully
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Error: Process %s failed with exit status %d\n", argv[i], WEXITSTATUS(status));
                exit(WEXITSTATUS(status));
            }

            // Exit after the last command is executed
            if (i == argc - 1) {
                exit(0);
            }
        }
    }

    return 0;
}
