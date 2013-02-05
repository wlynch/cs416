#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include <semaphore.h>

#include "wtc_proc.h"

/* shared vertices graph, will result in the transitive closure graph */
int * T;
sem_t T_sem;
struct shared_memory * T_shared_memory;
#define shared_memory_path "/tmp/wtc_proc_shared4"

struct shared_memory * share_memory(char * path, size_t size) {
  int shared_fd;
  void * map;

  shared_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
  if (shared_fd == -1) {
    perror("cannot open shared memory"); exit(1);
  }

  if (ftruncate(shared_fd, size) == -1) {
    perror("cannot truncate shared memory fd");
    goto failure;
  }

  map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_fd, 0);
  if (map == MAP_FAILED) {
    perror("could not mmap");
    goto failure;
  }

  struct shared_memory * shm = malloc(sizeof(struct shared_memory));
  shm->address = map;
  shm->fd = shared_fd;
  shm->size = size;

  return shm;
  failure:
  close(shared_fd);
  shm_unlink(path); exit(1);
}

void unshare_memory(struct shared_memory * memory, char * path) {
  int ret = munmap(memory->address, memory->size);
  if (ret == -1) {
    perror("munmap"); exit(1);
  }

  ret = close(memory->fd);
  if (ret == -1) {
    perror("close of shared_memory_fd"); exit(1);
  }

  ret = shm_unlink(path);
  if (ret == -1) {
    perror("shm_unlink failed. should change shared_memory_path"); exit(1);
  }
}

void wtc_proc_init(int * initial_matrix, int n, int number_of_processes) {
  T_shared_memory = share_memory(shared_memory_path, sizeof(int) * n * n);
  T = T_shared_memory->address;
  memcpy(T, initial_matrix, sizeof(int) * n * n);
  sem_init(&T_sem, 1, number_of_processes);
}

int * wtc_proc(int n) {
  int i,j,k;

  for (k = 0; k < n; k++) { /* for each vertex */
    for (i = 0 ; i < n; i++) { /* for each row */
      if (T[k + i*n]) { /* optimization, check the first of the row */
        for (j = 0; j < n; j++) { /* for each column */
          T[j + i*n] = T[j + i*n] | (T[j + k*n] & T[k + i*n]);
        }
      }
    }
  }

  return T;
}

void wtc_proc_cleanup() {
  unshare_memory(T_shared_memory, shared_memory_path);
}

