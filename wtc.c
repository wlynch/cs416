#include <stdlib.h>
#include <stdio.h>

#include "wtc_proc_bt.h"

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
  int * initial_matrix, * transitive_closure;

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
  initial_matrix = calloc(number_of_vertices * number_of_vertices, sizeof(int));

  while ( fscanf(input_fd, "%i %i", &i, &j) == 2 ) {
      printf("(%i, %i)\n", i, j);
      initial_matrix[(i-1) + (j-1)*number_of_vertices] = 1;
  }

  /* print the graph */
  print_adjacency_matrix(initial_matrix, number_of_vertices);

  /* determine which method to use to solve the transitive graph */
  switch (argv[1][0] - '0') {
    case 1:
      break;
    case 2:
      break;
    case 3:
      wtc_proc_bt_init(initial_matrix, number_of_vertices, number_of_processes);
      puts("");
      transitive_closure = wtc_proc_bt(number_of_vertices, number_of_processes);
      print_adjacency_matrix(transitive_closure, number_of_vertices);
      wtc_proc_bt_cleanup();
      break;
    case 4:
      break;
    default:
      fprintf(stderr, "invalid method %s\n", argv[1]);
      break;
  }

  /* clean up */
  free(initial_matrix);

  /* close the input file */
  fclose(input_fd);

  return 0;
}
