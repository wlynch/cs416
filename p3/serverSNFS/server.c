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

int main(int argc, char **argv) {
  int parsed_error;
  struct server_param * params = malloc(sizeof(struct server_param));

  if(argc != 5) {
    print_usage();
    return 1;
  }

  parsed_error = parse_server_params(argc, argv, params);

  if(parsed_error) {
    printf("Error, unable to parse your input arguments, " \
      "please only use -mount and -port arguments\n");
    return 5;
  }

  printf("Starting server, mount: %s, port: %i\n", params->mount, params->port);

  return 0;
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
