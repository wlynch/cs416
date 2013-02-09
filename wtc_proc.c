#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "wtc_proc.h"

/* shared vertices graph, will result in the transitive closure graph */
int * T;
sem_t * sem;
pthread_mutex_t * lock;
pthread_cond_t * cond;

int AllocateSharedMemory(int n) {
  return shmget(IPC_PRIVATE, n, IPC_CREAT | SHM_R | SHM_W);
}

void* MapSharedMemory(int id) {
  void* addr;
  addr = shmat(id, NULL, 0); /* Attach the segment... */
  shmctl(id, IPC_RMID, NULL); /* ...and mark it destroyed. */
  return addr;
}

void * give_memory(size_t num_bytes) {
  return MapSharedMemory(AllocateSharedMemory(num_bytes));
}

void wtc_proc_init(int * initial_matrix, int n, int number_of_processes) {
  sem_t * temp_sem;
  int process_number;

  T = give_memory(sizeof(int) * n * n);
  sem = give_memory(sizeof(int));
  temp_sem = sem_open("/semaphore", O_CREAT, 0644, 0);

  memcpy(&sem, &temp_sem, sizeof(int));
  memcpy(T, initial_matrix, sizeof(int) * n * n);

  lock = give_memory(sizeof(pthread_mutex_t));
  cond = give_memory(sizeof(pthread_cond_t));

  pthread_mutex_init(lock, NULL);
  pthread_cond_init(cond, NULL);

  for (process_number = 0; process_number < number_of_processes; process_number++) {
    wtc_proc_create(process_number, number_of_processes, n);
  }
}

int * wtc_proc(int n, int number_of_processes) {
  int i, k;

  for (k = 0; k < n; k++) { /* for each vertex */
    /* Wait for all threads to finish before returning */
    for (i=0; i < number_of_processes; i++){
      printf("parent: waiting for %i to finish\n", i);
      sem_wait(sem);
    }

    /* send signal to start running again */
    pthread_mutex_lock(lock);
    fprintf(stderr, "parent: got lock\n");
    pthread_cond_broadcast(cond);
    fprintf(stderr, "parent: broadcast\n");
    pthread_mutex_unlock(lock);
    fprintf(stderr, "parent: unlocked\n");
  }

  return T;
}

void wtc_proc_create(int process_number, int number_of_processes, int n) {
  /* create forks, detach them, and make pools */
  int pid, i, j, k;

  pid = fork();
  switch (pid) {
    case -1:
      perror("we done fucked up"); exit(1);
      break;
    case 0:
      printf("p%i: hello world\n", process_number);
      for (k = 0; k < n; k++) { /* each k */
        fprintf(stderr, "p%i: starting k: %i\n", process_number, k);
        for (i = process_number; i < n; i += number_of_processes) { /* row */
          for (j = 0 ; j < n; j++) { /* column */
            T[j + i*n] = T[j + i*n] | (T[j + k*n] & T[k + i*n]);
          }
        }

        fprintf(stderr, "p%i: posting\n", process_number);
        /* announce that we finished the row */
        sem_post(sem);

        fprintf(stderr, "p%i: waiting for cond\n", process_number);
        /* wait to continue to work on the next k */
        pthread_mutex_lock(lock);
        fprintf(stderr, "p%i: got a lock\n", process_number);
        pthread_cond_wait(cond, lock);
        fprintf(stderr, "p%i: passed wait\n", process_number);
        pthread_mutex_unlock(lock);
      }
      exit(0);
      break;
  }
}

void wtc_proc_cleanup() {
  shmdt(T);
  shmdt(sem);
  shmdt(lock);
  shmdt(cond);
  if (sem_close(sem) == -1) {
    perror("sem_close"); exit(EXIT_FAILURE);
  }
  if (sem_unlink("/semaphore") == -1) {
    perror("sem_unlink"); exit(EXIT_FAILURE);
  }

}

