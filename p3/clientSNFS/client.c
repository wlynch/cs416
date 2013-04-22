#define FUSE_USE_VERSION 26

#include <fuse.h>

#ifdef __APPLE__
  #include <fuse_darwin.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include "client.h"

bool parse_input(char **, char **);
char *create_addr();
extern struct fuse_operations ops;

int port_number;

int main(int argc, char **argv)
{
    char * fuse_args[6];
    bool parsed;

    if(argc != 7)
    {
       printf("Error, your input must contain input arguments" \
                "please supply a -port, -address, and -mount option\n");
        return 1;
    }

    parsed = parse_input(argv, fuse_args);

    if(!parsed)
    {
        printf("Error, you must supply a -mount, -address, and -port flag\n");
        return 1;
    }

    address = create_addr();
    init_client();

    /*xmlrpc_client_call2f(&env, client, address, methodName, &result,
            "(ii)", (xmlrpc_int32) 7, (xmlrpc_int32) 7);

    xmlrpc_read_int(&env, result, &sum);

    xmlrpc_DECREF(result);*/

    fuse_args[2] = "-o";
    fuse_args[3] = "user_allow_other";
    fuse_args[4] = "-o";
    fuse_args[5] = "allow_root";

    return fuse_main(4, fuse_args, &ops, NULL);
}

bool parse_input(char **argv, char **fuse_args)
{
    int i;

    for(i = 1; i < 7; i += 2)
    {
        if(strcmp(argv[i], "-mount") == 0)
        {
            fuse_args[0] = "-mount";
            fuse_args[1] = argv[i + 1];
            mount_path = argv[i + 1];
        }
        else if(strcmp(argv[i], "-port") == 0)
        {
            port_number = strtol(argv[i + 1], (char **)NULL, 10);
        }
        else if(strcmp(argv[i], "-address") == 0)
        {
            address = argv[i + 1];
        }
        else
        {
            return false;
        }
    }

    return true;
}

char *create_addr()
{
    char number[7];
    char *addr = (char *)malloc((strlen(address) + 19)*sizeof(char));
    addr[0] = '\0';

    sprintf(number, "%d", port_number);

    strcat(addr, "http://");
    strcat(addr, address);
    strcat(addr, ":");
    strcat(addr, number);
    strcat(addr, "/RPC2");

    return addr;
}
