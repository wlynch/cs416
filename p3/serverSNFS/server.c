#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include "server.h"

#include "../protobuf-model/ping.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>

#define DPRINT(str) { fprintf(stderr, "%s\n", str); fflush(stderr); }

void print_usage() {
  printf("Error, you must call this program in the format ./serverSNFS " \
   "-port [port number] -mount [mount location]\n");
}

static protobuf_c_boolean starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

void ping__reply_to_ping(PingService_Service * service,
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
  closure(ping_response_buf, closure_data);
}

static PingService_Service ping_service =
PING_SERVICE__INIT(ping__);

int main(int argc, char **argv) {
  ProtobufC_RPC_Server * server;
  const char *name = NULL;
  unsigned i;

  for (i = 1; i < (unsigned) argc; i++) {
    if (starts_with(argv[i], "--port=")) {
      name = strchr (argv[i], '=') + 1;
    }
  }

  server = protobuf_c_rpc_server_new (PROTOBUF_C_RPC_ADDRESS_TCP, name, (ProtobufCService *) &ping_service, NULL);

  if (server == NULL) {
    perror("server: ");
    return 0;
  }

  for (;;)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  protobuf_c_rpc_server_destroy(server, 1);
  return 0;
}
