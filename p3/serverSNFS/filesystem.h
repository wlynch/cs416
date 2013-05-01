#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

char * get_full_path(char *);
char * get_root_path();
bool set_root_path(const char *);

