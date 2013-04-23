#define FUSE_USE_VERSION 26

#include <fuse.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

#include "../protobuf-model/ping.pb-c.h"

/* globals are bad, stop this */
int port_number;
char * address, * mount_path;

/* super bad, no no no */
extern struct fuse_operations ops;

int main (int argc, char ** argv) {
  Ping ping = PING__INIT;
  void * ping_buf;
  unsigned ping_buf_length;

  // create the message
  ping.origin = strdup("localhost:8080");

  // get it's length and create the buffer it will go into
  ping_buf_length = ping__get_packed_size(&ping);
  ping_buf = malloc(ping_buf_length);

  // pack the message into the buffer
  ping__pack(&ping, ping_buf);

  // send it on it's way
  fwrite(ping_buf, ping_buf_length, 1, stdout);

  free(ping_buf);
  //return start_fuse(argc, argv);
}

int start_fuse (int argc, char ** argv) {
  char * fuse_args[6];
  bool parsed;

  if (argc != 7) {
    printf("Error, your input must contain input arguments" \
     "please supply a -port, -address, and -mount option\n");
    return 1;
  }

  parsed = parse_input(argv, fuse_args);

  if (!parsed) {
    printf("Error, you must supply a -mount, -address, and -port flag\n");
    return 1;
  }

  fuse_args[2] = "-o";
    #ifdef __APPLE__
  fuse_args[3] = "allow_other";
    #else
  fuse_args[3] = "user_allow_other";
    #endif
  fuse_args[4] = "-o";
  fuse_args[5] = "allow_root";

  return fuse_main(4, fuse_args, &ops, NULL);
}

bool parse_input (char **argv, char **fuse_args) {
  int i;

  for (i = 1; i < 7; i += 2) {
    if (strcmp(argv[i], "-mount") == 0) {
      fuse_args[0] = "-mount";
      fuse_args[1] = argv[i + 1];
      mount_path = argv[i + 1];
    } else if(strcmp(argv[i], "-port") == 0) {
      port_number = strtol(argv[i + 1], (char **)NULL, 10);
    } else if(strcmp(argv[i], "-address") == 0) {
      address = argv[i + 1];
    } else {
      return false;
    }
  }

  return true;
}
