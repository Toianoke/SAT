#include "formula.h"
#include <stdio.h>
#include <stdlib.h>

/*git commit -m "To solver.c added code for is_consistent_literals. Added two new helper functions equals() and negated(). Added a main that tests is_consistent_literals with an absurdly simple test. To formula.h changed everything from arrays to pointers in order to manage memory more efficiently. Old code is simply commented out, in case we'd like to revert."*/

/*
 *Helper function that returns a literal
 *which is identical to lit but with an inverted sign.
 */
Literal negated(Literal lit)
{
	Literal new_lit;
	new_lit.ID = lit.ID;
    new_lit.sign = !lit.sign;
	return new_lit;
}

/*
 *Helper function that determines if two literals are equal,
 * Two literals are equal if they have the same ID and the same
 * sign.
 */
int equals(Literal lit1, Literal lit2)
{
	return ((lit1.ID == lit2.ID) && (lit1.sign == lit2.sign));
}

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
    if(cp->num_lits != 1)
	  return 0;
  }

  cp = f->clauses;
  //check to see if the negation of any variable is present in f
  //If the code makes it to this point all clauses contain only one variable.
  for(i = 0; i < f->num_clauses-1; i++)
  {
	  for (j = i; j < f->num_clauses; j++) {
		  Clause* next = cp+1;
		  if(equals(cp->Lits[0], negated(next->Lits[0]) ))
			 return 0;
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
  // TODO
  int i;
  for(i = 0; i < f->num_clauses; i++)
  {
	if(cp->num_lits == 0)
		return 1;
	
  }
  return 0;
}

/*
 * returns 1 if c contains exactly 1 variable
 *         0 otherwise
 */
int is_unit_clause(Clause c){
  //TODO
}

/*
 * for each clause c in f:
 *   if i appears in c:
 *     remove c from f
 *   if -i appears in c:
 *     remove -i from c
 */
Formula* propagate_unit(Formula *f, int i){
  // TODO
}

/*
 * for any variable v in F with a single polarity
 *   remove every clause in which v occurs
 */
void eliminate_pure_literals(Formula *f){
  // TODO
}

/*
 * return a variable that occurs in f
 */
int pick_var_from_formula(Formula *f){
  // TODO
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
	  cp->Lits = malloc(sizeof(Literal)*cp->num_lits);
	  cp->Lits[i].ID = vars[i];
  	  cp->Lits[i].sign = false;
	  cp++;
  }
  printf("Expecting 1 from is_consistent_literals(): %d\n",
		 is_consistent_literals(f));

  cp = f->clauses;

  for(i = 0; i < f->num_clauses; i++)
  {
	free(cp->Lits);
	cp++;
  }

  free(f->clauses);
  free(f);

  return 0;
}

