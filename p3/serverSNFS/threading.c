#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "threading.h"
#include "thread_helpers.h"
#include "../message_def.h"
#include "filesystem.h"
#include "../protobuf-model/fs.pb-c.h"

void * handle_request(void * args){

  int bytes_read;
  thread_args * thr_arg = (thread_args *)args;
  uint32_t message_type, message_size;
  void * message_buffer;

  /* never used */
  bytes_read = read(thr_arg->socket, &message_size, sizeof(message_size));

  // TODO: HANDLE ERRORS

  read(thr_arg->socket, &message_type, sizeof(message_type));
  message_size = ntohl(message_size);
  message_type = ntohl(message_type);
  message_buffer = malloc(message_size);
  read(thr_arg->socket, message_buffer, message_size);

  switch (message_type) {
    case CREATE_MESSAGE:
      {
        Create * create = create__unpack(NULL, message_size, message_buffer);
        FileResponse *resp = malloc(sizeof(FileResponse));
        create_file(create, resp);

        uint32_t send_size = file_response__get_packed_size(resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        // ignore the length when writing the length of the message
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(FILE_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        file_response__pack(resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        free(resp);
        free(send_buffer);
        create__free_unpacked(create, NULL);
        break;
      }
    case TRUNCATE_MESSAGE:
      {
        Truncate * input = truncate__unpack(NULL, message_size, message_buffer);
        StatusResponse *resp = malloc(sizeof(StatusResponse));
        truncate_file(input, resp);

        /*  Send to client code */
        uint32_t send_size = status_response__get_packed_size(resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        // ignore the length when writing the length of the message
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(FILE_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        status_response__pack(resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        free(resp);
        free(send_buffer);
        truncate__free_unpacked(input, NULL);
        break;
      }
    case OPEN_MESSAGE:
      {
        Open* open = open__unpack(NULL, message_size, message_buffer);

        FileResponse* resp = malloc(sizeof(FileResponse));
        open_file(open, resp);
        uint32_t send_size = file_response__get_packed_size(resp) + 2*sizeof(uint32_t);
        void* send_buffer = malloc(send_size);

        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(FILE_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        file_response__pack(resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);

        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        free(send_buffer);
        free(resp);
        open__free_unpacked(open, NULL);
        break;
      }
    case GETATTR_MESSAGE:
      {
        Simple * getattr = simple__unpack(NULL, message_size, message_buffer);
        GetAttrResponse resp = GET_ATTR_RESPONSE__INIT;
        get_attr(getattr, &resp);

        /*  Send to client code */
        uint32_t send_size = get_attr_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(FILE_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        get_attr_response__pack(&resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        free(send_buffer);
        simple__free_unpacked(getattr, NULL);
        break;
      }
    case WRITE_MESSAGE:
      {
        Write *write_msg =  write__unpack(NULL, message_size, message_buffer);
        StatusResponse resp = STATUS_RESPONSE__INIT;
        write_file(write_msg, message_size, &resp);

        /* Send to client */
        uint32_t send_size = status_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2*sizeof(uint32_t));
        message_type = htonl(ERROR_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        status_response__pack(&resp, send_buffer + 2*sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);

        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }
        fprintf(stderr, "successfully sent the message over back to the client\n");
        write__free_unpacked(write_msg, NULL);
        break;
      }
    case READ_MESSAGE:
      {
        Read * read = read__unpack(NULL, message_size, message_buffer);
        ReadResponse resp = READ_RESPONSE__INIT;
        void * read_buffer = read_help(read, &resp); 

        /*  Send to client code */
        uint32_t send_size = read_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(READ_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        read_response__pack(&resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }
        free(send_buffer);
        free(read_buffer);
        read__free_unpacked(read, NULL);
        break;
      }
    case CLOSE_MESSAGE:
      {
        Close * close = close__unpack(NULL, message_size, message_buffer);
        ErrorResponse resp = ERROR_RESPONSE__INIT;
        close_file(close, &resp);

        /*  Send to client code */
        uint32_t send_size = error_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(READ_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        error_response__pack(&resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        free(send_buffer);
        close__free_unpacked(close, NULL);
        break;
      }
    case MKDIR_MESSAGE:
      {
        Create * create = create__unpack(NULL, message_size, message_buffer);
        ErrorResponse resp = ERROR_RESPONSE__INIT;
        make_dir(create, &resp);

        /*  Send to client code */
        uint32_t send_size = error_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(READ_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        error_response__pack(&resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        create__free_unpacked(create, NULL);
        free(send_buffer);
        break;
      }
    case OPENDIR_MESSAGE:
      {
        Simple * dir = simple__unpack(NULL, message_size, message_buffer);
        ErrorResponse resp = ERROR_RESPONSE__INIT;
        open_dir(dir, &resp);

        /*  Send to client code */
        uint32_t send_size = error_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(READ_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        error_response__pack(&resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        simple__free_unpacked(dir, NULL);
        free(send_buffer);
        break;
      }
    case READDIR_MESSAGE:
      {
        Simple * readdir = simple__unpack(NULL, message_size, message_buffer);
        ReadDirResponse resp = READ_DIR_RESPONSE__INIT;
        read_directory(readdir, &resp);

        /*  Send to client code */
        uint32_t send_size = read_dir_response__get_packed_size(&resp) + 2*sizeof(uint32_t);
        void * send_buffer = malloc(send_size);
        uint32_t net_data_size = htonl(send_size - 2 * sizeof(uint32_t));
        message_type = htonl(READ_RESPONSE_MESSAGE);
        memcpy(send_buffer, &net_data_size, sizeof(uint32_t));
        memcpy(send_buffer + sizeof(uint32_t), &message_type, sizeof(uint32_t));
        read_dir_response__pack(&resp, send_buffer + 2 * sizeof(uint32_t));

        int num_written = write(thr_arg->socket, send_buffer, send_size);
        while(num_written < send_size)
        {
          write(thr_arg->socket, send_buffer + num_written, send_size - num_written);
        }

        simple__free_unpacked(readdir, NULL);
        clean_readdir(&resp);
        free(send_buffer);
        break;
      }

  }

  close(thr_arg->socket);
  free(message_buffer);
  free(args);
  return NULL;
}
