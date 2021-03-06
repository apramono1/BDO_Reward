/* Generated by the protocol buffer compiler.  DO NOT EDIT! */
/* Generated from: comms_message.proto */

#ifndef PROTOBUF_C_comms_5fmessage_2eproto__INCLUDED
#define PROTOBUF_C_comms_5fmessage_2eproto__INCLUDED

#include <protobuf-c/protobuf-c.h>

PROTOBUF_C__BEGIN_DECLS

#if PROTOBUF_C_VERSION_NUMBER < 1000000
# error This file was generated by a newer version of protoc-c which is incompatible with your libprotobuf-c headers. Please update your headers.
#elif 1001001 < PROTOBUF_C_MIN_COMPILER_VERSION
# error This file was generated by an older version of protoc-c which is incompatible with your libprotobuf-c headers. Please regenerate this file with a newer version of protoc-c.
#endif


typedef struct _CommsHostConnectionInfo CommsHostConnectionInfo;
typedef struct _CommsRequestMsg CommsRequestMsg;
typedef struct _CommsResponseMsg CommsResponseMsg;


/* --- enums --- */

typedef enum _CommsHostConnectionInfo__CommsType {
  COMMS_HOST_CONNECTION_INFO__COMMS_TYPE__COMMS_IP = 0,
  COMMS_HOST_CONNECTION_INFO__COMMS_TYPE__COMMS_DIALUP = 1
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMS_HOST_CONNECTION_INFO__COMMS_TYPE)
} CommsHostConnectionInfo__CommsType;
typedef enum _CommsHostConnectionInfo__LinkStatus {
  COMMS_HOST_CONNECTION_INFO__LINK_STATUS__LINK_CONNECTING = 0,
  COMMS_HOST_CONNECTION_INFO__LINK_STATUS__LINK_CONNECTED = 1,
  COMMS_HOST_CONNECTION_INFO__LINK_STATUS__LINK_DISCONNECTED = 2
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMS_HOST_CONNECTION_INFO__LINK_STATUS)
} CommsHostConnectionInfo__LinkStatus;
typedef enum _CommsRequestMsg__RequestType {
  COMMS_REQUEST_MSG__REQUEST_TYPE__PRECONNECT = 0,
  COMMS_REQUEST_MSG__REQUEST_TYPE__ISCONNECTED = 1,
  COMMS_REQUEST_MSG__REQUEST_TYPE__WAITCONNECTED = 2,
  COMMS_REQUEST_MSG__REQUEST_TYPE__SEND = 3,
  COMMS_REQUEST_MSG__REQUEST_TYPE__RECEIVE = 4,
  COMMS_REQUEST_MSG__REQUEST_TYPE__DISCONNECT = 5,
  COMMS_REQUEST_MSG__REQUEST_TYPE__FORCEFALLBACK = 7,
  COMMS_REQUEST_MSG__REQUEST_TYPE__SUSPEND_SERVER = 8,
  COMMS_REQUEST_MSG__REQUEST_TYPE__RESUME_SERVER = 9,
  COMMS_REQUEST_MSG__REQUEST_TYPE__GET_HOST_CONNECTION_INFO = 10
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMS_REQUEST_MSG__REQUEST_TYPE)
} CommsRequestMsg__RequestType;
typedef enum _CommsResponseMsg__Status {
  COMMS_RESPONSE_MSG__STATUS__OK = 0,
  COMMS_RESPONSE_MSG__STATUS__UNKNOWN_HOST = 1,
  COMMS_RESPONSE_MSG__STATUS__CONNECTED = 2,
  COMMS_RESPONSE_MSG__STATUS__CONNECTING = 3,
  COMMS_RESPONSE_MSG__STATUS__CONNECTION_FAILED = 4,
  COMMS_RESPONSE_MSG__STATUS__TIMEOUT = 5,
  COMMS_RESPONSE_MSG__STATUS__SEND_FAILED = 6,
  COMMS_RESPONSE_MSG__STATUS__CONNECTION_LOST = 7,
  COMMS_RESPONSE_MSG__STATUS__COMMS_APP_SUSPENDED = 8
    PROTOBUF_C__FORCE_ENUM_TO_BE_INT_SIZE(COMMS_RESPONSE_MSG__STATUS)
} CommsResponseMsg__Status;

/* --- messages --- */

struct  _CommsHostConnectionInfo
{
  ProtobufCMessage base;
  char *host_name;
  protobuf_c_boolean has_link_index;
  uint64_t link_index;
  protobuf_c_boolean has_comms_type;
  CommsHostConnectionInfo__CommsType comms_type;
  char *destination;
  protobuf_c_boolean has_link_status;
  CommsHostConnectionInfo__LinkStatus link_status;
};
#define COMMS_HOST_CONNECTION_INFO__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&comms_host_connection_info__descriptor) \
    , NULL, 0,0, 0,COMMS_HOST_CONNECTION_INFO__COMMS_TYPE__COMMS_IP, NULL, 0,COMMS_HOST_CONNECTION_INFO__LINK_STATUS__LINK_CONNECTING }


struct  _CommsRequestMsg
{
  ProtobufCMessage base;
  CommsRequestMsg__RequestType request_type;
  char *host;
  protobuf_c_boolean has_data;
  ProtobufCBinaryData data;
  protobuf_c_boolean has_timeout;
  uint32_t timeout;
  protobuf_c_boolean has_max_byte;
  uint32_t max_byte;
  protobuf_c_boolean has_duration;
  uint32_t duration;
};
#define COMMS_REQUEST_MSG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&comms_request_msg__descriptor) \
    , COMMS_REQUEST_MSG__REQUEST_TYPE__ISCONNECTED, NULL, 0,{0,NULL}, 0,0, 0,0, 0,0 }


struct  _CommsResponseMsg
{
  ProtobufCMessage base;
  CommsResponseMsg__Status status;
  protobuf_c_boolean has_data;
  ProtobufCBinaryData data;
  protobuf_c_boolean has_byte_sent;
  uint64_t byte_sent;
  char *partition_name;
  char *config_provider;
  CommsHostConnectionInfo *host_connection_info;
};
#define COMMS_RESPONSE_MSG__INIT \
 { PROTOBUF_C_MESSAGE_INIT (&comms_response_msg__descriptor) \
    , COMMS_RESPONSE_MSG__STATUS__OK, 0,{0,NULL}, 0,0, NULL, NULL, NULL }


/* CommsHostConnectionInfo methods */
void   comms_host_connection_info__init
                     (CommsHostConnectionInfo         *message);
size_t comms_host_connection_info__get_packed_size
                     (const CommsHostConnectionInfo   *message);
size_t comms_host_connection_info__pack
                     (const CommsHostConnectionInfo   *message,
                      uint8_t             *out);
size_t comms_host_connection_info__pack_to_buffer
                     (const CommsHostConnectionInfo   *message,
                      ProtobufCBuffer     *buffer);
CommsHostConnectionInfo *
       comms_host_connection_info__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   comms_host_connection_info__free_unpacked
                     (CommsHostConnectionInfo *message,
                      ProtobufCAllocator *allocator);
/* CommsRequestMsg methods */
void   comms_request_msg__init
                     (CommsRequestMsg         *message);
size_t comms_request_msg__get_packed_size
                     (const CommsRequestMsg   *message);
size_t comms_request_msg__pack
                     (const CommsRequestMsg   *message,
                      uint8_t             *out);
size_t comms_request_msg__pack_to_buffer
                     (const CommsRequestMsg   *message,
                      ProtobufCBuffer     *buffer);
CommsRequestMsg *
       comms_request_msg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   comms_request_msg__free_unpacked
                     (CommsRequestMsg *message,
                      ProtobufCAllocator *allocator);
/* CommsResponseMsg methods */
void   comms_response_msg__init
                     (CommsResponseMsg         *message);
size_t comms_response_msg__get_packed_size
                     (const CommsResponseMsg   *message);
size_t comms_response_msg__pack
                     (const CommsResponseMsg   *message,
                      uint8_t             *out);
size_t comms_response_msg__pack_to_buffer
                     (const CommsResponseMsg   *message,
                      ProtobufCBuffer     *buffer);
CommsResponseMsg *
       comms_response_msg__unpack
                     (ProtobufCAllocator  *allocator,
                      size_t               len,
                      const uint8_t       *data);
void   comms_response_msg__free_unpacked
                     (CommsResponseMsg *message,
                      ProtobufCAllocator *allocator);
/* --- per-message closures --- */

typedef void (*CommsHostConnectionInfo_Closure)
                 (const CommsHostConnectionInfo *message,
                  void *closure_data);
typedef void (*CommsRequestMsg_Closure)
                 (const CommsRequestMsg *message,
                  void *closure_data);
typedef void (*CommsResponseMsg_Closure)
                 (const CommsResponseMsg *message,
                  void *closure_data);

/* --- services --- */


/* --- descriptors --- */

extern const ProtobufCMessageDescriptor comms_host_connection_info__descriptor;
extern const ProtobufCEnumDescriptor    comms_host_connection_info__comms_type__descriptor;
extern const ProtobufCEnumDescriptor    comms_host_connection_info__link_status__descriptor;
extern const ProtobufCMessageDescriptor comms_request_msg__descriptor;
extern const ProtobufCEnumDescriptor    comms_request_msg__request_type__descriptor;
extern const ProtobufCMessageDescriptor comms_response_msg__descriptor;
extern const ProtobufCEnumDescriptor    comms_response_msg__status__descriptor;

PROTOBUF_C__END_DECLS


#endif  /* PROTOBUF_C_comms_5fmessage_2eproto__INCLUDED */
