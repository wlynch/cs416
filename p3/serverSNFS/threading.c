#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "threading.h"
#include "../message_def.h"
#include "filesystem.h"
#include "../protobuf-model/fs.pb-c.h"

void *handle_request(void * args){
 
  int bytes_read;
  thread_args * thr_arg = (thread_args *)args;
  uint32_t message_type, message_size;
  void *message_buffer;

  bytes_read = read(thr_arg->socket, &message_size, sizeof(message_size));
  
  // TODO: HANDLE ERRORS

  read(thr_arg->socket, &message_type, sizeof(message_type));
  message_size = ntohl(message_size) - sizeof(message_type); 
  message_type = ntohl(message_type);
  message_buffer = malloc(message_size);
  read(thr_arg->socket, message_buffer, message_size);
  
  switch (message_type)
  {
    case CREATE_MESSAGE:
      {
        Create * create;
        create = create__unpack(NULL, message_size, message_buffer);
        FileResponse *resp;
      }
  }

  close(thr_arg->socket);
  free(message_buffer);
  free(args);
  return NULL;
}

void create_file(Create * input)
{
  int create_res;
  char * full_path;

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

  FileResponse * resp = malloc(sizeof(FileResponse));
  *resp = create_handle;
  return resp;
}
