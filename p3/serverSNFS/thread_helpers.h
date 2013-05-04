#pragma once

#include "../protobuf-model/fs.pb-c.h"

void create_file(Create *, FileResponse *);
void truncate_file(Truncate *, StatusResponse *);
void open_file(Open* input, FileResponse* resp);
void close_file(Close * input, ErrorResponse * resp);
int get_attr(Simple *, GetAttrResponse *);
void write_file(Write *, size_t count, StatusResponse *);
void *read_help(Read *, ReadResponse *);
void make_dir(Create *, ErrorResponse *);
