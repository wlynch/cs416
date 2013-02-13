#include <stdlib.h>
#include <stdio.h>

#include "wtc_thr.h"
#include "wtc_btthr.h"

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

	printf("Input graph is \n");
  /* print the graph */
  print_adjacency_matrix(T, number_of_vertices);

  /* determine which method to use to solve the transitive graph */
  switch (argv[1][0] - '0') {
    case 1:
     break;
    case 2:
     wtc_thr_init(T, number_of_vertices, number_of_processes);
     printf("Output of method two is \n");
     print_adjacency_matrix(wtc_thr(), number_of_vertices);
     wtc_thr_destroy();
     break;
    case 3:
     break;
    case 4:
     wtc_thr_init(T, number_of_vertices, number_of_processes);
     printf("Output of method four is \n");
     print_adjacency_matrix(wtc_btthr(), number_of_vertices);
     wtc_thr_destroy();
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
