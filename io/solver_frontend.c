#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "solver.h"

#define BUFFSIZE (262144)

/**
 * Prints "ERROR\n" and exits with 0 condition when bool is true
 */
static void
error_on(int bool)
{
  if (bool) {
    printf("ERROR\n");
    exit(0);
  }
}


/**
 * Prints "UNKNOWN\n" and exits with 0 condition when bool is true
 */
void // TODO: may bee good to allow solver to use this func on bad mallocs
unknown_on(int bool)
{
  if (bool) {
    printf("UNKNOWN\n");
    exit(0);
  }
}


/**
 * Similar interface as strtol without base,
 *     and with inclusive min and max values
 * Exits on all non-parsable or invalid inputs
 */
static long
parse_long(char* str, char ** endptr, long min, long max)
{
  assert(str != NULL);
  assert(endptr != NULL);
  assert(min <= max);
  
  long retval = strtol(str, endptr, 10);
  error_on(*endptr == str);
  error_on(errno == ERANGE);
  error_on(retval < min);
  error_on(retval > max);
  return retval;
}


/**
 * Opens file and reads the reduced DIMACS header.
 *     Disregards comment lines.
 * Exits on improper formatting or file errors
 */
static FILE *
read_header(char * filename, short * nbvar, short * nbclauses)
{
  assert(filename != NULL);
  assert(nbvar != NULL);
  assert(nbclauses != NULL);
  
  FILE * fp = fopen(filename, "r");
  error_on(fp == NULL);

  char * buff = (char *) malloc(sizeof(char)*(BUFFSIZE+1));
  unknown_on(fp == NULL);

  while(1){
    char * out = fgets(buff, BUFFSIZE, fp);
    error_on(ferror(fp) != 0);
    error_on(out == NULL);

    // Skip lines with comments
    if (out[0] == 'c') {
      while (strlen(out) == BUFFSIZE-1) {
	out = fgets(buff, BUFFSIZE, fp);
	error_on(ferror(fp) != 0);
	error_on(out == NULL);
      }
      continue;
    }

    // TODO: skip blank lines

    // If its not a comment it MUST be the header line
    // TODO: What about whitespace, eg "p  cnf"?
    error_on(strncmp("p cnf ", out, 6) != 0);

    // TODO: What about whitespace causing this to split a buffer?
    char * endptr1;
    *nbvar = parse_long(&(out[6]), &endptr1, 1, MAX_VAR_CNT-1);

    char * endptr2;
    *nbclauses = parse_long(endptr1, &endptr2, 1, MAX_CLAUSE_CNT-1);

    // There can be only whitespace after the second number
    while (*endptr2 == ' ') {
      endptr2++;
    }
    error_on(*endptr2 != '\n');

    free(buff);
    return fp;
  }
}


/**
 * Uses open file to read a DIMACS body.
 *     Disregards comment lines.
 * Exits on improper formatting or file errors
 */
static short **
read_body(FILE* fp, short nbvar, short nbclauses)
{
  assert(fp != NULL);
  assert(nbvar > 0);
  assert(nbclauses > 0);
  
  short ** retval = (short **) malloc(sizeof(short *)*(nbclauses));
  unknown_on(retval == NULL);

  short * temp = (short *) malloc(sizeof(short)*(nbvar+1));
  unknown_on(temp == NULL);

  char * buff = (char *) malloc(sizeof(char)*(BUFFSIZE+1));
  unknown_on(buff == NULL);

  int clause_index = 0;
  temp[0] = 0;
  char * out;
  while (1) {
    out = fgets(buff, BUFFSIZE, fp);
    error_on(ferror(fp) != 0);
    error_on(out == NULL);

    // Skip lines with comments
    if (out[0] == 'c') {
      while (strlen(out) == BUFFSIZE-1) {
	out = fgets(buff, BUFFSIZE, fp);
	error_on(ferror(fp) != 0);
      }
      continue;
    }

    // TODO: skip empty lines

    // place numbers into temp, error on same or negate literals in a line
    char * endptr1 = out;
    char * endptr2;
    int found_zero = 0;
    int i,j;
    for (i=0; i<=nbvar; i++) {
      temp[i] = parse_long(endptr1, &endptr2, -nbvar, nbvar);

      // Check for duplicated or inverted literals in the same clause
      for (j=0; j<i; j++) {
	error_on(temp[j] == temp[i]);
	error_on(temp[j] == -temp[i]);
      }

      endptr1 = endptr2;
      if (temp[i] == 0) {
	found_zero = 1;
	break;
      }
    }

    // TODO: what if a line splits a buffer?

    // a 0 must be the last thing in a line other than whitespace
    error_on(found_zero == 0);
    while (*endptr2 == ' ') {
      endptr2++;
    }
    error_on(!(*endptr2 == '\n' || *endptr2 == '\0'));

    // malloc the final placement of the array and copy from temp
    retval[clause_index] = (short *) malloc(sizeof(short)*(i));
    unknown_on(retval[clause_index] == NULL);
    memcpy(retval[clause_index], temp, sizeof(short)*(i+1));

    // Go to next clause and terminate when done
    clause_index++;
    if (clause_index == nbclauses) {
      // there should be no lines with content after the last clause;
      // read until there is no more
      out = endptr2;
      while (!feof(fp)) {
	// TODO skip comments
	while (*out == ' ') {
	  out ++;
	}
	error_on(!(*out == '\n' || *out == '\0'));
	out = fgets(buff, BUFFSIZE, fp);
	error_on(ferror(fp) != 0);
      }

      free(temp);
      free(buff);
      return retval;
    }
  }
}


/**
 * Takes a single filename as input.
 * Prints ERROR\n on invalid input: no filename given, invalid filename,
 *                                  improperly formatted file,
 * Prints UNKNOWN\n on internal error: malloc failing
 * Prints SATISFIABLE\n if input is sat
 * Prints UNSATISFIABLE\n if input is unsat
 */
int
main(int argc, char ** argv)
{
  error_on(argc != 2);

  short nbvar, nbclauses;
  FILE * fp = read_header(argv[1], &nbvar, &nbclauses);

  short ** clauses = read_body(fp, nbvar, nbclauses);
  fclose(fp);

#ifdef DEBUG
  printf("The final array with %d clauses and at the most %d "
	 "variables in each clause:\n", nbclauses, nbvar);
  short i,j;
  for (i=0; i<=nbclauses; i++) {
    for (j=0; j<=nbvar; j++) {
      printf("%d ",clauses[i][j]);
      if (clauses[i][j] == 0) {
	break;
      }
    }
    printf("\n");
  }
#endif

  if (solve(nbvar, nbclauses, clauses)) {
    printf("SATISFIABLE\n");
  } else {
    printf("UNSATISFIABLE\n");
  }

  return 0;
}
