#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <fuse.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/fs.pb-c.h"

#include "rpc.h"
#include "externs.h"
#include "log.h"
#include "fuse_ops_helpers.h"
#include "../message_def.h"

static char log_buffer[256];

static int _getattr(const char *path, struct stat *stbuf) {
  log_msg("logging get_attr");
  sprintf(log_buffer, "path being logged is %s", path);
  log_msg(log_buffer);
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, net_data_size, message_type, receive_size;
  GetAttrResponse * resp;

  Simple attr_req = SIMPLE__INIT;
  attr_req.path = strdup(path);

  /* Pack code */

  send_size = simple__get_packed_size(&attr_req) + 2*sizeof(uint32_t);

  send_buffer = malloc(send_size);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
  message_type = htonl(GETATTR_MESSAGE);
  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  simple__pack(&attr_req, send_buffer + 2 * sizeof(uint32_t));

  /* Send code */

  int sock = socket(AF_INET, SOCK_STREAM, 0);;
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if(connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  write(sock, send_buffer, send_size);

  /* Receive code */

  read(sock, &receive_size, sizeof(receive_size));
  read(sock, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);

  receive_buffer = malloc(receive_size);
  read(sock, receive_buffer, receive_size);
  resp = get_attr_response__unpack(NULL, receive_size, receive_buffer);
  parse_get_attr(resp, stbuf);

  sprintf(log_buffer, "GetAttr: Error code is %d and uid is %u our uid is %d\n", resp->error_code, stbuf->st_uid, getuid());
  log_msg(log_buffer);

  close(sock);
  free(send_buffer);
  free(receive_buffer);

  return -1 * resp->error_code;
}

static int _readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  log_msg("logging readdir");
  sprintf(log_buffer, "path being logged is %s", path);
  log_msg(log_buffer);
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, net_data_size, message_type, receive_size;
  int error, i;
  ReadDirResponse * resp;

  Simple readdir_req = SIMPLE__INIT;
  readdir_req.path = strdup(path);

  /* Pack code */

  send_size = simple__get_packed_size(&readdir_req) + 2*sizeof(uint32_t);

  send_buffer = malloc(send_size);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
  message_type = htonl(READDIR_MESSAGE);
  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  simple__pack(&readdir_req, send_buffer + 2 * sizeof(uint32_t));

  /* Send code */

  int sock = socket(AF_INET, SOCK_STREAM, 0);;
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if(connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  write(sock, send_buffer, send_size);

  /* Receive code */

  read(sock, &receive_size, sizeof(receive_size));
  read(sock, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);

  receive_buffer = malloc(receive_size);
  read(sock, receive_buffer, receive_size);
  resp = read_dir_response__unpack(NULL, receive_size, receive_buffer);
  error = resp->error_code;
  
  // if there was no error, fill that ish in
  if(error == 0){
    for(i = 0; i < resp->n_records; i++){
      if(filler(buf, resp->records[i]->name, NULL, 0) != 0){
        log_msg("out of error when doing readdir");
        return -ENOMEM;
      }
    }
  }


  close(sock);
  read_dir_response__free_unpacked(resp, NULL);
  free(send_buffer);
  free(receive_buffer);
  free(readdir_req.path);
  return -1 * error;
}

static int _create(const char *path, mode_t mode, struct fuse_file_info *fi){
  log_msg("logging in create");
  sprintf(log_buffer, "path being created is %s", path);
  log_msg(log_buffer);
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, net_data_size, message_type, receive_size;
  int error, i;
  FileResponse * resp;

  Create create = CREATE__INIT;
  create.path = strdup(path);
  create.mode = mode;

  /* Pack code */

  send_size = create__get_packed_size(&create) + 2*sizeof(uint32_t);

  send_buffer = malloc(send_size);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
  message_type = htonl(CREATE_MESSAGE);
  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  create__pack(&create, send_buffer + 2 * sizeof(uint32_t));

  /* Send code */

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if(connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  int bytes_written = write(sock, send_buffer, send_size);
  sprintf(log_buffer, "bytes_written is %d", bytes_written);
  log_msg(log_buffer);

  /* Receive code */

  read(sock, &receive_size, sizeof(send_size));
  read(sock, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  receive_buffer = malloc(receive_size);
  read(sock, receive_buffer, receive_size);
  resp = file_response__unpack(NULL, receive_size, receive_buffer);
  sprintf(log_buffer, "Create: file descriptor is %d and error code is %d\n", resp->fd, resp->error_code);
  log_msg(log_buffer);

  free(create.path);
  free(receive_buffer);
  free(send_buffer);
  file_response__free_unpacked(resp, NULL);
  close(sock);

  if(resp->fd > 0){
    fi->fh = resp->fd;
  }

  return resp->fd > 0 ? 0 : -1 * resp->error_code;

}

static int _truncate(const char *path, off_t length, struct fuse_file_info *fi) {
  log_msg("logging in truncate");
  Truncate truncate = TRUNCATE__INIT;
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, receive_size, net_data_size, message_type;

  FileResponse * resp;

  truncate.path = strdup(path);
  truncate.num_bytes = length;

  send_size = truncate__get_packed_size(&truncate) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size);
  // ignore the length when writing the length of the message
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
  message_type = htonl(TRUNCATE_MESSAGE);
  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  truncate__pack(&truncate, send_buffer + 2 * sizeof(uint32_t));

  int sock = socket(AF_INET, SOCK_STREAM, 0);;
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if(connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  /* send the truncate message */
  int bytes_written = write(sock, send_buffer, send_size);
  free(send_buffer);
  sprintf(log_buffer, "bytes_written is %d", bytes_written);
  log_msg(log_buffer);

  /* wait for the response */
  read(sock, &receive_size, sizeof(send_size));
  read(sock, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  read(sock, receive_buffer, receive_size);
  resp = file_response__unpack(NULL, receive_size, receive_buffer);

  sprintf(log_buffer, "Create: file descriptor is %d and error code is %d\n", resp->fd, resp->error_code);
  log_msg(log_buffer);

  close(sock);
  free(receive_buffer);
  file_response__free_unpacked(resp, NULL);

  return resp->fd > 0 ? 0 : resp->fd;
}

static int _release(char * path, struct fuse_file_info * fi) {
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, receive_size, net_data_size, message_type;

  Close close_struct = CLOSE__INIT;
  ErrorResponse * resp;
  close_struct.fd = fi->fh;

  log_msg("logging in close");

  send_size = close__get_packed_size(&close_struct) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size);
  // ignore the length when writing the length of the message
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
  message_type = htonl(CLOSE_MESSAGE);
  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  close__pack(&close_struct, send_buffer + 2 * sizeof(uint32_t));

  /* send the message */
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  int bytes_written = write(sock, send_buffer, send_size);
  sprintf(log_buffer, "bytes_written is %d", bytes_written);
  log_msg(log_buffer);

  /* get that response */
  read(sock, &receive_size, sizeof(send_size));
  read(sock, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  receive_buffer = malloc(receive_size);
  read(sock, receive_buffer, receive_size);
  resp = error_response__unpack(NULL, receive_size, receive_buffer);

  sprintf(log_buffer, "Error code is %d\n", resp->error_code);
  log_msg(log_buffer);

  close(sock);
  free(receive_buffer);
  free(send_buffer);
  error_response__free_unpacked(resp, NULL);

  return -1 * resp->error_code;
}

static int _ex_open(const char *path, struct fuse_file_info *fi) {
  log_msg("logging open");
  sprintf(log_buffer, "path is %s and flags are %d", path, fi->flags);
  log_msg(log_buffer);
  uint32_t send_size, net_data_size, message_type, receive_size;
  void* receive_buffer;
  void* send_buffer;

  Open open_struct = OPEN__INIT;

  open_struct.path = strdup(path);
  open_struct.flags = fi->flags;

  send_size = open__get_packed_size(&open_struct) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size - 2*sizeof(uint32_t));
  message_type = htonl(OPEN_MESSAGE);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));

  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));

  open__pack(&open_struct, send_buffer + 2*sizeof(uint32_t));


  /* All the sockets */
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int connected = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  write(socket_fd, send_buffer, send_size);

  /* Reading things back */
  read(socket_fd, &receive_size, sizeof(send_size));
  read(socket_fd, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  receive_buffer = malloc(receive_size);
  read(socket_fd, receive_buffer, receive_size);

  FileResponse * resp = file_response__unpack(NULL, receive_size, receive_buffer);

  sprintf(log_buffer, "file descriptor is %d and error code is %d\n", resp->fd, resp->error_code);
  log_msg(log_buffer);

  close(socket_fd);
  free(open_struct.path);
  free(receive_buffer);
  free(send_buffer);

  if(resp->fd > 0){
    fi->fh = resp->fd;
  }

  return resp->fd > 0 ? 0 : -1 * resp->error_code;
}

static int _read(const char * path, const char * buffer, size_t size, off_t off,
    struct fuse_file_info *fi){

  log_msg("logging read");
  sprintf(log_buffer, "path is %s", path);
  log_msg(log_buffer);
  uint32_t send_size, net_data_size, message_type, receive_size;
  void* receive_buffer;
  void* send_buffer;

  Read read_struct = READ__INIT;
  read_struct.fd = fi->fh;
  read_struct.num_bytes = size;
  read_struct.offset = off;

  send_size = read__get_packed_size(&read_struct) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size - 2*sizeof(uint32_t));
  message_type = htonl(READ_MESSAGE);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));

  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));

  read__pack(&read_struct, send_buffer + 2*sizeof(uint32_t));

  /* All the sockets */
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int connected = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }
  
  write(socket_fd, send_buffer, send_size);
  
  /* Reading things back */
  read(socket_fd, &receive_size, sizeof(send_size));
  read(socket_fd, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  receive_buffer = malloc(receive_size);
  read(socket_fd, receive_buffer, receive_size);
  log_msg("successfully read a message into the receive buffer");

  ReadResponse *resp = read_response__unpack(NULL, receive_size, receive_buffer);
  if(resp->error_code == 0){
    memcpy(buffer, resp->data.data, resp->data.len); 
  }

 
  free(send_buffer);
  free(receive_buffer);
  return resp->bytes_read >= 0 ? resp->bytes_read : -1 * resp->error_code;
}

static int _mkdir(char * path, mode_t mode){
  log_msg("logging mkdir");
  sprintf(log_buffer, "log path is %s", path);
  log_msg(log_buffer);
  uint32_t send_size, net_data_size, message_type, receive_size;
  void* receive_buffer;
  void* send_buffer;

  Create create_struct = CREATE__INIT;
  create_struct.path = path;
  create_struct.mode = mode;

  send_size = create__get_packed_size(&create_struct) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size - 2*sizeof(uint32_t));
  message_type = htonl(MKDIR_MESSAGE);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));

  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));

  create__pack(&create_struct, send_buffer + 2*sizeof(uint32_t));

  /* All the sockets */
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int connected = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  write(socket_fd, send_buffer, send_size);
  
  /* Reading things back */
  read(socket_fd, &receive_size, sizeof(send_size));
  read(socket_fd, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  receive_buffer = malloc(receive_size);
  read(socket_fd, receive_buffer, receive_size);
  log_msg("successfully read a message into the receive buffer");

  ErrorResponse *resp = error_response__unpack(NULL, receive_size, receive_buffer);

  free(send_buffer);
  free(receive_buffer);
  return -1 * resp->error_code;
}

static int _opendir(char *path, struct fuse_file_info *fi){
  log_msg("logging opendir");
  sprintf(log_buffer, "log path is %s", path);
  log_msg(log_buffer);
  uint32_t send_size, net_data_size, message_type, receive_size;
  void* receive_buffer;
  void* send_buffer;

  Simple simple_struct = SIMPLE__INIT;
  simple_struct.path = path; 

  send_size = simple__get_packed_size(&simple_struct) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size - 2*sizeof(uint32_t));
  message_type = htonl(OPENDIR_MESSAGE);
  net_data_size = htonl(send_size - 2 * sizeof(uint32_t));

  memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));

  simple__pack(&simple_struct, send_buffer + 2*sizeof(uint32_t));

  /* All the sockets */
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  int connected = connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (connected < 0) {
    perror("cannot connect: ");
    free(send_buffer);
    return -1;
  }

  write(socket_fd, send_buffer, send_size);
  
  /* Reading things back */
  read(socket_fd, &receive_size, sizeof(send_size));
  read(socket_fd, &message_type, sizeof(message_type));
  receive_size = ntohl(receive_size);
  message_type = ntohl(message_type);
  receive_buffer = malloc(receive_size);
  read(socket_fd, receive_buffer, receive_size);
  log_msg("successfully read a message into the receive buffer");

  ErrorResponse *resp = error_response__unpack(NULL, receive_size, receive_buffer);
  free(send_buffer);
  free(receive_buffer);

  sprintf(log_buffer, "error code is %d\n", resp->error_code);
  log_msg(log_buffer);

  return -1 * resp->error_code;
}

static int _releasedir(char *path, struct fuse_file_info * fi){
  // because we never REALLY open up directories, this always returns zero
  return 0;
}

struct fuse_operations ops = {
  .readdir = _readdir,
  .getattr = _getattr,
  .read = _read,
  .open = _ex_open,
  .create = _create,
  .release = _release,
  .truncate = _truncate,
  .mkdir = _mkdir,
  .opendir = _opendir,
  .releasedir = _releasedir
};
