#pragma once

#include "../protobuf-model/ping.pb-c.h"

static void handle_ping_response (const Ping *result,
                                   void *closure_data);
