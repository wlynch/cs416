#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#include <pthread.h>
#include <semaphore.h>

#include "wtc_thr.h"

int *even_closure;
int **both_closures;

size_t size_of_graph;
size_t number_of_vertices;
size_t number_of_threads;

bool still_running = true;

sem_t finish;

/*
 * Create any global structures which are necessary
 *
 */
void wtc_thr_init(int * T, size_t num_vertices, size_t num_threads){
    size_of_graph = num_vertices * num_vertices * sizeof(int);
    number_of_vertices = num_vertices;

    /* allocate both transitive closure objects */
    even_closure = (int *)malloc(size_of_graph);

    /* copy the current graph into both even and odd closures */
    memcpy(even_closure, T, size_of_graph);

    /* initialize both transitive closures */
    both_closures = (int**)malloc(sizeof(int*) * 2);
    both_closures[0] = even_closure;

    number_of_threads = num_threads;
}

/*
 * Actually run warshal's algorithm
 */
int *wtc_thr(){

    int k, i, count;
    wtc_thr_args *args;
    pthread_t t;

    k = 0;

    /* Struct initialization. This is what we will pass in to the threads */
    args=(wtc_thr_args *)malloc(sizeof(wtc_thr_args));
    pthread_cond_init(&(args->condition), NULL);
    pthread_mutex_init(&(args->loop_lock), NULL);
    pthread_mutex_init(&(args->lock),NULL);

    sem_init(&finish,0,0);

    /* Create the pthreads */
    for(count = 0; count < number_of_threads; count++)
    {
        /* Make this operation atomic, that's not necessary on the first
         * iteration*/
        pthread_mutex_lock(&(args->lock));

        /* Pass in k by reference so we can use the proper closure*/
        args->k = &k;
        args->mod = count;

        pthread_create(&t, NULL, wtc_thr_thread, (void *)args);
        pthread_detach(t);
    }

    /* hold each vertex steady */
    while ( k < number_of_vertices ) {
        /* Wait for all threads to finish before returning */
        for (i=0; i < number_of_threads; i++){
          sem_wait(&finish);
        }

        pthread_mutex_lock(&(args->lock));
        k++;
        pthread_cond_broadcast(&(args->condition));
        pthread_mutex_unlock(&(args->lock));
    }
    still_running = false;

    for (i=0; i < number_of_threads; i++){
      sem_wait(&finish);
    }

    pthread_cond_broadcast(&(args->condition));

    pthread_cond_destroy(&(args->condition));
    free(args);

    /* return the most recently written array */
    return even_closure;
}

/* Thread to run Warshalls algo for the given row */
void *wtc_thr_thread(void *args){

    int i, *k, j, index, mod;
    int *closure_reading, *closure_writing;

    wtc_thr_args *data = (wtc_thr_args *)args;

    /* Some abstraction just to make things easier to read below */
    k=data->k;
    mod = data->mod;

    /* Unlock so that k and i can be changed again for the next thread. */
    pthread_mutex_unlock(&(data->lock));

    while(still_running == true)
    {
        /* figure out which closure we are reading and which we are writing */
        closure_reading = even_closure;
        closure_writing = even_closure;

        /* The main part of the thread. Does work for the given row */
        i = mod;
        while(i < number_of_vertices){
            for( j = 0; j < number_of_vertices; j++ ) 
            {
                index = i*number_of_vertices + j;
                closure_writing[index] = closure_reading[index] | (closure_reading[i*number_of_vertices + *k] & closure_reading[*k * number_of_vertices + j]);
            }
            i+=number_of_threads;
        }

        pthread_mutex_lock(&(data->lock));
        sem_post(&finish);
        pthread_cond_wait(&(data->condition), &(data->lock));
        pthread_mutex_unlock(&(data->lock));
    }


    sem_post(&finish);
    return NULL;
}

/*
 * Finds the corresponding 2D array location in our 1D array
 */
int get_array_loc(int x, int y){
    return ((x * number_of_vertices)) + y;
}

/**
 * Frees all memory allocated by this process
 */
void wtc_thr_destroy(){
    free(even_closure);
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



