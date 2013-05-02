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
#include "thread_helpers.h"
#include "../message_def.h"
#include "filesystem.h"
#include "../protobuf-model/fs.pb-c.h"

void create_file(Create * input, FileResponse * resp) {
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
  create_handle.error_code = -errno;

  memcpy(resp, &create_handle, sizeof(create_handle));
}

void truncate_file(Truncate * input, FileResponse * resp) {
  
}

void open_file(Open* input, FileResponse* resp) {
  
  int open_fd, open_errors;
  char* full_path

  full_path = get_full_path(input->path);  
  open_fd = open(full_path, input->flags);

  if (open_fd < 0) {
    open_errors = -errno;
  }
  
  print_f("open_fd has a value of %d", open_fd);
  fprintf(stderr, "full path is %s\n", full_path);

  FileResponse open_handle = FILE_RESPONSE__INIT;
  open_handle.fd = open_fd;
  open_handle.error = open_errors;
  open_handle.is_done = 1;

  memcpy(resp, &open_handle, sizeof(open_handle));

  free(full_path);
}
