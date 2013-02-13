#ifndef _WTC_BTTHR_H
#define _WTC_BTTHR_H

/* Struct to pass in all variables to a thread as single parameter */
typedef struct wtc_btthr_args{
    pthread_mutex_t lock;
    pthread_cond_t condition;
    pthread_mutex_t loop_lock;
    int *k;
} wtc_btthr_args;

void *wtc_btthr_thread(void *s);
int *wtc_btthr();
void enqueue_all();
int dequeue();

#endif
