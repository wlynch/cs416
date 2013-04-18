#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

    xmlrpc_value *
sample_add(xmlrpc_env *envP,
        xmlrpc_value *const paramArrayP,
        void *       const serverContext)
{

    printf("CALLED THE METHOD!!");
    return NULL;
}

int main(int argc, char **argv)
{
    xmlrpc_server_abyss_parms serverParam;
    xmlrpc_registry *registryP;
    xmlrpc_env env;

    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);
    xmlrpc_registry_add_method(&env, registryP, NULL, \
            "sample.add", &sample_add, NULL);

    serverParam.config_file_name = "config_file";
    serverParam.registryP = registryP;

    printf("Starting up our server!!");

    xmlrpc_server_abyss(&env, &serverParam, XMLRPC_APSIZE(registryP));

    return 0;

}
