#pragma once

struct server_param {
  int port;
  char * mount;
};

void print_usage();
int parse_server_params(int argc, char ** argv, struct server_param * param);
