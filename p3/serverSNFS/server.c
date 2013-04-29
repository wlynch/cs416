#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <errno.h>

#include "../protobuf-model/fs.pb-c.h"
#include "filesystem.h"
#include <google/protobuf-c/protobuf-c-rpc.h>

static int starts_with (const char *str, const char *prefix) {
  return memcmp (str, prefix, strlen (prefix)) == 0;
}

// this will handle all rpc calls using the reply_to_ping service
void fs__reply_to_ping(FSService_Service * service,
 const Ping * input,
 Ping_Closure closure,
 void * closure_data) {
  printf("input->message: %s\n", input->message);

  // init the message
  Ping ping_response = PING__INIT;
  ping_response.message = strdup("hi");

  // respond with the ping_response buffer
  closure(&ping_response, closure_data);
}

void fs__create_file(FSService_Service * service,
  const Create * input,
  const CreateResp_Closure closure,
  void * closure_data){
  int create_res;
  printf("incoming path is %s and mode is %d\n", input->path, input->mode);

  CreateResp create_handle = CREATE_RESP__INIT;
  char * full_path = get_full_path(input->path);
  create_res = creat(full_path, input->mode);

  if(create_res < 0){
    create_res = -errno; 
  }
  
  printf("create_res has a value of %d\n", create_res);
  free(full_path);
  create_handle.result = create_res;

  closure(&create_handle, closure_data);
}

static FSService_Service fs_service = FSSERVICE__INIT(fs__);

int main(int argc, char **argv) {
  ProtobufC_RPC_Server * server;
  const char * port = NULL, * mount = NULL;
  unsigned i;
  bool set_root;

  if(argc != 5){
    fprintf(stderr, "Error, please supply a -mount and -port option\n");
    return 1;
  }

  for (i = 1; i < (unsigned) argc; i += 2) {
    if (strcmp(argv[i], "-port") == 0) {
      port = argv[i + 1];
    }
    else if(strcmp (argv[i], "-mount") == 0){
      mount = argv[i + 1];        
    }
    else{
      fprintf(stderr, "Error, %s is an invalid argument\n", argv[i]);
      return 1;
    }
  }

  set_root = set_root_path(mount);
  if(!set_root){
    fprintf(stderr, "Error, couldn't set the root path to %s "\
        "please enter a path that both exists and is a directory\n", mount);
    return 1;
  }

  server = protobuf_c_rpc_server_new (PROTOBUF_C_RPC_ADDRESS_TCP, port, (ProtobufCService *) &fs_service, NULL);

  for (;;)
    protobuf_c_dispatch_run (protobuf_c_dispatch_default ());

  protobuf_c_rpc_server_destroy(server, 1);
  return 0;
}
