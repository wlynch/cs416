#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "threading.h"
#include "filesystem.h"
#include <google/protobuf-c/protobuf-c-rpc.h>


int main(int argc, char **argv) {
  int sock, port, new_socket, bytes_read;
  const char * mount = NULL;
  struct sockaddr_in serv_addr, client_addr;
  socklen_t client_len;
  unsigned i;
  bool set_root;
  char buffer[256];

  if(argc != 5){
    fprintf(stderr, "Error, please supply a -mount and -port option\n");
    return 1;
  }

  for (i = 1; i < (unsigned) argc; i += 2) {
    if (strcmp(argv[i], "-port") == 0) {
      port = strtol(argv[i + 1], NULL, 10);
    }
    else if(strcmp (argv[i], "-mount") == 0){
      mount = argv[i + 1];        
    }
    else{
      fprintf(stderr, "Error, %s is an invalid argument\n", argv[i]);
      return 1;
    }
  }

  set_root = set_root_path(mount);
  if(!set_root){
    fprintf(stderr, "Error, couldn't set the root path to %s "\
        "please enter a path that both exists and is a directory\n", mount);
    return 1;
  }

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0)
  {
    fprintf(stderr, "Error, couldn't create a socket, something's eally wrong\n");
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  /* intialize the socket */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  
  if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    fprintf(stderr, "Error while trying to bind the socket\n");
    return 1;
  }

  listen(sock, 5);

  while(true)
  {
    new_socket = accept(sock, (struct sockaddr *) &client_addr, &client_len);
    bzero(buffer, 256);
    bytes_read = read(new_socket, buffer, 255);
    printf("message is %s\n", buffer);
    close(new_socket);
  }

  return 0;
}
