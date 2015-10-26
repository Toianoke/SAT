#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


size_t count_lines(char * file_name, size_t * max_line_len)
{
  size_t lines = 0;
  *max_line_len = 1;

  FILE *fp = fopen(file_name, "r");

  if (fp == NULL) {
    return lines;
  }

  size_t line_len = 1;
  while (!feof(fp)) {
      line_len++;
      if (line_len > *max_line_len) {
	*max_line_len = line_len;
      }
	
      if (fgetc(fp) == '\n') {
	lines++;
	line_len = 1;
      }
    }

  fclose(fp);
  
  return lines;
}


int main(int argc, char ** argv)
{
  if (argc < 2) {
    return 0; // argument error (but we have to return 0)
  }
  
  size_t max_line_len;
  size_t line_count = count_lines(argv[1], &max_line_len);
  printf("Line count: %lu\nMax line len: %lu\n", line_count, max_line_len);
  
  char *lines[line_count];

  for (size_t i=0; i<line_count; i++) {
    lines[i] = (char *) malloc(sizeof(char)*max_line_len);

    if (lines[i] == NULL) {
      return 0; // internal error (but we have to return 0)
    }
  }

  FILE *fp = fopen(argv[1], "r");
  if (fp == NULL) {
    return 0; // file error (but we have to return 0)
  }
  size_t c_i = 0;
  for (size_t i=0; i<line_count;) {
    lines[i][c_i] = fgetc(fp);
    if (lines[i][c_i] == '\n') {
      lines[i][c_i+1] = '\0';
      printf("Line %lu: %s", i, lines[i]);
      c_i = -1;
      i++;
    }
    c_i++;
  }
  fclose(fp);
  return 0;
}
