#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#define CLIENT_NAME "XML-RPC CS416 Test Client"
#define CLIENT_VERSION "1.0"

bool parse_input(char **);
char *create_addr();

int port_number;
char *address;
char *mount_path;

int main(int argc, char **argv)
{
    int sum;
    bool parsed;
    xmlrpc_env env;
    xmlrpc_client *client;
    xmlrpc_value *result;
    char *methodName = "sample.add";

    if(argc != 7)
    {
        printf("Error, your input must contain input arguments" \
                "please supply a -port, -address, and -mount option");
        return 1;
    }

    parsed = parse_input(argv); 

    if(!parsed)
    {
        printf("Error, you must supply a -mount, -address, and -port flag");
        return 1;
    }

    address = create_addr(); 
    

    xmlrpc_env_init(&env);
    xmlrpc_client_setup_global_const(&env);
    xmlrpc_client_create(&env, XMLRPC_CLIENT_NO_FLAGS, CLIENT_NAME, \
            CLIENT_VERSION, NULL, 0, &client);

    fprintf(stderr, "Got through client creation\n");
    fprintf(stderr, "address is %s\n", address);

    xmlrpc_client_call2f(&env, client, address, methodName, &result,
            "(ii)", (xmlrpc_int32) 7, (xmlrpc_int32) 7);

    fprintf(stderr, "Made the client call\n");

    xmlrpc_read_int(&env, result, &sum);
    printf("The sum is %d\n", sum);

    xmlrpc_DECREF(result);

    xmlrpc_env_clean(&env);
    xmlrpc_client_destroy(client);
    xmlrpc_client_teardown_global_const();
    return 0;
}

bool parse_input(char **argv)
{
    int i;

    for(i = 1; i < 7; i += 2)
    {
        if(strcmp(argv[i], "-mount") == 0)
        {
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
    char *addr = (char *)malloc((strlen(address) + 19)*sizeof(char)); 
    addr[0] = '\0';

    char number[7];
    sprintf(number, "%d", port_number);

    strcat(addr, "http://");
    strcat(addr, address);
    strcat(addr, ":");
    strcat(addr, number);
    strcat(addr, "/RPC2");

    return addr;
}
