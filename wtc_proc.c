#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "wtc_proc.h"

/* shared vertices graph, will result in the transitive closure graph */
int * T;
sem_t * T_sem;

int AllocateSharedMemory(int n) {
  return shmget(IPC_PRIVATE, n, IPC_CREAT | SHM_R | SHM_W);
}

void* MapSharedMemory(int id) {
  void* addr;
  addr = shmat(id, NULL, 0); /* Attach the segment... */
  shmctl(id, IPC_RMID, NULL); /* ...and mark it destroyed. */
  return addr;
}

void wtc_proc_init(int * initial_matrix, int n, int number_of_processes) {
  sem_t * temp_sem;
  T = MapSharedMemory(AllocateSharedMemory(sizeof(int) * n * n));
  T_sem = MapSharedMemory(AllocateSharedMemory(sizeof(int)));
  temp_sem = sem_open("/semaphore", O_CREAT, 0644, number_of_processes);

  memcpy(&T_sem, &temp_sem, sizeof(int));
  memcpy(T, initial_matrix, sizeof(int) * n * n);
}

int * wtc_proc(int n) {
  int i,j,k;
  int pid;

  for (k = 0; k < n; k++) { /* for each vertex */
    for (i = 0 ; i < n; i++) { /* for each row */
      if (T[k + i*n]) { /* optimization, check the first of the row */
        sem_wait(T_sem);
        pid = fork();

        if (pid == -1) {
          perror("FFFFUUUUUUU"); exit(1);
        } else if (pid == 0) {
          printf("working on row %i\n", i);
          for (j = 0; j < n; j++) { /* for each column */
            T[j + i*n] = T[j + i*n] | (T[j + k*n] & T[k + i*n]);
          }
          sem_post(T_sem);
          exit(0);
        } else {
        }
      }
    }
  }

  return T;
}

void wtc_proc_cleanup() {
  shmdt(T);
  puts("unlinked T_shared_memory");
  shmdt(T_sem);
  puts("unlinked T_sem_shared_memory");
  if (sem_close(T_sem) == -1) {
    perror("sem_close"); exit(EXIT_FAILURE);
  }
  if (sem_unlink("/semaphore") == -1) {
    perror("sem_unlink"); exit(EXIT_FAILURE);
  }
}

