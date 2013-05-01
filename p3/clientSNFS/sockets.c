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

int port;
struct sockaddr_in serv_addr;

int get_socket(){
  return socket(AF_INET, SOCK_STREAM, 0);
}

int set_port(int new_port){
  port = new_port;
}

void set_host(char *host_name){
  bzero((char *) &serv_addr, sizeof(serv_addr));
  struct hostent *host = gethostbyname(host_name);
  serv_addr.sin_family = AF_INET;
  memcpy((char *)&serv_addr.sin_addr.s_addr,
      (char *)host->h_addr,
      host->h_length);
  serv_addr.sin_port = htons(port);
}

struct sockaddr_in *get_host(){
  return &serv_addr;
}
