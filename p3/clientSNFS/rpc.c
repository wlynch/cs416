#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpc.h"

ProtobufC_RPC_Client * rpc_client;
ProtobufCService * rpc_service;

extern void handle_ping_response (const Ping *result,
 void *closure_data) {
  printf("ping reply: %s\n", result->message);
  *(protobuf_c_boolean *) closure_data = 1;
}

extern void handle_create_response(const CreateResp *result,
  void * closure_data){
  printf("Got a response to my create message!\n");
  CreateResp * create_resp = (CreateResp *) closure_data;
  create_resp->is_done = 1;
  create_resp->result = result->result;
}
