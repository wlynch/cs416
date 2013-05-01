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
#include "../message_def.h"

static char log_buffer[256];

static int _getattr(const char *path, struct stat *stbuf) {
  int res = 0;

  memset(stbuf, 0, sizeof(struct stat));
  if (strcmp(path, "/") == 0) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
  } else if (strcmp(path, "/hello") == 0) {
    stbuf->st_mode = S_IFREG | 0444;
    stbuf->st_nlink = 1;
    stbuf->st_size = strlen("Hello world");
  } else {
    res = -ENOENT;
  }

  return res;
}

static int _readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  fprintf(stderr, "reading %s\n", path);
  fflush(stderr);
  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);
  filler(buf, "FAKE", NULL, 0);

  Ping ping = PING__INIT;
  ping.message = strdup("reddir result...");
  protobuf_c_boolean is_done = 0;
  /*fsservice__reply_to_ping(rpc_service, &ping, handle_ping_response, &is_done);*/

  return 0;
}

static int _create(const char *path, mode_t mode, struct fuse_file_info *fi){

  log_msg("logging in create");
  Create create = CREATE__INIT;
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, byte_order, message_type;
  create.path = strdup(path);
  create.mode = mode;
  create.type = CREATE_MESSAGE;

  FileResponse is_done = FILE_RESPONSE__INIT; 
  
  send_size = create__get_packed_size(&create) + 2*sizeof(uint32_t);
  sprintf(message_buffer, "size which should be read is %d\n", send_size - sizeof(uint32_t));
  log_msg(message_buffer);

  send_buffer = malloc(send_size);
  // ignore the length when writing the length of the message
  byte_order = htonl(send_size - sizeof(uint32_t));
  message_type = htonl(CREATE_MESSAGE);
  memcpy(send_buffer, &byte_order, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  create__pack(&create, send_buffer + 2 * sizeof(uint32_t));
 
  int sock = socket(AF_INET, SOCK_STREAM, 0);;
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  int bytes_written = write(sock, send_buffer, send_size);
  sprintf(log_buffer, "bytes_written is %d", bytes_written);
  log_msg(log_buffer);

  close(sock);

  return is_done.fd > 0 ? 0 : is_done.fd;

}

static int _truncate(const char *path, off_t length, struct fuse_file_info *fi){

  log_msg("logging in truncate");
  Truncate truncate = TRUNCATE__INIT;
  void *send_buffer;
  void *receive_buffer;
  uint32_t send_size, byte_order, message_type;
  truncate.path = strdup(path);
  truncate.num_bytes = length;
  truncate.type = TRUNCATE_MESSAGE;

  FileResponse is_done = FILE_RESPONSE__INIT; 
  
  send_size = truncate__get_packed_size(&truncate) + 2*sizeof(uint32_t);
  send_buffer = malloc(send_size);
  // ignore the length when writing the length of the message
  byte_order = htonl(send_size - sizeof(uint32_t));
  message_type = htonl(TRUNCATE_MESSAGE);
  memcpy(send_buffer, &byte_order, sizeof(uint32_t));
  memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
  truncate__pack(&truncate, send_buffer + 2 * sizeof(uint32_t));
 
  int sock = socket(AF_INET, SOCK_STREAM, 0);;
  int connected = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  int bytes_written = write(sock, send_buffer, send_size);
  sprintf(log_buffer, "bytes_written is %d", bytes_written);
  log_msg(log_buffer);

  close(sock);

  return is_done.fd > 0 ? 0 : is_done.fd;
}

static int _ex_open(const char *path, struct fuse_file_info *fi) {
  if (strcmp(path, "/") != 0)
    return -ENOENT;

  if ((fi->flags & 3) != O_RDONLY)
    return -EACCES;

  return 0;
}

struct fuse_operations ops = {
  .readdir = _readdir,
  .getattr = _getattr,
  .open = _ex_open,
  .create = _create
};
