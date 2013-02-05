#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include <pthread.h>
#include <semaphore.h>

#include "wtc_thr.h"

int *odd_closure;
int *even_closure;
int **both_closures;
size_t size_of_graph;
size_t number_of_vertices;

bool still_running;

int get_array_loc(int, int);
void print_array();

/*
 * Create any global structures which are necessary
 *
 */
void wtc_thr_init(int * T, size_t num_vertices){
	size_of_graph = num_vertices * num_vertices * sizeof(int);
	number_of_vertices = num_vertices;

    //allocate both transitive closure objects
	odd_closure = (int *)malloc(size_of_graph);
	even_closure = (int *)malloc(size_of_graph);
	
    //copy the current graph into both even and odd closures
    memcpy(odd_closure, T, size_of_graph);
	memcpy(even_closure, T, size_of_graph);
   
    //initialize both transitive closures
    both_closures = (int**)malloc(sizeof(int*) * 2);
    both_closures[0] = even_closure;
    both_closures[1] = odd_closure;

}

/*
 * Actually run warshal's algorithm
 */
int *wtc_thr(){

    int even_closure_loc, odd_closure_loc;
	int k, i, j, index;

	//hold each vertex steady
	for( k = 0; k < number_of_vertices; k++ ) 
    {
        //figure out which closure we are reading and which we are writing
        int *closure_writing = k % 2 == 0 ? even_closure : odd_closure;
        int *closure_reading = k % 2 == 1 ? even_closure : odd_closure;

		for( i = 0; i < number_of_vertices; i++ ) 
        {
            for( j = 0; j < number_of_vertices; j++ ) 
            {
                index = get_array_loc(i, j);
                closure_writing[index] = closure_reading[index] | (closure_reading[get_array_loc(i, k)] & closure_reading[get_array_loc(k, j)]); 
                
            }
        }
   	}
   
    //return the most recently written array
    return k % 2 == 0 ? odd_closure : even_closure;
}

/*
 * Finds the corresponding 2D array location in our 1D array
 */
int get_array_loc(int x, int y){
	return ((x * number_of_vertices)) + y;
}

/**
 * Does all operations related to creating a thread for warshal's algorithm
 * 
 * Thread logic is:
 *  Given a high and low index, check all i values in that range
 *  Sleep
 *
 * Threads should join with the main process to let them know when to terminate.
 */
void create_thread(int low_index, int high_index)
{
  
}

void thread_function(int low, int high)
{
  while(still_running == true)
  {
    //get the current value of k
    //figure out which closure is being read and which is being written
    //do the j logic in there
  }
}

/**
 * Frees all memory allocated by this process
 */
void wtc_thr_destroy(){
	free(even_closure);
    free(odd_closure);
    free(both_closures);
}

/**
 * Helper method to print out a given 1D array that represents the graph
 */
void print_array(int *array)
{
		int i;
		for(i = 0; i < number_of_vertices * number_of_vertices; i++)
		{
		    printf("%d ", array[i]);
		}
		printf("\n");
}



