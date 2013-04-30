#pragma once

#include "../protobuf-model/fs.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>

extern void handle_ping_response (const Ping *result, void *closure_data);
extern void handle_create_response(const FileResponse * result, void * data);
