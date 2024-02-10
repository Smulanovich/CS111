#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;

  TAILQ_ENTRY(process) pointers;

  /* Additional fields here */
  u32 remaining_burst_time; // Remaining burst time of the process
  u32 waiting_time;         // Total waiting time of the process
  u32 response_time;        // Response time of the process
  bool is_executing;        // Flag indicating if the process is currently executing
  /* End of "Additional fields here" */
};


TAILQ_HEAD(process_list, process);

u32 next_int(const char **data, const char *data_end)
{
  u32 current = 0;
  bool started = false;
  while (*data != data_end)
  {
    char c = **data;

    if (c < 0x30 || c > 0x39)
    {
      if (started)
      {
        return current;
      }
    }
    else
    {
      if (!started)
      {
        current = (c - 0x30);
        started = true;
      }
      else
      {
        current *= 10;
        current += (c - 0x30);
      }
    }

    ++(*data);
  }

  printf("Reached end of file while looking for another integer\n");
  exit(EINVAL);
}

u32 next_int_from_c_str(const char *data)
{
  char c;
  u32 i = 0;
  u32 current = 0;
  bool started = false;
  while ((c = data[i++]))
  {
    if (c < 0x30 || c > 0x39)
    {
      exit(EINVAL);
    }
    if (!started)
    {
      current = (c - 0x30);
      started = true;
    }
    else
    {
      current *= 10;
      current += (c - 0x30);
    }
  }
  return current;
}

void init_processes(const char *path,
                    struct process **process_data,
                    u32 *process_size)
{
  int fd = open(path, O_RDONLY);
  if (fd == -1)
  {
    int err = errno;
    perror("open");
    exit(err);
  }

  struct stat st;
  if (fstat(fd, &st) == -1)
  {
    int err = errno;
    perror("stat");
    exit(err);
  }

  u32 size = st.st_size;
  const char *data_start = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data_start == MAP_FAILED)
  {
    int err = errno;
    perror("mmap");
    exit(err);
  }

  const char *data_end = data_start + size;
  const char *data = data_start;

  *process_size = next_int(&data, data_end);

  *process_data = calloc(sizeof(struct process), *process_size);
  if (*process_data == NULL)
  {
    int err = errno;
    perror("calloc");
    exit(err);
  }

  for (u32 i = 0; i < *process_size; ++i)
  {
    (*process_data)[i].pid = next_int(&data, data_end);
    (*process_data)[i].arrival_time = next_int(&data, data_end);
    (*process_data)[i].burst_time = next_int(&data, data_end);
  }

  munmap((void *)data, size);
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    return EINVAL;
  }
  struct process *data;
  u32 size;
  init_processes(argv[1], &data, &size);

  u32 quantum_length = next_int_from_c_str(argv[2]);

  struct process_list list;
  TAILQ_INIT(&list);

  u32 total_waiting_time = 0;
  u32 total_response_time = 0;

/* Your code here */
u32 current_time = 0;
u32 completed_processes = 0;

// Iterate over the processes until all processes are completed
while (completed_processes < size) {
    for (u32 i = 0; i < size; ++i) {
        // Check if the process has arrived and has remaining burst time
        if (data[i].arrival_time <= current_time && data[i].remaining_burst_time > 0) {
            // Execute the process for the quantum length or until it completes
            u32 execution_time = MIN(quantum_length, data[i].remaining_burst_time);
            data[i].remaining_burst_time -= execution_time;
            
            // Update waiting time and response time if the process is just starting execution
            if (!data[i].is_executing) {
                data[i].is_executing = true;
                data[i].response_time = current_time - data[i].arrival_time;
            }
            
            // Update current time
            current_time += execution_time;
            
            // Check if the process has completed
            if (data[i].remaining_burst_time == 0) {
                completed_processes++;
                data[i].waiting_time = current_time - data[i].arrival_time - data[i].burst_time;
                total_waiting_time += data[i].waiting_time;
                total_response_time += data[i].response_time;
            } else {
                // Move the process to the end of the list to simulate round-robin scheduling
                TAILQ_REMOVE(&list, &data[i], pointers);
                TAILQ_INSERT_TAIL(&list, &data[i], pointers);
            }
        }
    }
}

/* End of "Your code here" */


  printf("Average waiting time: %.2f\n", (float)total_waiting_time / (float)size);
  printf("Average response time: %.2f\n", (float)total_response_time / (float)size);

  free(data);
  return 0;
}
