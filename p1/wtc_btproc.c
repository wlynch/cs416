#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/shm.h>

#include "wtc_btproc.h"
#include "shared_memory.h"

/* shared vertices graph, will result in the transitive closure graph */
int * T;
sem_t * sem;
pthread_mutex_t * lock;
pthread_cond_t * cond;

pthread_mutex_t * row_lock, * other_lock;
pthread_cond_t * k_cond;
int * row, * running, * k;
int number_of_vertices;

void wtc_proc_bt_init(int * initial_matrix, int n, int number_of_processes) {
  sem_t * temp_sem;
  int process_number;
  pthread_condattr_t cond_attr;
  pthread_mutexattr_t lock_attr;

  T = share_memory(sizeof(int) * n * n);
  sem = share_memory(sizeof(int));
  temp_sem = sem_open("/semaphore", O_CREAT, 0644, 0);

  memcpy(&sem, &temp_sem, sizeof(int));
  memcpy(T, initial_matrix, sizeof(int) * n * n);

  lock = share_memory(sizeof(pthread_mutex_t));
  cond = share_memory(sizeof(pthread_cond_t));

  other_lock = share_memory(sizeof(pthread_mutex_t));
  row_lock = share_memory(sizeof(pthread_mutex_t));
  k_cond = share_memory(sizeof(pthread_cond_t));
  running = share_memory(sizeof(int));
  row = share_memory(sizeof(int));
  k = share_memory(sizeof(int));

  *k = 0;
  *row = 0;
  *running = 1;
  number_of_vertices = n;

  pthread_mutexattr_init(&lock_attr);
  pthread_mutexattr_setpshared(&lock_attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(lock, &lock_attr);
  pthread_mutex_init(row_lock, &lock_attr);
  pthread_mutex_init(other_lock, &lock_attr);

  pthread_condattr_init(&cond_attr);
  pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
  pthread_cond_init(cond, &cond_attr);
  pthread_cond_init(k_cond, &cond_attr);

  for (process_number = 0; process_number < number_of_processes; process_number++) {
    wtc_proc_bt_create(process_number, number_of_processes, n);
  }
}

/* wtc_proc_bt_dequeue a single row to be operated on */
int wtc_proc_bt_dequeue() {
    int retval;
    if (*row < number_of_vertices) {
        retval = *row;
        *row += 1;
    } else {
        retval = -1;
    }
    return retval;
}

int * wtc_proc_bt(int n, int number_of_processes) {
  int i;

  while (*k < n) { /* for each vertex */
    /* Wait for all threads to finish before returning */
    for (i = 0; i < number_of_processes; i++){
      sem_wait(sem);
    }

    pthread_mutex_lock(row_lock);
    *row = 0;
    pthread_mutex_unlock(row_lock);

    /* send signal to start running again */
    pthread_mutex_lock(other_lock);
    *k += 1;
    pthread_cond_broadcast(k_cond);
    pthread_mutex_unlock(other_lock);
  }

  /* no more running */
  *running = 0;

  /* wait for all of the processes to finish */
  for (i = 0; i < number_of_processes; i++) {
    sem_wait(sem);
  }

  return T;
}

void wtc_proc_bt_create(int process_number, int number_of_processes, int n) {
  /* create forks, detach them, and make pools */
  int pid, i, j;

  pid = fork();
  switch (pid) {
    case -1:
      perror("fork"); exit(1);
      break;
    case 0:
      while (*running) {
        pthread_mutex_lock(row_lock);
        while (*row < n) {
          i = wtc_proc_bt_dequeue();
          pthread_mutex_unlock(row_lock);

          if (i >= 0) {
            for (j = 0 ; j < n; j++) { /* column */
              T[j + i*n] = T[j + i*n] | (T[j + (*k)*n] & T[(*k) + i*n]);
            }
          }

          pthread_mutex_lock(row_lock);
        }
        pthread_mutex_unlock(row_lock);

        pthread_mutex_lock(other_lock);
        sem_post(sem);
        pthread_cond_wait(k_cond, other_lock);
        pthread_mutex_unlock(other_lock);
      }
      sem_post(sem);
      exit(0);
      break;
  }
}

void wtc_proc_bt_cleanup() {
  shmdt(lock);
  shmdt(cond);
  shmdt(sem);
  shmdt(T);

  shmdt(other_lock);
  shmdt(row_lock);
  shmdt(k_cond);
  shmdt(row);
  shmdt(k);

  if (sem_close(sem) == -1) {
    perror("sem_close"); exit(EXIT_FAILURE);
  }
  if (sem_unlink("/semaphore") == -1) {
    perror("sem_unlink"); exit(EXIT_FAILURE);
  }

}

