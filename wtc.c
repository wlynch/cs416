#include <stdlib.h>
#include <stdio.h>

void print_usage() {
  fprintf(stderr, "usage: wtc <input file>\n");
  exit(0);
}

void print_adjacency_matrix(int * E, int number_of_vertices) {
  int i, j;
  for (i = 0; i < number_of_vertices; i++) {
    for (j = 0; j < number_of_vertices; j++) {
      printf("%i ", E[i + j * number_of_vertices]);
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
  int * E;

  if (argc != 2) {
    print_usage();
  }

  /* load the input file */
  input_fd = fopen(argv[1], "r");
  if (!input_fd) {
    fprintf(stderr, "cannot open input file %s\n", argv[1]);
    exit(1);
  }

  fscanf(input_fd, "%i", &number_of_processes);
  fscanf(input_fd, "%i", &number_of_vertices); /* n, for n by n matrix */
  printf("%i processes\n%i vertices\n", number_of_processes, number_of_vertices);

  /* initialize E, the vertex graph*/
  E = calloc(number_of_vertices * number_of_vertices, 0);

  while ( fscanf(input_fd, "%i %i", &i, &j) == 2 ) {
      printf("(%i, %i)\n", i, j);
      E[i-1 + (j-1)*number_of_vertices] = 1;
  }

  /* print the graph */
  print_adjacency_matrix(E, number_of_vertices);

  /* close the input file */
  fclose(input_fd);

  return 0;
}
