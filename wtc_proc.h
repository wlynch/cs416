#ifndef _WTC_PROC_H
#define _WTC_PROC_H

void wtc_proc_init(int * initial_matrix, int number_of_vertices, int number_of_processes);
int * wtc_proc(int n, int number_of_processes);
void wtc_proc_create(int process_number, int number_of_processes, int n);
void wtc_proc_cleanup();

#endif

