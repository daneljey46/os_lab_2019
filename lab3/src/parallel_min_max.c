#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"
#include <signal.h>

pid_t *child_pids;
int pnum_global;

void kill_children(int sig) {
    for (int i = 0; i < pnum_global; i++) {
        kill(child_pids[i], SIGKILL);
    }
    printf("\nTimeout reached. All child processes killed.\n");
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed <= 0) {
              printf("Error: seed must be a positive number\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size <= 0) {
              printf("Error: array_size must be a positive number\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum <= 0) {
              printf("Error: pnum must be a positive number\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          case 4:
            timeout = atoi(optarg);
            if (timeout <= 0) {
               printf("Error: timeout must be a positive number\n");
              return 1;
            }
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  int pipefd[2];
  if (!with_files) {
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
  }

  pnum_global = pnum;
  child_pids = malloc(sizeof(pid_t) * pnum);

  if (timeout > 0) {
    signal(SIGALRM, kill_children);
    alarm(timeout);
  }

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid > 0) {
      child_pids[i] = child_pid;
      active_child_processes += 1;
    }
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        int start = i * (array_size / pnum);
        int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);

        struct MinMax part_min_max = GetMinMax(array, start, end);

        if (with_files) {
          char file_name[64];
          sprintf(file_name, "result_%d.txt", i);
          FILE *f = fopen(file_name, "w");
          fprintf(f, "%d %d", part_min_max.min, part_min_max.max);
          fclose(f);
        } else {
          write(pipefd[1], &part_min_max.min, sizeof(int));
          write(pipefd[1], &part_min_max.max, sizeof(int));
        }
        
        free(array);
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    int status;
    pid_t done_pid = waitpid(-1, &status, WNOHANG);

    if (done_pid > 0) {
        active_child_processes -= 1;
    } else if (done_pid == 0) {
        usleep(10000); 
    } else {
        if (active_child_processes > 0 && timeout > 0) {
            break; 
        }
        
        break;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      char file_name[64];
      sprintf(file_name, "result_%d.txt", i);
      FILE *f = fopen(file_name, "r");
      if (f) {
          fscanf(f, "%d %d", &min, &max);
          fclose(f);
          remove(file_name);
      }
    } else {
      read(pipefd[0], &min, sizeof(int));
      read(pipefd[0], &max, sizeof(int));
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);
  if (active_child_processes > 0 && timeout > 0) {
    free(child_pids);
    free(array);
    return 1;
  }

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
