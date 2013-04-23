#pragma once

struct server_param {
  int port;
  char * mount;
};

void print_usage();
struct server_param * setup(int argc, char ** argv);
int parse_server_params(int argc, char ** argv, struct server_param * param);
