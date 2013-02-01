#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include "wtc_thr.h"

int *transitive_closure;
size_t size_of_graph;
size_t number_of_vertices;

int get_array_loc(int, int);
void print_array();
/*
 * Create any global structures which are necessary
 *
 */
void wtc_thr_init(int * T, size_t num_vertices){
	size_of_graph = num_vertices * num_vertices * sizeof(int);
	number_of_vertices = num_vertices;
	transitive_closure = (int *)malloc(size_of_graph);
	memcpy(transitive_closure, T, size_of_graph);

}

/*
 * Actually run warshal's algorithm
 */
int *wtc_thr(){
	int k, i, j, index;


	//hold each vertex steady
	for( k = 0; k < number_of_vertices; k++ ) {
		for( i = 0; i < number_of_vertices; i++ ) {
			if(transitive_closure[get_array_loc(i, k)]){
				for( j = 0; j < number_of_vertices; j++ ) {
					index = get_array_loc(i, j);
					transitive_closure[index] = transitive_closure[index] | transitive_closure[get_array_loc(k, j)]; 
					
				}
			}
		}
	}
	return transitive_closure;
}

/*
 * Finds the corresponding 2D array location in our 1D array
 */
int get_array_loc(int x, int y){
	return ((x * number_of_vertices)) + y;
}

void print_array()
{
		int i;
		for(i = 0; i < number_of_vertices * number_of_vertices; i++)
		{
				printf("%d ", transitive_closure[i]);
		}
		printf("\n");
}

void wtc_thr_destroy(){
	free(transitive_closure);
}
