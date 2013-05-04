#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
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
  create_res = creat(full_path, input->mode);

  if(create_res < 0){
    create_res = errno;
  }

  free(full_path);
  create_handle.fd = create_res;
  create_handle.error_code = -errno;

  memcpy(resp, &create_handle, sizeof(create_handle));
}

void truncate_file(Truncate * input, StatusResponse * resp) {
  int truncate_res, num_bytes;
  char * full_path;

  StatusResponse truncate_handle = STATUS_RESPONSE__INIT;
  full_path = get_full_path(input->path);
  num_bytes = input->num_bytes;
  truncate_handle.retval = truncate(full_path, num_bytes);

  if (truncate_handle.retval < 0) {
    truncate_handle.has_err = 1;
    truncate_handle.err = errno;
  }

  memcpy(resp, &truncate_handle, sizeof(truncate_handle));
}

void close_file(Close * input, ErrorResponse * resp) {
  int close_res = close(input->fd);

  if (close_res < 0) {
    close_res = errno;
  }

  ErrorResponse close_handle = ERROR_RESPONSE__INIT;
  close_handle.error_code = close_res;

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

int get_attr(Simple * input, GetAttrResponse * resp){
  char * full_path;
  struct stat stat_buf;
  int res;

  full_path = get_full_path(input->path);
  res = stat(full_path, &stat_buf);

  resp->st_dev = stat_buf.st_dev;
  resp->st_ino = stat_buf.st_ino;
  resp->st_mode = stat_buf.st_mode;
  resp->st_nlink = stat_buf.st_nlink;
  resp->st_uid = stat_buf.st_uid;
  resp->st_gid = stat_buf.st_gid;
  resp->st_rdev = stat_buf.st_rdev;
  resp->atime = stat_buf.st_atime;
  resp->mtime = stat_buf.st_mtime;
  resp->ctime = stat_buf.st_ctime;
  resp->st_blksize = stat_buf.st_blksize;
  resp->st_blocks = stat_buf.st_blocks;
  resp->st_size = stat_buf.st_size;

  resp->error_code = res == 0 ? res : errno;
  free(full_path);

  return res == 0 ? res : errno;
}

void write_file(Write * input, size_t count, StatusResponse * response) {
  int res, fd = input->fd;
  off_t offset = input->offset;
  void *buf;
  memcpy(buf, input->data.data, sizeof(input->data.len));
  fprintf(stderr,"writing %d bytes\n",input->data.len);
  res = pwrite(fd, buf, input->data.len, offset);
  response->retval = res;
  if (res < 0) {
    response->has_err = 1;
    response->err = errno;
  }
  
}

void *read_help(Read * input, ReadResponse *resp) {
  int res, errors;
  void * buffer = malloc(input->num_bytes);

  res = pread(input->fd, buffer, input->num_bytes, input->offset);
  resp->error_code = res >= 0 ? 0 : errno;
  resp->bytes_read = res >= 0 ? res : 0;
  resp->data.data = buffer;
  resp->data.len = input->num_bytes;

  return buffer;
}

void make_dir(Create * input, ErrorResponse * resp){
 int error, res;
 char * full_path = get_full_path(input->path);
 res = mkdir(full_path, input->mode);
 free(full_path);

 if(res < 0){
   perror("make directory");
   error = errno;
 }

 resp->error_code = res == 0 ? res : error;
}

void open_dir(Simple * input, ErrorResponse * resp){
  char * full_path = get_full_path(input->path);
  DIR * dp = opendir(full_path);
  
  if(dp != NULL){
    closedir(dp);    
  }

  free(full_path);
  
  resp->error_code = dp == NULL ? errno : 0;
}

void read_directory(Simple * input, ReadDirResponse * resp){
  int error, result, i = 0, file_count = 0;
  char * full_path;
  DIR * dp, * dir_front;
  struct dirent * dirent;
  DirRecord init_dir = DIR_RECORD__INIT;

  full_path = get_full_path(input->path);
  dp = opendir(full_path);
  dir_front = opendir(full_path);
  free(full_path);

  // if the directory couldn't be opened, return an error
  if(dp == NULL || dir_front == NULL){
    resp->n_records = 0;
    resp->error_code = errno;
    return;
  }

  dirent = readdir(dp);
  // if dirent was null after we opened the directory, there is an erro
  if(dirent == NULL){
    resp->n_records = 0;
    resp->error_code = errno;
    return;
  }
 
  // get the number of files for this directory we do while because we already
  // read the first one
  do
  {
    file_count++;
  }
  while((dirent = readdir(dp)) != NULL);

  resp->n_records = file_count;
  resp->records = malloc(sizeof(DirRecord *) * file_count);

  //next, fill up all those lovely dir records
  while((dirent = readdir(dir_front)) != NULL){
    resp->records[i] = malloc(sizeof(DirRecord));
    memcpy(resp->records[i], &init_dir, sizeof(init_dir));
    resp->records[i]->name = strdup(dirent->d_name);
    i++;
  }
  resp->error_code = 0;

  closedir(dp);
  closedir(dir_front);

  //any cleanup needs to happen after packing
}

void clean_readdir(ReadDirResponse *resp){
  int i = 0;

  // free the mallocs for each individual records
  for(; i < resp->n_records; i++){
    free(resp->records[i]->name);
    free(resp->records[i]);
  }

  free(resp->records);
}
