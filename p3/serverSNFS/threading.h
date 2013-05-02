#pragma once

#include "../protobuf-model/fs.pb-c.h"

struct _thread_args {
  int socket;
};

typedef struct _thread_args thread_args;

void *handle_request(void * args);
