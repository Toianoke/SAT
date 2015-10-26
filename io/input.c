#include <string.h>
#include <stdio.h>

typedef struct sizes
{
  int lines;
  int chars;
} sizes_t;




sizes_t line_number(char *file)
{

	sizes_t numbers;
	int lines = 0;
	int max_char = 0;
	int len = 0;
	FILE *f = fopen(file,"r");

  if (f == NULL)
    return numbers;
  while(!feof(f))
    {
      if(fgetc(f) == '\n')
    	{
	  if(len >= max_char)
	    max_char = len;
	  len = 0;
	  lines++;
    	}
      else
    	{
	  len++;
    	}	
    }
  numbers.lines = lines;
  numbers.chars = max_char;
  fclose(f);
  return numbers;
}




int main(int argc, char **argv)
{
  char c;
  char *input = argv[1];
  FILE *input_file;
  sizes_t line_num = line_number(input);
  int num_lines = line_num.lines;
  int max_char = line_num.chars;
	
<<<<<<< HEAD

	char *lines[num_lines];
    char line[max_char];
=======
  //char *lines[8];
  char *lines[num_lines];
  char line[max_char];
>>>>>>> db563980ca6e3962f90994be67915bc496f7adfb
    
  input_file = fopen(input, "r");
    
  //printf("The size of the input file: %lu\n", strlen(input_file));
  if (input_file == 0)
    {
      //fopen returns 0, the NULL pointer, on failure
      perror("Canot open input file\n");
      return -1;
    }
    
    
   
    
  printf("The number of lines in the file: %i and chars: %i\n", num_lines, max_char);
  int j = 0;
  int i = 0;
  do 
    {
    
      c = fgetc(input_file);
      //printf("Characters in the string.: %c\n", c);
      if(c == '\n')
    	{
<<<<<<< HEAD
			j = 0;
    		printf("Line %i: %s\n", i, line);
    		int n;
    		//char line[max_char];
    		lines[i] = line;
			i++;
=======
	  j = 0;
	  printf("Line %i: %s\n", i, line);
	  int n;
	  //char line[max_char];
	  //lines[i] = line;
	  i++;
>>>>>>> db563980ca6e3962f90994be67915bc496f7adfb
    					
	}
      else
    	{
	  //printf("Characters in the string.: %c, %i\n", c, j);
	  line[j] = c;
	  j++;
    		
    		
    		
    	}
    		

    } while (!feof(input_file));

  fclose(input_file);
  return 0;
}
