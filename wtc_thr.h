#ifndef _WTC_THR_H
#define _WTC_THR_H

/* Struct to pass in all variables to a thread as single parameter */
typedef struct wtc_thr_args{
    pthread_mutex_t lock;
    pthread_cond_t condition;
    pthread_mutex_t loop_lock;
    int nov;
    int *k;
    int mod;
} wtc_thr_args;

void *wtc_thr_thread(void *s);
void wtc_thr_init(int * T, size_t size, size_t num_threads);
int *wtc_thr();
void wtc_thr_destroy();

int get_array_loc(int x, int y);
void print_array(int *array);

#endif
