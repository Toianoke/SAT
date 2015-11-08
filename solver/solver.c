#include "formula.h"
#include <stdio.h>
#include <stdlib.h>


/*
 *Helper function that returns a literal
 *which is identical to lit but with an inverted sign.
 */
/*
Literal negated(Literal lit)
{
	Literal new_lit;
	new_lit.ID = lit.ID;
    new_lit.sign = !lit.sign;
	return new_lit;
}
*/

/*
 *Helper function that determines if two literals are equal,
 * Two literals are equal if they have the same ID and the same
 * sign.
 */
/*
int equals(Literal lit1, Literal lit2)
{
	return ((lit1.ID == lit2.ID) && (lit1.sign == lit2.sign));
}
*/

/*
 * returns 1 if f is composed only of
 * literals that are consistent
 * i.e. all clauses contain 1 variable and
 *      no two clauses contain a variable and its negation
 */
int is_consistent_literals(Formula *f){
  int i,j;
  Clause *cp = f->clauses;
  //Check the length of all clauses
  for(i = 0; i < f->num_clauses; i++)
  {
    if(cp++->num_lits != 1)
	  return 0;
  }

  cp = f->clauses;
  //check to see if the negation of any variable is present in f
  //If the code makes it to this point all clauses contain only one variable.
  for(i = 0; i < f->num_clauses-1; i++)
  {
	  for (j = i; j < f->num_clauses; j++) {
		  Clause* next = cp+1;
		  if(cp->literals[0] + next->literals[0] == 0)
		    return 0;
		  /*
		  if(equals(cp->literals[0], negated(next->literals[0]) ))
			 return 0;
		  */
	  }
	  cp++;
  }
  return 1;
}

/*
 * returns 1 if there is at least one clause in f
 *              that is empty
 *         0 otherwise
 */
int contains_empty_clause(Formula *f){
  int i;
  Clause *cp = f->clauses;

  for(i = 0; i < f->num_clauses; i++)
  {
	if(cp++->num_lits == 0)
		return 1;	
  }
  return 0;
}

/*
 * returns 1 if c contains exactly 1 variable
 *         0 otherwise
 */
int is_unit_clause(Clause c){
  return c.num_lits == 1;
}

void remove_var(short idx, Clause *c){
  short j;

  j = idx + 1;
  while(j<c->num_lits)
    c->literals[idx++] = c->literals[j++];
  c->num_lits--;
}

void remove_clause(short idx, Formula *f){
  short j;

  j = idx + 1;
  while(j<f->num_clauses)
    f->clauses->literals[idx++] = f->clauses->literals[j++];
  f->num_clauses--;
}

/*
 * for each clause c in f:
 *   if n appears in c:
 *     remove c from f
 *   if -n appears in c:
 *     remove -n from c
 */
Formula* propagate_unit(Formula *f, short n){
  short i, j;
  Clause *c;

  for(i=0; i<f->num_clauses; i++){
    c = &f->clauses[i];
    for(j=0; j<c->num_lits; j++){
      if(c->literals[j] == n)
	remove_clause(i, f);
      else if(c->literals[j] == (-1*n))
	remove_var(j, c);
    }
  }
}

int has_single_polarity(short v)
{

}

/*
 *Returns true if clause c contains variable v,
 *false, otherwise.
 */
int contains(Clause *c, short v)
{

}

/*
 * for any variable v in F with a single polarity
 *   remove every clause in which v occurs
 */
void eliminate_pure_literals(Formula *f){
	/*This is how I'd like to propose writing eliminate_pure_literals.
	 *It requires adding two new fields to Formula struct - variable_list
	 *and variable_list_length. variable_list will be made in create_formula.
	 *It will contain all the variables without any duplications (kind of like
	 *Tony already suggested).
	 */
	int i, j;
	short v, c_idx = 0;
  Clause *cp = f->clauses;

  for(i = 0; i < f->variable_list_length; i++)
  {
	  v = f->variable_list[i];
	  if(has_single_polarity(v)){
		  for(j = 0; j < f->num_clauses; j++){
			  if(contains(cp++, v))
				  remove_clause(c_idx++, f);
		  }
	  }
  }
  return 1;
}

/*
 * return a variable that occurs in f
 */
short pick_var_from_formula(Formula *f){
  return f->clauses->literals[0];
}

/*
 * returns a new Formula with nv variables
 * and nc clauses, where clauses is an array
 * of arrays of shorts, representing each clause
 * in the formula
 */
Formula* create_formula(short nv, short nc, short **in_clauses){
  int i, j, count;
  Clause *cp;

  Formula *f = malloc(sizeof(Formula));
  f->num_clauses = nc;

  f->clauses = malloc(sizeof(Clause)*nc);
  cp = f->clauses;

  for(i = 0; i < f->num_clauses; i++)
  {
    count = 0;
    while(in_clauses[i][count] != 0)
	  count++;
  
    cp->num_lits = count;

    cp->literals = malloc(sizeof(short)*count);
    for (j = 0; j < count; j++)
	  f->clauses[i].literals[j] = in_clauses[i][j];		  
    cp++;
  }
  return f;
}

/*
 * called from outside
 */
int solve(short nv, short nc, short **clauses){
  return dpll(create_formula(nv, nc, clauses));
}

int dpll(Formula *F){
  short i;

  if(is_consistent_literals(F))
    return 1;
  if(contains_empty_clause(F))
    return 0;

  //for(i=0; i<MAX_CLAUSE_CNT/*change to length of F*/; i++)
  //if(is_unit_clause(F->clauses[i]))
  //propagate_unit(F, F->clauses->variables[0]);

  eliminate_pure_literals(F);
  
  /* pick one variable v */
  short v = pick_var_from_formula(F);
  return dpll(propagate_unit(F, v)) || dpll(propagate_unit(F, ((-1)*v)));
}


int main(int argc, char *argv[])
{
	/*Crazy simple test case made to test is_consistent_literals()
	  where there are only 2 clauses. Each clause is a unit clause, i.e. has
	only one literal. The literals are ID=2, sign = false; ID= 3, sign = false.*/
  int i;
  int num_of_clauses = 2;
  int lit_cnt[2] = {1,1};
  int vars[2] = {2,3};


  Formula *f = malloc(sizeof(Formula));
  f->num_clauses = num_of_clauses;

  f->clauses = malloc(sizeof(Clause)*num_of_clauses);
  Clause *cp = f->clauses;


  for(i = 0; i < f->num_clauses; i++)
  {
//	  printf("f->num_clauses: %d\n", f->num_clauses);
	  cp->num_lits = lit_cnt[i];
	  cp->literals = malloc(sizeof(short)*cp->num_lits);
	  cp->literals[i] = vars[i];
	  cp++;
  }
  // test for is_consistent_literals
  printf("Expecting 1 from is_consistent_literals(): %d\n",
		 is_consistent_literals(f));

  // test for contains_empty_clause
  printf("Expection 0 from contains_empty_clause, actual %d\n",
	 contains_empty_clause(f));

  cp = f->clauses;

  for(i = 0; i < f->num_clauses; i++)
  {
	free(cp->literals);
	cp++;
  }

  free(f->clauses);
  free(f);

  return 0;
}

