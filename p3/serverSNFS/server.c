#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "server.h"
#include "../protobuf-model/ping.pb-c.h"

#include <google/protobuf-c/protobuf-c-rpc.h>

void print_usage() {
  printf("Error, you must call this program in the format ./serverSNFS " \
   "-port [port number] -mount [mount location]\n");
}

void ping__reply_to_ping(ProtobufCService * service,
                                 const Ping * input,
                                 PingResponse_Closure closure,
                                 void * closure_data) {
  // init the message
  PingResponse ping_response = PING_RESPONSE__INIT;
  void * ping_response_buf;
  unsigned ping_response_buf_length;

  // set the reply string
  ping_response.reply = strdup("hi");

  // get the lenght of the response and malloc some space
  ping_response_buf_length = ping_response__get_packed_size(&ping_response);
  ping_response_buf = malloc(ping_response_buf_length);

  // pack it into the buffer
  ping_response__pack(&ping_response, ping_response_buf);

  // respond with the ping_response buffer
  closure(&ping_response_buf, closure_data);
}

static PingService_Service ping_service = PING_SERVICE__INIT(ping__);

int main(int argc, char **argv) {
  ProtobufC_RPC_Server * server;
  const char * name = malloc(6);

  struct server_param * params = setup(argc, argv);
  if (!params) {
    fprintf(stderr, "could not parse params\n");
  } else {
    sprintf(name, "%d", params->port);
  }

  printf("Starting server, mount: %s, port: %i\n", params->mount, params->port);

  server = protobuf_c_rpc_server_new (PROTOBUF_C_RPC_ADDRESS_TCP, name, (ProtobufCService *) &(ping_service), NULL);

  if (!server) {
    perror("server: ");
    return 1;
  }

  for (;;)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  perror("dispatch: ");

  return 0;
}

struct server_param * setup(int argc, char ** argv) {
  int parsed_error;
  struct server_param * params = malloc(sizeof(struct server_param));

  if(argc != 5) {
    print_usage();
    free(params);
    return 0;
  }

  parsed_error = parse_server_params(argc, argv, params);

  if(parsed_error) {
    printf("Error, unable to parse your input arguments, " \
           "please only use -mount and -port arguments\n");
    return 0;
  }

  return params;
}

int parse_server_params(int argc, char ** argv, struct server_param * param) {
  int i;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-port") == 0) {
      param->port = atoi(argv[++i]);
      printf("port: %i\n", param->port);
    } else if (strcmp(argv[i], "-mount") == 0) {
      param->mount = strdup(argv[++i]);
      printf("mount: %s\n", param->mount);
    } else {
      return 1;
    }
  }

  return 0;
}
