#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "filesystem.h"

static char * root_path = NULL;

char * get_full_path(char * relative_path){
  char * new_path = (char *)malloc(strlen(relative_path) + strlen(root_path) + 1);
  new_path[0] = '\0';

  strcat(new_path, root_path);
  strcat(new_path, relative_path);

  return new_path;
}

char * get_root_path(){
  return root_path;
}

bool set_root_path(const char * new_root){
  char path[PATH_MAX + 1];
  struct stat dir;

  stat(new_root, &dir);

  if(!S_ISDIR(dir.st_mode)){
    return false;
  }

  if(root_path != NULL){
    free(root_path);
  }

  root_path = strdup(new_root);
  
  realpath(new_root, path);
  root_path = strdup(path);
  
  /*Remove the trailing slash from the path, if applicable*/
  if(root_path[strlen(root_path) - 1] == '/'){
    root_path[strlen(root_path) - 1] = '\0';
  }

  return true;
}
