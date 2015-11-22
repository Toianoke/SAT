#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static int idx;


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
  // printf("out of loop num_clauses: %d\n", f->num_clauses);
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

void remove_var(short indx, Clause *c){
  assert(c != NULL);

  short j;

  j = indx + 1;
  while(j<c->num_lits)
    c->literals[indx++] = c->literals[j++];
  
  c->num_lits--;
}

void remove_clause(short indx, Formula *f){
  assert(f != NULL);
  short j;

  j = indx + 1;
  //printf(" f->clauses[indx]: %d\n",  f->clauses[indx].num_lits);
  //printf(" f->clauses[indx]: %ux\n",  f->clauses[indx].literals);
  // IB seems to be removing literals instead of a clause
  f->clauses[indx] = f->clauses[f->num_clauses-1];
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
  Formula *f1;
    short i, j;
  Clause *c;
  
  f1 = malloc(sizeof(Formula));

  memcpy(f1, f, sizeof(Formula));
  f1->num_clauses = f->num_clauses;
  f1->vl_length = f->vl_length;
  f1->svl_length = f->svl_length;
  f1->clauses = malloc(sizeof(Clause)*f1->num_clauses);
  memcpy(f1->clauses, f->clauses, sizeof(Clause)*f1->num_clauses);
  f1->var_list = malloc(sizeof(short)*f->vl_length);
  memcpy(f1->var_list, f->var_list, sizeof(short)*f->vl_length);
  f1->sorted_var_list =  malloc(sizeof(Var)*f->svl_length);
  memcpy(f1->sorted_var_list, f->sorted_var_list, sizeof(Var)*f->svl_length);

  
  for(i=0; i<f1->num_clauses; i++)
  {
		c = &f1->clauses[i];
		for(j=0; j<c->num_lits; j++)
		{
			/*if(c->literals[j] == n){
				remove_var(j, c);
				j--;}*/
			 if(c->literals[j] == n)
				remove_clause(i--, f1);
			else if(c->literals[j] == (-1*n))
				remove_var(j--, c);
		}
  }
  return f1;
}

int has_single_polarity(short v, Formula *f)
{
  assert(f != NULL);
  int result;
  v--;
  result = (f->var_list[v] ^ f->var_list[v + f->svl_length]);
  //printf("result: %d\n", result);
  return result;
  
  int i;
  for (i = 0; i < f->vl_length; i++){
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
  int i, j;
  short v= 0;
  Clause *cp = f->clauses;

  for(i = 0; i < f->vl_length; i++)
  {
	v = f->sorted_var_list[i].value;
    if(has_single_polarity(v, f))
	{
	  for(j = 0; j < f->num_clauses; j++)
	  {
	    if(clause_contains(&f->clauses[j], v))
		{
//		  printf("B\n");
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
  return f->sorted_var_list[idx--].value;  
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

int cmp_Var(const void *a, const void *b)
{
	const struct Var *Va = a;
	const struct Var *Vb = b;

	if (Va->frequency < Vb->frequency)
		return -1;

	if (Va->frequency > Vb->frequency)
		return 1;

	return 0;
}

void free_formula(Formula *f){
	int i;
	Clause *c = f->clauses;
	free(f->sorted_var_list);
	free(f->var_list);
	for(i = 0; i < f->num_clauses; i++)
	{
		free(c[i].literals);
	}
	free(f->clauses);
	free(f);
}
/*
 * returns a new Formula with nv variables
 * and nc clauses, where clauses is an array
 * of arrays of shorts, representing each clause
 * in the formula
 */
Formula* create_formula(short nv, short nc, short **in_clauses){
  assert(in_clauses != NULL);
  short var;
  int i, j, count;
  
  Formula *f = malloc(sizeof(Formula));
  f->num_clauses = nc;
  f->var_list = malloc(sizeof(short)*2*nv);
  for (i = 0; i < nv*2; i++) 
	f->var_list[i] = 0;

  f->clauses = malloc(sizeof(Clause)*nc);

  for(i = 0; i < f->num_clauses; i++)
  {
    count = 0;
    while(in_clauses[i][count] != 0)
	  count++;

    f->clauses[i].num_lits = count;
    f->clauses[i].literals = malloc(sizeof(short)*(count+1));
	
    for (j = 0; j < count; j++){
	  var = in_clauses[i][j];
	  f->clauses[i].literals[j] = var;
	  if(var < 0)
		  f->var_list[(abs(var)) + nv-1]++;//keep track how many times var occurs
	  else
		  f->var_list[var-1]++;
	}

    f->clauses[i].literals[j] = 0;
  }
  f->vl_length = nv*2;
  f->svl_length = nv*2;
  f->sorted_var_list = malloc(sizeof(Var)*f->vl_length);
  
  for (i = 0; i < nv; i++) {
		f->sorted_var_list[i].frequency = f->var_list[i];
		f->sorted_var_list[i].value = i+1;
  }
  for (i = nv; i < f->vl_length; i++) {
		f->sorted_var_list[i].frequency = f->var_list[i];
		f->sorted_var_list[i].value = -(i-nv+1);	  
  }


  qsort(f->sorted_var_list, nv, sizeof(Var), cmp_Var);
  
  return f;
}

int dpll(Formula *F){
  assert(F != NULL);
  Formula *f;
  short i;
  if(is_consistent_literals(F))
    return 1;
  if(contains_empty_clause(F))
    return 0;
  
  for(i=0; i<F->num_clauses; i++)
    if(is_unit_clause(F->clauses[i]))
		f = propagate_unit(F, F->clauses[i].literals[0]);

  eliminate_pure_literals(F);
  /* pick one variable v */
  short v = pick_var_from_formula(f);
  //printf("idx: %d, v: %d\n",idx, v);
  return dpll(propagate_unit(f, v)) || dpll(propagate_unit(f, ((-1)*v)));
}

/*
 * called from outside
 */
int solve(short nv, short nc, short **clauses){
  assert(clauses != NULL);
  Formula *F = create_formula(nv, nc, clauses);
  idx = F->vl_length-1;
  return dpll(F);
}
