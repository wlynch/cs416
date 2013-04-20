#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#define CONFIG_FILE "config_file"

int port_number = 10000;
char *mount_location = "";

bool create_config_file(int);
bool parse_input(char **);


    xmlrpc_value *
sample_add(xmlrpc_env *envP,
        xmlrpc_value *const paramArrayP,
        void *       const serverContext)
{

    xmlrpc_int32 x, y, z;

    /* Parse our argument array. */
    xmlrpc_parse_value(envP, paramArrayP, "(ii)", &x, &y);
    if (envP->fault_occurred)
        return NULL;

    /* Add our two numbers. */
    z = x + y;

    /* Return our result. */
    return xmlrpc_build_value(envP, "i", z);

}

int main(int argc, char **argv)
{
    bool created, parsed;
    xmlrpc_server_abyss_parms serverParam;
    xmlrpc_registry *registryP;
    xmlrpc_env env;

    if(argc != 5)
    {
        printf("Error, you must call this program in the format ./serverSNFS " \
                "-port [port number] -mount [mount location]\n");
        return 1;
    }

    parsed = parse_input(argv);

    if(!parsed)
    {
        printf("Error, unable to parse your input arguments, " \
                "please only use -mount and -port arguments\n");
        return 1;
    }

    created = create_config_file(port_number);

    if(!created)
    {
        printf("Error, unable to create a config file for this server instance, " \
                "please make sure your directory is wrtitable");
        return 1;
    }

    xmlrpc_env_init(&env);

    registryP = xmlrpc_registry_new(&env);
    xmlrpc_registry_add_method(&env, registryP, NULL, \
            "sample.add", &sample_add, NULL);

    serverParam.config_file_name = CONFIG_FILE;
    serverParam.registryP = registryP;

    printf("Starting up our server!!\n");

    xmlrpc_server_abyss(&env, &serverParam, XMLRPC_APSIZE(registryP));

    return 0;
}

bool create_config_file(int port)
{
    FILE *fh = fopen(CONFIG_FILE, "w+");

    if(fh == NULL)
    {
        return false;
    }

    fprintf(fh, "port %d", port);
    fclose(fh);

    return true;
}

bool parse_input(char **argv)
{
    /* Test if the first arg is port */
    if(strcmp(argv[1], "-port") == 0)
    {
        port_number = strtol(argv[2], (char **)NULL, 10);

        if(strcmp(argv[3], "-mount") == 0)
        {
            mount_location = argv[4]; 
            return true;
        }

        return false;
    }
    else if (strcmp(argv[1], "-mount") == 0)
    {
        mount_location = argv[2];

        if(strcmp(argv[3], "-port") == 0)
        {
            port_number = strtol(argv[4], (char **)NULL, 10);
            return true;
        }

        return false;
    }

    return false;
}
