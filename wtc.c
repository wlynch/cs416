#include <stdlib.h>
#include <stdio.h>

#include "wtc_proc.h"

void print_usage() {
  fprintf(stderr, "usage: wtc <method> <input file>\n");
  exit(0);
}

void print_adjacency_matrix(int * T, int number_of_vertices) {
  int i, j;
  for (i = 0; i < number_of_vertices; i++) {
    for (j = 0; j < number_of_vertices; j++) {
      printf("%i ", T[i + j * number_of_vertices]);
    }
    printf("\n");
  }
}

int main(int argc, char ** argv) {
  int number_of_processes, number_of_vertices;
  int i, j;
  FILE * input_fd;

  /* the vertices graph, length number_of_vertices * number_of_vertices
   * to find a connection you look at E[x + y * number_of_vertices] */
  int * T;

  if (argc != 3) {
    print_usage();
  }

  /* load the input file */
  input_fd = fopen(argv[2], "r");
  if (!input_fd) {
    fprintf(stderr, "cannot open input file %s\n", argv[1]);
    exit(1);
  }

  fscanf(input_fd, "%i", &number_of_processes);
  fscanf(input_fd, "%i", &number_of_vertices); /* n, for n by n matrix */
  printf("%i processes\n%i vertices\n", number_of_processes, number_of_vertices);

  /* initialize E, the vertex graph*/
  T = calloc(number_of_vertices * number_of_vertices, sizeof(int));

  while ( fscanf(input_fd, "%i %i", &i, &j) == 2 ) {
      printf("(%i, %i)\n", i, j);
      T[(i-1) + (j-1)*number_of_vertices] = 1;
  }

  /* print the graph */
  print_adjacency_matrix(T, number_of_vertices);

  /* determine which method to use to solve the transitive graph */
  switch (argv[1][0] - '0') {
    case 1:
      wtc_proc_init(T, sizeof(int) * number_of_vertices);
      wtc_proc();
      wtc_proc_cleanup();
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    default:
      fprintf(stderr, "invalid method %s\n", argv[1]);
      break;
  }

  /* clean up */
  free(T);

  /* close the input file */
  fclose(input_fd);

  return 0;
}
