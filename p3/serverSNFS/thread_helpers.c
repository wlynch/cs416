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
