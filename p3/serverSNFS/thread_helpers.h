#pragma once

#include "../protobuf-model/fs.pb-c.h"

void create_file(Create *, FileResponse *);
void truncate_file(Truncate *, FileResponse *);
int get_attr(Simple *, GetAttrResponse *);
