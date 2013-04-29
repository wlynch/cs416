/* Generated by the protocol buffer compiler.  DO NOT EDIT! */

#ifndef PROTOBUF_C_fs_2eproto__INCLUDED
#define PROTOBUF_C_fs_2eproto__INCLUDED

#include <google/protobuf-c/protobuf-c.h>

PROTOBUF_C_BEGIN_DECLS

#include "ping.pb-c.h"

typedef struct _Create Create;
typedef struct _CreateResp CreateResp;


/* --- enums --- */


/* --- messages --- */

struct  _Create
{
  ProtobufCMessage base;
  char *path;
  int32_t mode;
};
#define CREATE__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&create__descriptor) \
    , NULL, 0 }


struct  _CreateResp
{
  ProtobufCMessage base;
  int32_t result;
  protobuf_c_boolean has_is_done;
  protobuf_c_boolean is_done;
};
#define CREATE_RESP__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&create_resp__descriptor) \
    , 0, 0,0 }


/* Create methods */
void   create__init
                     (Create         *message);
size_t create__get_packed_size
                     (const Create   *message);
size_t create__pack
                     (const Create   *message,
                      uint8_t             *out);
size_t create__pack_to_buffer
                     (const Create   *message,
                      ProtobufCBuffer     *buffer);
Create *
       create__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   create__free_unpacked
                     (Create *message,
                      ProtobufCAllocator *allocator);
/* CreateResp methods */
void   create_resp__init
                     (CreateResp         *message);
size_t create_resp__get_packed_size
                     (const CreateResp   *message);
size_t create_resp__pack
                     (const CreateResp   *message,
                      uint8_t             *out);
size_t create_resp__pack_to_buffer
                     (const CreateResp   *message,
                      ProtobufCBuffer     *buffer);
CreateResp *
       create_resp__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   create_resp__free_unpacked
                     (CreateResp *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*Create_Closure)
                 (const Create *message,
                  void *closure_data);
typedef void (*CreateResp_Closure)
                 (const CreateResp *message,
                  void *closure_data);

/* --- services --- */

typedef struct _FSService_Service FSService_Service;
struct _FSService_Service
{
  ProtobufCService base;
  void (*reply_to_ping)(FSService_Service *service,
                        const Ping *input,
                        Ping_Closure closure,
                        void *closure_data);
  void (*create_file)(FSService_Service *service,
                      const Create *input,
                      CreateResp_Closure closure,
                      void *closure_data);
};
typedef void (*FSService_ServiceDestroy)(FSService_Service *);
void fsservice__init (FSService_Service *service,
                      FSService_ServiceDestroy destroy);
#define FSSERVICE__BASE_INIT \
    { &fsservice__descriptor, protobuf_c_service_invoke_internal, NULL }
#define FSSERVICE__INIT(function_prefix__) \
    { FSSERVICE__BASE_INIT,\
      function_prefix__ ## reply_to_ping,\
      function_prefix__ ## create_file  }
void fsservice__reply_to_ping(ProtobufCService *service,
                              const Ping *input,
                              Ping_Closure closure,
                              void *closure_data);
void fsservice__create_file(ProtobufCService *service,
                            const Create *input,
                            CreateResp_Closure closure,
                            void *closure_data);

/* --- descriptors --- */

extern const ProtobufCMessageDescriptor create__descriptor;
extern const ProtobufCMessageDescriptor create_resp__descriptor;
extern const ProtobufCServiceDescriptor fsservice__descriptor;

PROTOBUF_C_END_DECLS


#endif  /* PROTOBUF_fs_2eproto__INCLUDED */
