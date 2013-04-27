#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <fuse.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/ping.pb-c.h"

#include "rpc.h"

extern struct fuse_operations ops;
extern ProtobufC_RPC_Client * rpc_client;

static int starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

int main (int argc, char ** argv) {
  ProtobufCService *service;
  const char * name = NULL, * mount = NULL;
  char * fuse_args[] = {"-mount", 0, "-o", "user_allow_other"};
  int fuse_argc = 4;
  unsigned i;

  // the ping message we will be sending
  Ping ping = PING__INIT;

  #ifdef __APPLE__
    fuse_args[3] = "allow_other";
  #endif

  for (i = 1; i < (unsigned) argc; i++) {
    if (starts_with (argv[i], "--tcp=")) {
      name = strchr(argv[i], '=') + 1;
    } else if (starts_with (argv[i], "--mount=")) {
      mount = strchr(argv[i], '=') + 1;
      fuse_args[1] = mount;
    }
  }

  if (name == NULL) {
    fprintf(stderr, "missing --tcp=HOST:PORT\n");
    return 1;
  } else if (mount == NULL) {
    fprintf(stderr, "missing --mount=DIR\n");
    return 1;
  }

  // service creates an rpc client, and client is a special cast
  service = protobuf_c_rpc_client_new (PROTOBUF_C_RPC_ADDRESS_TCP, name, &ping_service__descriptor, NULL);
  rpc_client = (ProtobufC_RPC_Client *) service;

  fprintf (stderr, "Connecting... ");
  while (!protobuf_c_rpc_client_is_connected (rpc_client))
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());
  fprintf (stderr, "done.\n");

  fuse_main(fuse_argc, fuse_args, &ops, NULL);

  // create the message
  ping.message = strdup("HELLO WORLD");

  // send it on it's way
  protobuf_c_boolean is_done = 0;
  printf("sending ping... ");
  // handle_ping_response will be called with the resulting PingResponse
  ping_service__reply_to_ping(service, &ping, handle_ping_response, &is_done);

  while (!is_done)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  return 0;
}
