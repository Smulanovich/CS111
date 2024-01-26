#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    // Check if there are programs as command-line arguments
    if (argc < 2)
    {
        perror("No programs specified");
        return EINVAL;
    }

    int num_programs = argc - 1;
    int pipes[num_programs - 1][2]; // rows = number of programs - 1, columns = 2 (read and write)

    // Create pipes
    for (int i = 0; i < num_programs - 1; ++i)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("Pipe creation failed");
            return errno;
        }
    }

    // Execute programs and set up pipes
    for (int i = 0; i < num_programs; ++i)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("Fork failed");
            return errno;
        }
        else if (pid == 0) // Child process
        {
            // Set up pipes for all processes except the first one
            if (i > 0)
            {
				// Redirect standard input of the child process to the read end of the previous pipe
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2 failed");
                    return errno;
                }
            }

            // Close unused ends of pipes
            for (int j = 0; j < num_programs - 1; ++j)
            {
                close(pipes[j][0]); // Close read end
                close(pipes[j][1]); // Close write end
            }

            // Execute the program
            execlp(argv[i + 1], argv[i + 1], NULL);
            perror("execlp failed");
            return errno;
        }
        else // Parent process
        {
            // Close the read end of the previous pipe (if not the first process)
            if (i > 0)
            {
                close(pipes[i - 1][0]);
            }
        }
    }

    // Close all remaining pipe ends in the parent
    for (int i = 0; i < num_programs - 1; ++i)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < num_programs; ++i)
    {
        wait(NULL);
    }

    return 0;
}
