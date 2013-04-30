#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include "threading.h"
#include "filesystem.h"
#include "../protobuf-model/fs.pb-c.h"

void *create_file(void * args)
{
  int create_res;
  char * full_path;
  void * closure_data;
  thread_args * data = (thread_args *)args; 
  Create * input = (Create *)data->input;
  FileResponse_Closure closure = (FileResponse_Closure)data->closure;
  closure_data = data->closure_data;

  FileResponse create_handle = FILE_RESPONSE__INIT;
  full_path = get_full_path(input->path);
  create_res = creat(full_path, input->mode);

  if(create_res < 0){
    create_res = -errno; 
  }
  
  printf("create_res has a value of %d\n", create_res);
  free(full_path);
  create_handle.fd = create_res;
  create_handle.error_code = errno;

  closure(&create_handle, closure_data);

  return NULL;

}
