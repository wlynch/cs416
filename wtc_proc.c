#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#include "wtc_proc.h"

/* shared vertices graph, will result in the transitive closure graph */
int * T, t;
int T_size;
int shared_memory_fd;
#define shared_memory_path "/tmp/wtc_proc_shared"

void wtc_proc_init(int * E, size_t size) {
  shared_memory_fd = shm_open(shared_memory_path,
                              O_CREAT | O_RDWR,
                              S_IRUSR | S_IWUSR);
  if (shared_memory_fd == -1) {
    perror("cannot open shared_memory_path with shm_open()");
    exit(1);
  }

  if (ftruncate(shared_memory_fd, size) == -1) {
    perror("cannot truncate shared_memory_path");
    close(shared_memory_fd);
    shm_unlink(shared_memory_path);
    exit(1);
  }

  T_size = size;
  T = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
  if (T == MAP_FAILED) {
    perror("could not mmap");
    exit(1);
  }
}

void wtc_proc(int * vertices, int n) {
  int i,j,k;

  for (k = 0; k < n; k++) {
    switch ( pid = fork() ) {
      case 0: /* child */

        break;
      case -1: /* error */
        perror("could not create a child process");
        exit(1);
        break;
      default: /* we are the parent */
        break;
    }
  }
}

void wtc_proc_cleanup() {
  munmap(T, T_size);
  close(shared_memory_fd);
  shm_unlink(shared_memory_path);
}

