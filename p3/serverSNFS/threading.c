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
  message_size = ntohl(message_size); 
  message_type = ntohl(message_type);
  message_buffer = malloc(message_size);
  read(thr_arg->socket, message_buffer, message_size);
  
  switch (message_type)
  {
    case CREATE_MESSAGE:
      {
        Create * create = create__unpack(NULL, message_size, message_buffer);
        FileResponse *resp = malloc(sizeof(FileResponse));
        create_file(create, resp);
        
        uint32_t send_size = file_response__get_packed_size(resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        // ignore the length when writing the length of the message
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(FILE_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        file_response__pack(resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }
        fprintf(stderr, "successfully sent the message over back to the client\n"\
            "error code is %d\n and fd is %d\n", resp->error_code, resp->fd);
        free(resp);
        break;
      }
  }

  close(thr_arg->socket);
  free(message_buffer);
  free(args);
  return NULL;
}

void create_file(Create * input, FileResponse * resp)
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
  fprintf(stderr, "full path is %s\n", full_path);
  free(full_path);
  create_handle.fd = create_res;
  create_handle.error_code = errno;

  memcpy(resp, &create_handle, sizeof(create_handle));
}
