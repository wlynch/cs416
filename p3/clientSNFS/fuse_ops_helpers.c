#include <sys/types.h>
#include <sys/stat.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/fs.pb-c.h"

void parse_get_attr(GetAttrResponse * resp, struct stat *stbuf){
  stbuf->st_dev = resp->st_dev;
  stbuf->st_ino = resp->st_ino;
  stbuf->st_mode = resp->st_mode;
  stbuf->st_nlink = resp->st_nlink;
  stbuf->st_uid = resp->st_uid;
  stbuf->st_gid = resp->st_gid;
  stbuf->st_rdev = resp->st_rdev;
  stbuf->st_atime = resp->atime;
  stbuf->st_mtime = resp->mtime;
  stbuf->st_ctime = resp->ctime;
  stbuf->st_blksize = resp->st_blksize;
  stbuf->st_blocks = resp->st_blocks;
  stbuf->st_size = resp->st_size;
  
}
