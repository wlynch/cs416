#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <stddef.h>
#include <math.h>
#include <stdbool.h>

#include <pthread.h>
#include <semaphore.h>

#include "wtc_btthr.h"
#include "wtc_thr.h"

/**
 * 
 * Representations and ways to get at the graph
 */
int *odd_closure;
int *even_closure;
int **both_closures;
int row;

/**
 * Graph information
 */
size_t size_of_graph;
size_t number_of_vertices;
size_t number_of_threads;

bool still_running;

int get_array_loc(int, int);
void print_array();

/**
 * Useful stuff for synchronization
 */
pthread_mutex_t row_lock;
sem_t finish;


/*
 * Actually run Bag of Tasks algorithm
 */
int *wtc_btthr()
{    
    int i, count, k;
    wtc_btthr_args *args;
    pthread_t t;
    still_running=true;
    row = 0;
    
    k = 0;

    /* Struct initialization. This is what we will pass in to the threads */
    args=(wtc_btthr_args *)malloc(sizeof(wtc_btthr_args));
    pthread_cond_init(&(args->condition), NULL);
    pthread_mutex_init(&(args->loop_lock), NULL);
    pthread_mutex_init(&(args->lock),NULL);
    pthread_mutex_init(&row_lock,NULL);

    sem_init(&finish,0,0);

    /* Create the pthreads */
    for(count = 0; count < number_of_threads; count++)
    {
        /* Make this operation atomic, that's not necessary on the first
         * iteration*/
        pthread_mutex_lock(&(args->lock));

        /* Pass in k by reference so we can use the proper closure*/
        args->k = &k;

        pthread_create(&t, NULL, wtc_btthr_thread, (void *)args);
        pthread_detach(t);
    }

    /* hold each vertex steady */
    while ( k < number_of_vertices) {
        /* Wait for all threads to finish before returning */
        for (i=0; i < number_of_threads; i++){
            sem_wait(&finish);
        }

        pthread_mutex_lock(&row_lock);
        enqueue_all();
        pthread_mutex_unlock(&row_lock);
    
        /*Increment k and wake up worker threads*/
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
    return k % 2 == 0 ? odd_closure : even_closure;
}

/* Thread to run Bag of Tasks algo for the given row */
void *wtc_btthr_thread(void *args){

    int i, *k, j, index;
    int *closure_reading, *closure_writing;

    wtc_btthr_args *data = (wtc_btthr_args *)args;

    /* Some abstraction just to make things easier to read below */
    k=data->k;

    /* Unlock so that k and i can be changed again for the next thread. */
    pthread_mutex_unlock(&(data->lock));

    while(still_running == true) {
        /* figure out which closure we are reading and which we are writing */
        closure_writing = *k % 2 == 0 ? even_closure : odd_closure;
        closure_reading = *k % 2 == 1 ? even_closure : odd_closure;

        /* The main part of the thread. Does work for the given row */
        pthread_mutex_lock(&row_lock);
        while (row < number_of_vertices) {
            i = dequeue();
            pthread_mutex_unlock(&row_lock);

            if ( i >= 0 ) {
                for( j = 0; j < number_of_vertices; j++ ) 
                {
                    index = get_array_loc(i, j);
                    closure_writing[index] = closure_reading[index] | (closure_reading[get_array_loc(i, *k)] & closure_reading[get_array_loc(*k, j)]);
                }
            }
            /* Lock before we check/dequeue the row */
            pthread_mutex_lock(&row_lock);
        }
        pthread_mutex_unlock(&row_lock);

        /* Post that we have finished work for this iteration and wait for the
         * next one
         */
        pthread_mutex_lock(&(data->lock));
        sem_post(&finish);
        pthread_cond_wait(&(data->condition), &(data->lock));
        pthread_mutex_unlock(&(data->lock));
    }

    sem_post(&finish);
    return NULL;
}

/* Enqueue all the rows to be operated on. 
 * This function seems unnecessary. It is. 
 * It's to enforce the point that this is how the queue is defined.*/
void enqueue_all() {
    row = 0;
}

/* Dequeue a single row to be operated on */
int dequeue() {
    int retval;
    /*If we still have more rows to give out, distribute it*/
    if (row < number_of_vertices) {
        retval=row;
        row++;
        
    } else {
       /*If we have no more items, just return negative one*/
        retval = -1;
    }
    return retval;
}
