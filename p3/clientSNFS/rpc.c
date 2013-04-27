#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rpc.h"

extern void handle_ping_response (const Ping *result,
 void *closure_data) {
  printf("ping reply: %s\n", result->message);
  *(protobuf_c_boolean *) closure_data = 1;
}
