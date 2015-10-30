#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>




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
  
  
  
  
  int i;
  
  for(i = 0; i <= line_count; i++)
  {
  	//printf("The index: %i and size: %lu\n", i, line_count);
  	if(lines[i][0] == 'c')
  		printf("Ignore comments.\n");
  	else if(lines[i][0] == 'p')
  	{
  		//char *def_line;
  		//def_line = (char *) malloc(sizeof(char)*strlen(lines[i]));
  		//
  		////printf("Inside the second line\n");
  		//int j;
  		//for(j = 0; j <= sizeof(lines[i]); j++)
  		//{
  		//	if(lines[i][j] == ' ')
  		//	{
  		//		def_line[j] = ' ';
  		//	
  		//	}
  		//	else
  		//	{
  		//		def_line[j] = lines[i][j];
  		//	
  		//	}
  		//	
  		//}
  		//printf("The definition line: %s\n", def_line);
  		char *split;
  		split = strtok(lines[i]," ");
  		while(split != NULL)
  		{	
  			//if(isdigit(split) == 
  			if(isdigit(split))
  			{
  				printf("%s\n",split);
  			}

  			split = strtok(NULL, " ");
  			//printf("The variables: %c, %c \n", split[2], split[3]);
  			
  			
  		}
  	}
  	
  }
  
  
  fclose(fp);
  return 0;
}
