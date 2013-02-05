#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include "wtc_thr.h"

int *odd_closure;
int *even_closure;
int **both_closures;
size_t size_of_graph;
size_t number_of_vertices;

int get_array_loc(int, int);
void print_array();

sem_t finish;

/*
 * Create any global structures which are necessary
 *
 */
void wtc_thr_init(int * T, size_t num_vertices){
    size_of_graph = num_vertices * num_vertices * sizeof(int);
    number_of_vertices = num_vertices;

    /* allocate both transitive closure objects */
    odd_closure = (int *)malloc(size_of_graph);
    even_closure = (int *)malloc(size_of_graph);

    /* copy the current graph into both even and odd closures */
    memcpy(odd_closure, T, size_of_graph);
    memcpy(even_closure, T, size_of_graph);

    /* initialize both transitive closures */
    both_closures = (int**)malloc(sizeof(int*) * 2);
    both_closures[0] = even_closure;
    both_closures[1] = odd_closure;
    
    pthread_mutex_init(&even_lock,NULL);
    pthread_mutex_init(&odd_lock,NULL);
}

/*
 * Actually run warshal's algorithm
 */
int *wtc_thr(){

    int k, i;
    /* Struct initialization. This is what we will pass in to the threads */
    wtc_thr_args *args=(wtc_thr_args *)malloc(sizeof(wtc_thr_args));
    args->nov=number_of_vertices;
    pthread_mutex_init(&(args->lock),NULL);
    
    sem_init(&finish,0,0); 

    /* hold each vertex steady */
    for( k = 0; k < number_of_vertices; k++ ) 
    {
        pthread_t t;
        /* Lock so we don't try to change something that hasn't been read yet. */
        pthread_mutex_lock(&(args->lock));
        args->k=k;
        for (i=0; i < number_of_vertices; i++){
            
            /* Lock only if not the first iteration, i.e. if setting k did not already lock */
            if (i != 0) {    
                pthread_mutex_lock(&(args->lock));
            }
            args->i=i;


            /* Make a thread and detach. We will wait for it to finish later */
            pthread_create(&t,NULL,wtc_thr_thread,(void *)args);
            pthread_detach(t);
        }
        /* Wait for all threads to finish before returning */
        for (i=0; i < number_of_vertices; i++){
            sem_wait(&finish);
        }
    }
    
    free(args);

    /* return the most recently written array */
    return k % 2 == 0 ? odd_closure : even_closure;
}

/* Thread to run Warshalls algo for the given row */
void *wtc_thr_thread(void *args){

    /* Some abstraction just to make things easier to read below */
    int i=((wtc_thr_args *)args)->i;
    int k=((wtc_thr_args *)args)->k;
    /* Unlock so that k and i can be changed again for the next thread. */
    pthread_mutex_unlock(&(((wtc_thr_args *)args)->lock));
    
    int number_of_vertices=((wtc_thr_args *)args)->nov;

    /* figure out which closure we are reading and which we are writing */
    int *closure_writing = k % 2 == 0 ? even_closure : odd_closure;
    int *closure_reading = k % 2 == 1 ? even_closure : odd_closure;

    /* The main part of the thread. Does work for the given row */
    int j,index;
    for( j = 0; j < number_of_vertices; j++ ) 
    {
        index = get_array_loc(i, j);
        closure_writing[index] = closure_reading[index] | (closure_reading[get_array_loc(i, k)] & closure_reading[get_array_loc(k, j)]); 
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

void print_array(int *array)
{
    int i;
    for(i = 0; i < number_of_vertices * number_of_vertices; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

/**
 * Frees all memory allocated by this process
 */
void wtc_thr_destroy(){
    free(even_closure);
    free(odd_closure);
    free(both_closures);
}
