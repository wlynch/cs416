#ifndef _WTC_PROC_H
#define _WTC_PROC_H

struct shared_memory {
  void * address;
  int fd, size;
};

void wtc_proc_init(int * T, int number_of_vertices, int number_of_processes);
int * wtc_proc(int n);
void wtc_proc_cleanup();

#endif

