#pragma once

#include "../protobuf-model/ping.pb-c.h"

static void handle_ping_response (const PingResponse *result,
                                   void *closure_data);
int start_fuse (int argc, char ** argv);
bool parse_input(char **, char **);
