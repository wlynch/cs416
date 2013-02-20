#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "wtc_proc.h"
#include "wtc_btproc.h"
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
    int * initial_matrix, * transitive_closure;

    struct timeval start_time;
    struct timeval end_time;
    struct timeval timeTaken;
    unsigned long int msec;
    int method = argv[1][0] - '0';

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

    /* initialize E, the vertex graph*/
    initial_matrix = calloc(number_of_vertices * number_of_vertices, sizeof(int));

    while ( fscanf(input_fd, "%i %i", &i, &j) == 2 ) {
        initial_matrix[(i-1) + (j-1)*number_of_vertices] = 1;
    }

    /* determine which method to use to solve the transitive graph */
    switch (method) {
        case 1:
            wtc_proc_init(initial_matrix, number_of_vertices, number_of_processes);

            gettimeofday(&start_time, NULL);
            transitive_closure = wtc_proc(number_of_vertices, number_of_processes);
            gettimeofday(&end_time, NULL);

            print_adjacency_matrix(transitive_closure, number_of_vertices);
            wtc_proc_cleanup();
            break;
        case 2:
            wtc_thr_init(initial_matrix, number_of_vertices, number_of_processes);

            gettimeofday(&start_time, NULL);
            transitive_closure = wtc_thr();
            gettimeofday(&end_time,NULL);

            print_adjacency_matrix(transitive_closure, number_of_vertices);
            wtc_thr_destroy();
            break;
        case 3:
            wtc_proc_bt_init(initial_matrix, number_of_vertices, number_of_processes);

            gettimeofday(&start_time, NULL);
            transitive_closure = wtc_proc_bt(number_of_vertices, number_of_processes);
            gettimeofday(&end_time, NULL);

            print_adjacency_matrix(transitive_closure, number_of_vertices);
            wtc_proc_bt_cleanup();
            break;
        case 4:
            wtc_thr_init(initial_matrix, number_of_vertices, number_of_processes);

            gettimeofday(&start_time, NULL);
            transitive_closure = wtc_btthr(&timeTaken);
            gettimeofday(&end_time, NULL);

            print_adjacency_matrix(transitive_closure, number_of_vertices);
            wtc_thr_destroy();
            break;
        default:
            fprintf(stderr, "invalid method %s\n", argv[1]);
            break;
    }

    if (method > 0 && method <=4) {
        msec=((end_time.tv_sec * 1000000 + end_time.tv_usec)
                - (start_time.tv_sec * 1000000 + start_time.tv_usec));
        printf("\nTime: %lu us\n", msec);
    }
    /* clean up */
    free(initial_matrix);

    /* close the input file */
    fclose(input_fd);

    return 0;
}
