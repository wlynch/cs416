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
pthread_mutex_t even_lock;
pthread_mutex_t odd_lock;

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

    int even_closure_loc, odd_closure_loc;
    int k, i, j, index;
    
    sem_init(&finish,0,0); 
    
    /* hold each vertex steady */
    for( k = 0; k < number_of_vertices; k++ ) 
    {
        /* figure out which closure we are reading and which we are writing */
        int *closure_writing = k % 2 == 0 ? even_closure : odd_closure;
        int *closure_reading = k % 2 == 1 ? even_closure : odd_closure;
        pthread_mutex_t *closure_lock = k % 2 == 0 ? &even_lock : &odd_lock;
        pthread_t t;
        
        for (i=0; i < number_of_vertices; i++){
            /* Make a struct with the information for the thread to use.
             * I realize this is not the most optimal way of doing this,
             * but this is more of a proof of concept/get it working step.
             */
            wtc_thr_args *s=(wtc_thr_args *)malloc(sizeof(wtc_thr_args));
            s->cw=closure_writing;
            s->cr=closure_reading;
            s->lock=closure_lock;
            s->k=k;
            s->nov=number_of_vertices;
            s->i=i;
            
            /* Make a thread and detach. We will wait for it to finish later */
            pthread_create(&t,NULL,wtc_thr_thread,(void *)s);
            pthread_detach(t);
        }
        /* Wait for all threads to finish before returning */
        for (i=0; i < number_of_vertices; i++){
            sem_wait(&finish);
        }
    }

    printf("Done waiting\n");

    /* return the most recently written array */
    return k % 2 == 0 ? odd_closure : even_closure;
}

/* Thread to run Warshalls algo for the given row */
void *wtc_thr_thread(void *s){

    /* Some abstraction just to make things easier to read below */
    int number_of_vertices=((wtc_thr_args *)s)->nov;
    int k=((wtc_thr_args *)s)->k;
    int *closure_writing=((wtc_thr_args *)s)->cw;
    int *closure_reading=((wtc_thr_args *)s)->cr;
    int i=((wtc_thr_args *)s)->i;
    pthread_mutex_t *lock = ((wtc_thr_args *)s)->lock;

    /* The main part of the thread. Does work for the given row */
    int j,index;
    for( j = 0; j < number_of_vertices; j++ ) 
    {
        index = get_array_loc(i, j);
        pthread_mutex_lock(lock);
        closure_writing[index] = closure_reading[index] | (closure_reading[get_array_loc(i, k)] & closure_reading[get_array_loc(k, j)]); 
        pthread_mutex_unlock(lock);
    }


    sem_post(&finish);
    free(s);
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
