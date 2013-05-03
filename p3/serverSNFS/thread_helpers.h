#pragma once

#include "../protobuf-model/fs.pb-c.h"

void create_file(Create *, FileResponse *);
void truncate_file(Truncate *, FileResponse *);
void open_file(Open* input, FileResponse* resp);
void close_file(Close * input, FileResponse * resp);
int get_attr(Simple *, GetAttrResponse *);
void *read_help(Read *, ReadResponse *);
