#pragma once

#include "../protobuf-model/fs.pb-c.h"

struct _thread_args {
  int socket;
};

typedef struct _thread_args thread_args;

void create_file(Create *, FileResponse *);
void truncate_file(Truncate *, FileResponse *);
void *handle_request(void * args);
