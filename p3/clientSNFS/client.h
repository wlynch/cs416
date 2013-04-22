#ifndef CLIENT_H_
#define CLIENT_H_

#define CLIENT_NAME "XML-RPC CS416 Test Client"
#define CLIENT_VERSION "1.0"


char *address;
char *mount_path;

xmlrpc_client *client;
xmlrpc_env env;

void init_client()
{    
    xmlrpc_env_init(&env);
    xmlrpc_client_setup_global_const(&env);
    xmlrpc_client_create(&env, XMLRPC_CLIENT_NO_FLAGS, CLIENT_NAME, \
            CLIENT_VERSION, NULL, 0, &client);
}

void destroy_client()
{
    xmlrpc_env_clean(&env);
    xmlrpc_client_destroy(client);
    xmlrpc_client_teardown_global_const();
}

#endif

