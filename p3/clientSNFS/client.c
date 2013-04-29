#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <pthread.h>

#include <fuse.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/fs.pb-c.h"

#include "rpc.h"
#include "externs.h"

pthread_t fuse_thread;

struct fuse_params {
  int fuse_argc;
  char ** fuse_args;
};

void * handle_fuse(void * param) {
  struct fuse_params * params = (struct fuse_params *)param;
  printf("argc: %i, mount: %s\n", params->fuse_argc, params->fuse_args[1]);
  fflush(stdout);
  fuse_main(params->fuse_argc, params->fuse_args, &ops, NULL);
  return 0;
}

static int starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

int main (int argc, char ** argv) {
  char * fuse_args[] = {NULL, NULL};
  int fuse_argc = 2;
  char * name = NULL, * host = NULL, * port = NULL;
  unsigned i;

  if(argc != 7){
    fprintf(stderr, "Error, you must supply -port, -address, and -mount options\n");
    return 1;
  }

  fuse_args[0] = argv[0];

    // the ping message we will be sending
  Ping ping = PING__INIT;

  for (i = 1; i < (unsigned) argc; i += 2) {
    if (strcmp (argv[i], "-address") == 0) {
      host = argv[i + 1];
    } else if (strcmp (argv[i], "-mount") == 0) {
      fuse_args[1] = argv[i + 1]; /* mount path */
    }
    else if(strcmp (argv[i], "-port") == 0){
      port = argv[i + 1];
    }
    else{
      fprintf(stderr, "Error, invalid argument %s\n", argv[i]);
      return 1;
    }
  }

  name = (char *)malloc((strlen(host) + strlen(port) + 2));
  sprintf(name, "%s:%s", host, port);

  // service creates an rpc client, and client is a special cast
  rpc_service = protobuf_c_rpc_client_new (PROTOBUF_C_RPC_ADDRESS_TCP, name, &fsservice__descriptor, NULL);
  rpc_client = (ProtobufC_RPC_Client *) rpc_service;

  fprintf (stderr, "Connecting... ");
  while (!protobuf_c_rpc_client_is_connected (rpc_client))
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());
  fprintf (stderr, "done.\n");

  struct fuse_params * p = malloc(sizeof(struct fuse_params));
  p->fuse_argc = fuse_argc;
  p->fuse_args = fuse_args;

  pthread_create(&fuse_thread, NULL, handle_fuse, p);

  // create the message
  ping.message = strdup("HELLO WORLD");

  // send it on it's way
  protobuf_c_boolean is_done = 0;
  printf("sending ping... ");
  // handle_ping_response will be called with the resulting PingResponse
  fsservice__reply_to_ping(rpc_service, &ping, handle_ping_response, &is_done);

  while (!is_done)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  pthread_join(fuse_thread, NULL);

  return 0;
}
