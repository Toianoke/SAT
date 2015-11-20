#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
  assert(f != NULL);
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
  assert(f != NULL);

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
  assert(c != NULL);

  short j;

  j = idx + 1;
  while(j<c->num_lits)
    c->literals[idx++] = c->literals[j++];
  c->num_lits--;
}

void remove_clause(short idx, Formula *f){
  assert(f != NULL);
  short j;

  j = idx + 1;
  // IB seems to be removing literals instead of a clause
  f->clauses[idx] = f->clauses[f->num_clauses-1];
  /*
  while(j<f->num_clauses)
    f->clauses->literals[idx++] = f->clauses->literals[j++];
  */
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
  assert(f != NULL);

  short i, j;
  Clause *c;
  Formula *fn;
  short **lits;

  // suspicious code START
  lits = malloc(sizeof(short*)*f->num_clauses);
  for(i=0; i<f->num_clauses; i++){
    lits[i] = malloc(sizeof(short)*((f)->clauses[i].num_lits+1));
    for(j=0; j<f->clauses[i].num_lits; j++)
      lits[i][j] = f->clauses[i].literals[j];
    lits[i][j] = 0;
  }
  fn = create_formula(f->vl_length, f->num_clauses, lits);
  // suspicious code END

  for(i=0; i<fn->num_clauses; i++){
    c = &fn->clauses[i];
    for(j=0; j<c->num_lits; j++){
      if(c->literals[j] == n)
	remove_clause(i, fn);
      else if(c->literals[j] == (-1*n))
	remove_var(j, c);
    }
  }

  return fn;
}

int has_single_polarity(short v, Formula *f)
{
  assert(f != NULL);

  int i;
  
  for (i = 0; i < f->vl_length; i++){
    /*
    printf("v: %d\n", v);
    printf("f->var_list[%d]: %d\n", i, f->var_list[i]);
    */
    if((v + f->var_list[i]) == 0)
      return 0;
  }
  return 1;
}

/*
 *Returns true if clause c contains variable v,
 *false, otherwise.
 */
int clause_contains(Clause *c, short v)
{
  assert(c != NULL);

	int i;
	for (i = 0; i < c->num_lits; i++) {
		if(c->literals[i] == v)
			return 1;
	}
	return 0;
}

/*
 * for any variable v in F with a single polarity
 *   remove every clause in which v occurs
 */
void eliminate_pure_literals(Formula *f){
  assert(f != NULL);
	/*This is how I'd like to propose writing eliminate_pure_literals.
	 *It requires adding two new fields to Formula struct - var_list
	 *and vl_length. variable_list will be made in create_formula.
	 *It will contain all the variables without any duplications (kind of like
	 *Tony already suggested).
	 */
  int i, j;
  short v= 0;
  Clause *cp = f->clauses;

  for(i = 0; i < f->vl_length; i++)
  {
    v = f->var_list[i];
	
    if(has_single_polarity(v, f))
	{
		for(j = 0; j < f->num_clauses; j++)
		{
			if(clause_contains(&f->clauses[j], v))
			{
				remove_clause(j, f);
				j--;
			}
		}
    }
	
  }
}

/*
 * return a variable that occurs in f
 */
short pick_var_from_formula(Formula *f){
  assert(f != NULL);

  return f->clauses->literals[0];
}

int array_contains(short *arr, short length, short item)
{
  assert(arr != NULL);

	int i;
	for (i = 0; i < length; i++) {
		if(arr[i] == item)
			return 1;
	}
	return 0;
}

/*
 * returns a new Formula with nv variables
 * and nc clauses, where clauses is an array
 * of arrays of shorts, representing each clause
 * in the formula
 */
Formula* create_formula(short nv, short nc, short **in_clauses){
  assert(in_clauses != NULL);

  int i, j, k, count;
  Clause *cp;

  Formula *f = malloc(sizeof(Formula));
  f->num_clauses = nc;
  f->var_list = malloc(sizeof(short)*2*nv);

  f->clauses = malloc(sizeof(Clause)*nc);
  cp = f->clauses;

  k = 0;
  for(i = 0; i < f->num_clauses; i++){
    count = 0;
    while(in_clauses[i][count] != 0)
	  count++;
    /*
    printf("create_formula ...\n");
    printf("\tcount: %d\n", count);
    */
    cp->num_lits = count;

    cp->literals = malloc(sizeof(short)*(count+1));
	//printf("cp->literals: %x\n", cp->literals);
    for (j = 0; j < count; j++){
	  f->clauses[i].literals[j] = in_clauses[i][j];
	  if(!array_contains(f->var_list, k, /*abs*/(in_clauses[i][j])))
	  {
	    f->var_list[k] = /*abs*/(in_clauses[i][j]);
	    k++;
	  }
	}
    f->clauses[i].literals[j] = 0;
    cp++;
  }

  f->vl_length = k;
  return f;
}

int dpll(Formula *F){
  assert(F != NULL);

  short i;

  if(is_consistent_literals(F))
    return 1;
  if(contains_empty_clause(F))
    return 0;
  
  for(i=0; i<F->num_clauses; i++)
    if(is_unit_clause(F->clauses[i]))
      propagate_unit(F, F->clauses->literals[0]);

  eliminate_pure_literals(F);
  
  /* pick one variable v */
  short v = pick_var_from_formula(F);
  return dpll(propagate_unit(F, v)) || dpll(propagate_unit(F, ((-1)*v)));
}

/*
 * called from outside
 */
int solve(short nv, short nc, short **clauses){
  assert(clauses != NULL);

  return dpll(create_formula(nv, nc, clauses));
}
