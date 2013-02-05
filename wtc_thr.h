#ifndef _WTC_THR_H
#define _WTC_THR_H

/* Struct to pass in all variables to a thread as single parameter */
typedef struct wtc_thr_args{
    pthread_mutex_t lock;
    int nov;
    int k;
    int i;
} wtc_thr_args;

void *wtc_thr_thread(void *s);
void wtc_thr_init(int * T, size_t size);
int *wtc_thr();
void wtc_thr_destroy();

#endif
