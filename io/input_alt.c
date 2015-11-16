#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "solver.h"

#define MAX_NUM 65536
int check_dup(short ar[], short size)
{
	int i, j, dup = 0;
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
		{
			if(ar[i] != 0 && ar[j] != 0 && i != j)
			{
				//printf("Comparing %i and %i\n", abs(ar[i]), abs(ar[j]));
				if(abs(ar[i]) == abs(ar[j]))
					dup++;
			}
		}
	}
	if(dup == 0)
		return 0;
	else
		return 1;
}


size_t count_lines(char * file_name, size_t * max_line_len)
{
  	size_t lines = 0;
  	*max_line_len = 1;
  	char last = 0;
	
  	FILE *fp = fopen(file_name, "r");
	
  	if (fp == NULL) 
  	{
  	  	return lines;
  	}
	
  	size_t line_len = 1;
  	while (!feof(fp)) 
  	{
  	    line_len++;
  	    if (line_len > *max_line_len) 
  	    {
			*max_line_len = line_len;
  	    }
	
		last = fgetc(fp);
  	    if (last == '\n') {
			lines++;
			line_len = 1;
		}
	}
	
  	fclose(fp);
  	
  	//if (last != '\n') {
  	//lines++;
  	//}
  	
  	return lines;
}


int main(int argc, char ** argv)
{
	if (argc < 2)
  	{
  		printf("Error!");
		return 0; // argument error (but we have to return 0)
  	}
  
  	size_t max_line_len;
  	size_t line_count = count_lines(argv[1], &max_line_len);
  	//printf("Line count: %lu\nMax line len: %lu\n", line_count, max_line_len);
  
  	char *lines[line_count];
  	short **clauses;
	size_t i;
  	for (i=0; i<= line_count; i++) 
  	{
    	lines[i] = (char *) malloc(sizeof(char)*max_line_len);

    	if (lines[i] == NULL) 
    	{
      		return 0; // internal error (but we have to return 0)
    	}
  	}

  	FILE *fp = fopen(argv[1], "r");
  	if (fp == NULL) 
  	{
  		printf("Error!");
    	return 0; // file error (but we have to return 0)
  	}
  	size_t c_i = 0;
  	for (i=0; i < line_count;) 
  	{
    	lines[i][c_i] = fgetc(fp);
    	if (lines[i][c_i] == '\n' || lines[i][c_i] == '\0') 
    	{
      		lines[i][c_i+1] = '\0';
      		//printf("Line %lu: %s", i, lines[i]);
      		c_i = -1;
      		i++;
    	}
    	c_i++;
  	}
  
  	int /*i,*/ cls_cnt, var_cnt, temp, first = 1;
  	int com_done = 0;
  	int init_set = 0;
  	int num_clss = 0;
  	int row = 0;
  	for(i = 0; i <= line_count; i++)
  	{
  		//printf("The index: %i and size: %lu\n", i, line_count);
  		if(lines[i][0] == 'c')
  		{
  			if(com_done == 1)
  			{
  				printf("ERROR: Comment!\n");
  				return -1;
  			}
  		}
  		else if(lines[i][0] == 'p')
  		{
  			com_done = 1;
  			if(init_set == 1)
  			{
  				printf("ERROR already init!\n");
  				return -1;
  			}
  			init_set = 1;
  			char *split;
  			split = strtok(lines[i]," ");
  			char* error;
  			while(split != NULL)
  			{
  				temp = strtoll(split, &error, 10);
  				if(temp >= MAX_NUM)
  				{
  					printf("ERROR!");
  				
  				}
				if (temp != 0)
				{
					if(first == 1)
					{
						var_cnt = temp;
						first = 0;
					}
					else
					{
						cls_cnt = temp;
					}		
				}
  				split = strtok(NULL, " ");
  			}
  			if((var_cnt != 0) && (cls_cnt != 0))
  			{
  				
  				//printf("Creating an array.\n");
  				clauses = (short **)malloc(cls_cnt * sizeof(short*));
  				int i, j;
  				for(i = 0; i < cls_cnt; i++)
  					clauses[i] = (short *)malloc((var_cnt + 1)* sizeof(short));
  				for(i = 0; i < cls_cnt; i++)
  					for(j = 0; j <= var_cnt+1; j++)
  					{
  						clauses[i][j] = 0;
  					
  					
  					}
  				//for(i = 0; i < cls_cnt; i++)
  				//{
  				//	for(j = 0; j <= var_cnt; j++)
  				//	{
  				//		printf(" %i ", clauses[i][j]);
  				//	
  				//	
  				//	}
  				//	printf("\n");
  				//}
  				
  				
  			//
  			}
  		}
  		else if((isdigit(lines[i][0])) || (lines[i][0] == '-'))
  		{
  			if(init_set != 1)
  			{
  				printf("ERROR not init!\n");
  				return -1;
  			}
  			char *split;
  			int num_tok = 0;
  			split = strtok(lines[i]," ");
  			
  			short temp_clause[max_line_len];
  			char* error;
  			while(split != NULL)
  			{	
  				//printf("The string: %s\n",split);
  				temp = strtoll(split, &error, 10);
  				if(temp != 0)
  				{
  					if(abs(temp) > var_cnt)
  					{
  						printf("ERROR!\n");
  						return -1;
  					}
  				
  					temp_clause[num_tok] = temp;
  					//printf("The num: %i index: %i\n", temp_clause[num_tok], num_tok);
  					num_tok++;
  				}
  				
  				split = strtok(NULL, " ");	
  			}
  			
  			
  			
  			if(check_dup(temp_clause, num_tok))
  			{
  				printf("ERROR\n");
  				return -1;
  			
  			}
  			
  			
  			
  			
  			int num = 0;
  			int i;
  			for(i = 0; i < num_tok; i++)
  			{
  				clauses[row][i] = temp_clause[i];
  			}
  			
  			//printf("Size of array: %i\n", num_tok);
  			//printf("The index where the array is being saved: %i\n", num_clss);
  			//int j;
  			//for(j = 0; j < num_tok; j++)
  			//{
  			//	printf(" %i ", clauses[row][j]);
  			//}
  			//printf("\n");
  			row++;
  		}
  	}
  	
  	
  	for(i = 0; i <= line_count; i++)
  	{
  		printf("freeing line\n");
  		free(lines[i]);
  	}
  	
  	
  	printf("The final array with %i clauses and at the most %i variables in each clause:\n", cls_cnt, var_cnt);
  	int j;
  	
  	
  	for(i = 0; i < cls_cnt; i++)
  	{
  		for(j = 0; j <= var_cnt; j++)
  		{
  			printf(" %i ", clauses[i][j]);
  		}
  		printf("\n");
  	}
	// tt uncommented Nov 11 16:06
  	if(solve(var_cnt, cls_cnt, clauses))
  	{
  		printf("SOLVABLE");
  		return 0;
  	}
  	else
  	{
  		printf("UNSOLVABLE");
  		return 0;
  	
  	}
  
  	for(i = 0; i < cls_cnt; i++)
  	{
  		printf("freeing clause\n");
  		free(clauses[i]);
  	}
  	free(clauses);
  
  	fclose(fp);
  	return 0;
}
