#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "parser.h"

char * parser_next_word(FILE * fd) {
	if (!fd)
		return NULL;

  char c;
  char * word;
  int word_length = 0;

  do { c = fgetc(fd); } while ( c != EOF && !isalnum(c) );
  fseek(fd, -1, SEEK_CUR); /* rewind 1 char */

  do {
    c = fgetc(fd);
    if ( !isalnum(c) ) { break; }
    word_length++;
  } while (c != EOF);

  if (word_length == 0) { return NULL; }

  fseek(fd, -(word_length + 1), SEEK_CUR);
  word = calloc(1,word_length + 1);
  fread(word, sizeof(char), word_length, fd);

  return word;
}

char * parser_next_line(FILE * fd) {
	if (!fd)
		return NULL;

  char c = ' ';
  int count = 0;
  int cur_max = 128;
  char * line = calloc(1,cur_max + 1);

  while ( (c != '\n') && (!feof(fd)) ) {
    if (count == cur_max) {
      cur_max *= 2;
      line = realloc(line, cur_max);
    }
    c = fgetc(fd);
    line[count++] = c;
  }

  if (count == 0 || line[0] == 0) {
    return NULL;
  }

  line[count - 1] = '\0';
  return line;
}

