#pragma once

#include "../protobuf-model/ping.pb-c.h"
#include <google/protobuf-c/protobuf-c-rpc.h>

/* the client that will communicate with the remote server */
extern ProtobufC_RPC_Client * rpc_client;

extern void handle_ping_response (const Ping *result, void *closure_data);
