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
#include <limits.h> // For UINT_MAX

typedef uint32_t u32;
typedef int32_t i32;

struct process
{
  u32 pid;
  u32 arrival_time;
  u32 burst_time;
  TAILQ_ENTRY(process) pointers;

/* Additional fields here */
u32 start_execution_time;
u32 waiting_time;
u32 response_time;
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
    fprintf(stderr, "Usage: %s <path to input file> <quantum length>\n", argv[0]);
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
  bool is_process_running = false;
  struct process *current_process = NULL;
  u32 time_slice = 0;

  u32 completed_processes_count = 0;

  // Main simulation loop
  do {
    // Enqueue processes that arrive at the current time
    for (u32 i = 0; i < size; ++i) {
      if (data[i].arrival_time == current_time) {
        data[i].start_execution_time = UINT_MAX; // Mark as not started
        TAILQ_INSERT_TAIL(&list, &data[i], pointers);
      }
    }

    // Time slice or process completion handling
    if (is_process_running && (time_slice == quantum_length || current_process->burst_time == 0)) {
      if (current_process->burst_time > 0) {
        TAILQ_INSERT_TAIL(&list, current_process, pointers); // Only re-queue if not finished
      } else {
        completed_processes_count++; // Mark as completed
      }
      is_process_running = false;
      current_process = NULL;
    }

    // Process selection for execution
    if (!is_process_running && !TAILQ_EMPTY(&list)) {
      current_process = TAILQ_FIRST(&list);
      TAILQ_REMOVE(&list, current_process, pointers);
      is_process_running = true;
      time_slice = 0; // Reset time slice for the new process
      
      // Calculate response time if not already done
      if (current_process->start_execution_time == UINT_MAX) {
        current_process->start_execution_time = current_time;
        current_process->response_time = current_process->start_execution_time - current_process->arrival_time;
        total_response_time += current_process->response_time;
      }
    }

    // Process execution
    if (is_process_running) {
      current_process->burst_time--;
      time_slice++;
    }

    // Increment waiting time for all processes in the queue
    struct process *p;
    TAILQ_FOREACH(p, &list, pointers) {
      if (p != current_process) { // Make sure not to increment waiting time for the currently running process
        p->waiting_time++;
        total_waiting_time++;
      }
    }

    current_time++; // Move to the next time unit
  } while (completed_processes_count < size);

  printf("Waiting time: %.2f\n", (float)total_waiting_time / size);
  printf("Response time: %.2f\n", (float)total_response_time / size);
  /* End of "Your code here" */

  free(data);
  return 0;
}

