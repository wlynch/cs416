#pragma once

#include "../protobuf-model/ping.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>

extern void handle_ping_response (const Ping *result, void *closure_data);
