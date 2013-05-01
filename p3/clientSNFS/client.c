#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>
#include <fuse.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/fs.pb-c.h"
#include "rpc.h"
#include "externs.h"
#include "sockets.h"

pthread_t fuse_thread;

struct fuse_params {
  int fuse_argc;
  char ** fuse_args;
};

void * handle_fuse(void * param) {
  struct fuse_params * params = (struct fuse_params *)param;
  printf("argc: %i, mount: %s\n", params->fuse_argc, params->fuse_args[1]);
  fflush(stdout);
  return 0;
}

static int starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

extern struct fuse_operations ops;

int main (int argc, char ** argv) {
  int port, sock;
  struct hostent *server;
  char * fuse_args[] = {NULL, NULL};
  int fuse_argc = 2;
  char * host = NULL;
  unsigned i;

  if(argc != 7){
    fprintf(stderr, "Error, you must supply -port, -address, and -mount options\n");
    return 1;
  }

  fuse_args[0] = argv[0];

 for (i = 1; i < (unsigned) argc; i += 2) {
    if (strcmp (argv[i], "-address") == 0) {
      host = argv[i + 1];
    } else if (strcmp (argv[i], "-mount") == 0) {
      fuse_args[1] = argv[i + 1]; /* mount path */
    }
    else if(strcmp (argv[i], "-port") == 0){
      port = strtol(argv[i + 1], NULL, 10); 
    }
    else{
      fprintf(stderr, "Error, invalid argument %s\n", argv[i]);
      return 1;
    }
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);
  server = gethostbyname(host);
  if(server == NULL)
  {
    fprintf(stderr, "Error, could not find a host with that name\n");
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
      (char *)&serv_addr.sin_addr.s_addr,
      server->h_length);
  serv_addr.sin_port = htons(port);
  connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); 
  write(sock, "hello world", strlen("hello world"));
  close(sock);

  struct fuse_params * p = malloc(sizeof(struct fuse_params));
  p->fuse_argc = fuse_argc;
  p->fuse_args = fuse_args;
  fuse_main(fuse_argc, fuse_args, &ops, NULL);
  /*pthread_create(&fuse_thread, NULL, handle_fuse, p);

  pthread_join(fuse_thread, NULL);*/

  return 0;
}
