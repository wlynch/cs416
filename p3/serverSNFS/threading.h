#pragma once
struct _thread_args {
  void * closure_data;
  void * closure;
  void * input;
};

typedef struct _thread_args thread_args;

void *create_file(void *);

