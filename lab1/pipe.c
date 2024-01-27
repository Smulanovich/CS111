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

    // File descriptors for pipe
    int pipe_fd[2];

    // Initial input for the first program
    int input_fd = STDIN_FILENO;

    // Iterate through each program
    for (int i = 1; i < argc; i++) {
        if (i != argc - 1) {
            // If not the last program, create a pipe
            if (pipe(pipe_fd) == -1) {
                perror("Error creating pipe");
                exit(errno);
            }
        }

        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Error forking");
            exit(errno);
        }

        if (child_pid == 0) {
            // Child process
            if (i != 1) {
                // If not the first program, redirect stdin to the saved input_fd
                dup2(input_fd, STDIN_FILENO);
            }

            if (i != argc - 1) {
                // If not the last program, redirect stdout to the write end of the pipe
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]); // Close unused write end
            }

            // Close all file descriptors associated with the pipe
            close(pipe_fd[0]);
            
            // Execute the program
            execlp(argv[i], argv[i], NULL);
            perror("Error executing program");
            exit(errno);
        } else {
            // Parent process
            // Close the write end of the pipe for the next iteration
            if (i != argc - 1) {
                close(pipe_fd[1]);
            }

            // Save the read end of the pipe for the next iteration
            input_fd = pipe_fd[0];

            // Wait for child to finish
            waitpid(child_pid, NULL, 0);
        }
    }

    return 0;
}
