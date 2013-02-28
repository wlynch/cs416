#pragma once

void wtc_proc_bt_init(int * initial_matrix, int number_of_vertices, int number_of_processes);
int * wtc_proc_bt(int n, int number_of_processes);
void wtc_proc_bt_create(int process_number, int number_of_processes, int n);
void wtc_proc_bt_cleanup();

