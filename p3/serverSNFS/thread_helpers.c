#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "threading.h"
#include "thread_helpers.h"
#include "filesystem.h"

#include "../message_def.h"
#include "../protobuf-model/fs.pb-c.h"

void create_file(Create * input, FileResponse * resp) {
  int create_res;
  char * full_path;

  FileResponse create_handle = FILE_RESPONSE__INIT;
  full_path = get_full_path(input->path);
  printf("\tcreate_file %s\n", full_path);
  create_res = creat(full_path, input->mode);

  if(create_res < 0){
    create_res = errno;
  }

  printf("create_res has a value of %d\n", create_res);
  fprintf(stderr, "full path is %s\n", full_path);
  free(full_path);
  create_handle.fd = create_res;
  create_handle.error_code = -errno;

  memcpy(resp, &create_handle, sizeof(create_handle));
}

void truncate_file(Truncate * input, FileResponse * resp) {
  int truncate_res, num_bytes;
  char * full_path;

  printf("\ttruncate_file\n");

  FileResponse truncate_handle = FILE_RESPONSE__INIT;
  full_path = get_full_path(input->path);
  num_bytes = input->num_bytes;
  truncate_res = truncate(full_path, num_bytes);

  if (truncate_res < 0) {
    truncate_res = errno;
  }

  printf("truncate_res has a value of %d\n", truncate_res);
  fprintf(stderr,"full path is %s\n", full_path);
  truncate_handle.error_code = truncate_res;
  memcpy(resp, &truncate_handle, sizeof(truncate_handle));
}

void close_file(Close * input, FileResponse * resp) {
  int close_res = close(input->fd);

  printf("\tclose_file\n");

  if (close_res < 0) {
    close_res = errno;
  }

  printf("close_res hash a value of %d\n", close_res);
  fprintf(stderr, "fd: %i\n", input->fd);

  FileResponse close_handle = FILE_RESPONSE__INIT;
  close_handle.fd = input->fd;
  close_handle.error_code = close_res;
  close_handle.is_done = 1;

  memcpy(resp, &close_handle, sizeof(close_handle));
}

void open_file(Open* input, FileResponse* resp) {
  int open_fd, open_errors = 0;
  char * full_path;

  full_path = get_full_path(input->path);
  open_fd = open(full_path, input->flags);

  if (open_fd < 0) {
    open_errors = errno;
  }

  FileResponse open_handle = FILE_RESPONSE__INIT;
  open_handle.fd = open_fd;
  open_handle.error_code = open_errors;
  open_handle.is_done = 1;

  memcpy(resp, &open_handle, sizeof(open_handle));

  free(full_path);
}

int get_attr(Simple * input, GetAttrResponse * response){
  char * full_path;
  struct stat stat_buf;
  int res;

  full_path = get_full_path(input->path);
  res = stat(full_path, &stat_buf);

  if (res < 0) {
    printf("errno: %i, path: %s\n", errno, full_path);
    fflush(stdout);
    perror("stat");
  }

  response->st_dev = stat_buf.st_dev;
  response->st_ino = stat_buf.st_ino;
  response->st_mode = stat_buf.st_mode;
  response->st_nlink = stat_buf.st_nlink;
  response->st_uid = stat_buf.st_uid;
  response->st_gid = stat_buf.st_gid;
  response->st_rdev = stat_buf.st_rdev;
  response->atime = stat_buf.st_atime;
  response->mtime = stat_buf.st_mtime;
  response->ctime = stat_buf.st_ctime;
  response->st_blksize = stat_buf.st_blksize;
  response->st_blocks = stat_buf.st_blocks;
  response->st_size = stat_buf.st_size;

  response->error_code = res == 0 ? res : errno;
  free(full_path);

  return res == 0 ? res : errno;
}

void write_file(Write * input, size_t count, StatusResponse * response) {
  int res, fd = input->fd;
  void *buf;
  memcpy(buf, input->data.data, sizeof(input->data.len));

  res = write(fd, buf, count);
  response->retval = res;
  if (res < 0) {
    response->has_err = 1;
    response->err = errno;
  }
  
}

void *read_help(Read * input, ReadResponse *response) {
  int res, errors;
  void * buffer = malloc(input->num_bytes);

  res = pread(input->fd, buffer, input->num_bytes, input->offset);
  response->error_code = res >= 0 ? 0 : errno;
  response->bytes_read = res >= 0 ? res : 0;
  response->data.data = buffer;
  response->data.len = input->num_bytes;

  return buffer;
}
