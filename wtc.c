#include <stdlib.h>
#include <stdio.h>

#include "parser.h"

void print_usage() {
  fprintf(stderr, "usage: wtc <input file>\n");
  exit(0);
}

int main(int argc, char ** argv) {
  int number_of_processes, number_of_vertices;
  FILE * input_fd;

  if (argc != 2) {
    print_usage();
  }

  /* load the input file */
  input_fd = fopen(argv[1], "r");
  if (!input_fd) {
    fprintf(stderr, "cannot open input file %s\n", argv[1]);
    exit(1);
  }

  number_of_processes = atoi(parser_next_word(input_fd));
  number_of_vertices  = atoi(parser_next_word(input_fd));
  printf("%i processes\n%i vertices\n", number_of_processes, number_of_vertices);

  /* close the input file */
  fclose(input_fd);

  return 0;
}
