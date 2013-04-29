#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>

#include <fuse.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/ping.pb-c.h"
#include "../protobuf-model/fs.pb-c.h"

#include "rpc.h"
#include "externs.h"

static int getattr(const char *path, struct stat *stbuf) {
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

static int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  fprintf(stderr, "reading %s\n", path);
  fflush(stderr);
  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);
  filler(buf, "FAKE", NULL, 0);

  Ping ping = PING__INIT;
  ping.message = strdup("reddir result...");
  protobuf_c_boolean is_done = 0;
  fsservice__reply_to_ping(rpc_service, &ping, handle_ping_response, &is_done);

  while (!is_done)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  return 0;
}

static int create(const char *path, mode_t mode, struct fuse_file_info *fi){

  Create create = CREATE__INIT;
  create.path = strdup(path);
  create.mode = mode;
  CreateResp is_done = CREATE_RESP__INIT; 

  fsservice__create_file(rpc_service, &create, handle_create_response, &is_done);

  while (!is_done.is_done)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  if(is_done.result > 0){
    fi->fh = is_done.result;
  }

  return is_done.result > 0 ? 0 : is_done.result;

}

static int ex_open(const char *path, struct fuse_file_info *fi) {
  if (strcmp(path, "/") != 0)
    return -ENOENT;

  if ((fi->flags & 3) != O_RDONLY)
    return -EACCES;

  return 0;
}

struct fuse_operations ops = {
  .readdir = readdir,
  .getattr = getattr,
  .open = ex_open,
  .create = create
};
