#pragma once

#include <stdio.h>

FILE *log_open(void);
void log_msg(char *);
FILE *log_file;
