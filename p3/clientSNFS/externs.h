#pragma once

extern struct fuse_operations ops;

/* the client that will communicate with the remote server */
extern ProtobufC_RPC_Client * rpc_client;
extern ProtobufCService * rpc_service;
