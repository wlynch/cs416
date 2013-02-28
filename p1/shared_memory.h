#pragma once

int allocate_shared_memory(int n);
void * map_shared_memory(int id);
void * share_memory(size_t num_bytes);
