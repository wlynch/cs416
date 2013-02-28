#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared_memory.h"

int allocate_shared_memory(int n) {
  return shmget(IPC_PRIVATE, n, IPC_CREAT | SHM_R | SHM_W);
}

void * map_shared_memory(int id) {
  void * address = shmat(id, NULL, 0);
  shmctl(id, IPC_RMID, NULL); /* mark as destroyed */
  return address;
}

void * share_memory(size_t num_bytes) {
  return map_shared_memory(allocate_shared_memory(num_bytes));
}
