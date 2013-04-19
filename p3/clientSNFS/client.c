#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

int main(int argc, char **argv)
{
    xmlrpc_env env;
    xmlrpc_client *client;
    xmlrpc_value *result;
    int sum;
    char *clientName = "XML-RPC C Test Client";
    char *clientVersion = "1.0";
    char *url = "http://localhost:8080/RPC2";
    char *methodName = "sample.add";

    xmlrpc_env_init(&env);
    xmlrpc_client_setup_global_const(&env);
    xmlrpc_client_create(&env, XMLRPC_CLIENT_NO_FLAGS, clientName, \
            clientVersion, NULL, 0, &client);

    fprintf(stderr, "Got through client creation\n");

    xmlrpc_client_call2f(&env, client, url, methodName, &result,
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
