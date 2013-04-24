#define FUSE_USE_VERSION 26

#include <fuse.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "client.h"

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/ping.pb-c.h"

#define DPRINT(str) { fprintf(stderr, "%s\n", str); fflush(stderr); }

/* globals are bad, stop this */
int port_number;
char * address, * mount_path;

/* super bad, no no no */
extern struct fuse_operations ops;

static int starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

int main (int argc, char ** argv) {
  Ping ping = PING__INIT;
  void * ping_buf;
  unsigned ping_buf_length;

  ProtobufCService *service;
  ProtobufC_RPC_Client * client;
  const char *name = NULL;
  unsigned i;

  for (i = 1; i < (unsigned) argc; i++)
    {
      if (starts_with (argv[i], "--tcp="))
        {
          name = strchr (argv[i], '=') + 1;
        }
    }

  if (name == NULL) {
    fprintf(stderr, "missing --tcp=HOST:PORT");
    return 1;
  }

  service = protobuf_c_rpc_client_new (PROTOBUF_C_RPC_ADDRESS_TCP, name, &ping_service__descriptor, NULL);
  if (service == NULL) {
    perror("service: ");
    return 1;
  }

  client = (ProtobufC_RPC_Client *) service;

  fprintf (stderr, "Connecting... ");
  while (!protobuf_c_rpc_client_is_connected (client))
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());
  fprintf (stderr, "done.\n");

  // create the message
  ping.origin = strdup("localhost:8080");

  // get it's length and create the buffer it will go into
  ping_buf_length = ping__get_packed_size(&ping);
  ping_buf = malloc(ping_buf_length);

  // pack the message into the buffer
  ping__pack(&ping, ping_buf);

  // send it on it's way
  //for (;;) {
    protobuf_c_boolean is_done = 0;

    DPRINT("sending ping...");

    // actually make the rpc call by sending a Ping,
    // handle_ping_response will be called with the resulting PingResponse
    ping_service__reply_to_ping(service, &ping, handle_ping_response, &is_done);

    while (!is_done)
        protobuf_c_dispatch_run (protobuf_c_dispatch_default ());
  //}


  free(ping_buf);
  //return start_fuse(argc, argv);
}

static void handle_ping_response (const PingResponse *result,
                                   void *closure_data) {

  DPRINT("got a response");
  printf("ping reply: %s\n", result->reply);

  *(protobuf_c_boolean *) closure_data = 1;
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
