#include <stdio.h>
#include "log.h"

FILE *log_open(){
  FILE *logfile;

  // very first thing, open up the logfile and mark that we got in
  //     // here.  If we can't open the logfile, we're dead.
  logfile = fopen("snfs.log", "w+");
  if (logfile == NULL) {
    perror("logfile");
    return NULL;
  }

  // set logfile to line buffering
  setvbuf(logfile, NULL, _IOLBF, 0);

  return logfile;
}

void log_msg(char *msg){
  fprintf(log_file, msg);
  fprintf(log_file, "\n");
}
