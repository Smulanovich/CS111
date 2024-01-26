#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    // Check if there are command line arguments
    if (argc < 2) {
        perror("Error: No programs specified");
        exit(EINVAL);  // Exit with errno EINVAL for no programs specified
    }

    // Loop through each program in the command line arguments
    for (int i = 1; i < argc; i++) {
        // Fork a new process
        pid_t pid = fork();

        // Check for fork error
        if (pid == -1) {
            perror("Fork failed");
            exit(errno);  // Exit with the proper errno for fork failure
        }

if (pid == 0) {
    // Child process

    // If it's the first program, simply execute it
    if (i == 1) {
        if (execlp(argv[i], argv[i], (char *)NULL) == -1) {
            perror("Exec failed");
            exit(errno);
        }
    } else {
        int pipefd[2];

        // Create a pipe
        if (pipe(pipefd) == -1) {
            perror("Pipe creation failed");
            exit(errno);
        }

        // Fork a new process for the current program
        pid_t curr_pid = fork();

        if (curr_pid == -1) {
            perror("Fork failed");
            exit(errno);
        }

        if (curr_pid == 0) {
            // Child process of the current program

            // Close the write end of the pipe
            close(pipefd[1]);

            // Redirect standard input to the pipe
            dup2(pipefd[0], STDIN_FILENO);

            // Execute the current program
            if (execlp(argv[i], argv[i], (char *)NULL) == -1) {
                perror("Exec failed");
                exit(errno);
            }
        } else {
            // Parent process (original child process of the first program)

            // Close the read end of the pipe
            close(pipefd[0]);

            // Redirect standard output to the pipe
            dup2(pipefd[1], STDOUT_FILENO);

            // Execute the first program
            if (execlp(argv[1], argv[1], (char *)NULL) == -1) {
                perror("Exec failed");
                exit(errno);
            }
        }
    }

    // This part is reached only if there was an error in the exec calls
    perror("Exec failed");
    exit(errno);
}

    }

    // Parent process: Wait for all child processes to complete
    for (int i = 1; i < argc; i++) {
        int status;
        pid_t child_pid = waitpid(-1, &status, 0);

        // Check for waitpid error
        if (child_pid == -1) {
            perror("Waitpid failed");
            exit(errno);  // Exit with the proper errno for waitpid failure
        }
    }

    return 0;
}
