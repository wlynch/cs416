#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char ** argv) {
  int close_res, fd = creat("temp/test.txt", 777);

  if (fd < 0) {
    perror("could not create temp/test.txt. ");
    fprintf(stderr, "errno: %i\n", errno);
    return 1;
  }

  close_res = close(fd);

  if (close_res < 0) {
    perror("could not close the file descriptor.");
    return 1;
  }

  return 0;
}
