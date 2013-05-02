#pragma once

#include <sys/types.h>

#include <google/protobuf-c/protobuf-c-rpc.h>
#include "../protobuf-model/fs.pb-c.h"


void parse_get_attr(GetAttrResponse *, struct stat *);
