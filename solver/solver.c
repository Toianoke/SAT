#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


void free_formula(Formula * f);
int is_unit_clause(Clause * c);
int array_contains(int *arr, int length, int item);
  
// TODO: make and use util version
void error_on(int bool)
{
  if (bool) {
    printf("ERROR\n");
    exit(0);
  }
}


int var_to_index(int v) {
  assert(v != 0);
  if (v < 0) {
    return (-2)*v - 1;
  } else {
    return 2*v - 2;
  }
}


int index_to_var(int i) {
  assert(i >= 0);
  if (i%2 == 1) {
    return -((i+1)/2);
  } else {
    return (i+2)/2;
  }
}


/**
 * TODO: comment is_consistent_literals
 */
int is_consistent_literals(Formula * f)
{
  assert(f != NULL);

  // Check the length of all clauses
  int i;
  for (i = 0; i < f->num_clauses; i++) {
    if (!is_unit_clause(&f->clauses[i])) {
      return 0;
    }
  }

  // check to see if the negation of any variable is present in f
  // If the code makes it to this point all clauses contain only one variable.
  for (i = 0; i < f->original_vars*2-1; i+=2) {
    if (f->var_list[i] && f->var_list[i+1]) {
      return 0;
    }
  }

  return 1;
}


/**
 * returns 1 if there is at least one clause in f
 *              that is empty
 *         0 otherwise
 */
int contains_empty_clause(Formula * f)
{
  assert(f != NULL);

  int i;
  for (i = 0; i < f->num_clauses; i++) {
    if (f->clauses[i].num_lits == 0) {
      return 1;
    }
  }

  return 0;
}


/**
 * returns 1 if c contains exactly 1 variable
 *         0 otherwise
 */
int is_unit_clause(Clause * c)
{
  return c->num_lits == 1;
}


/**
 * TODO: comment remove_var
 */
void remove_var(Formula * f, int ci, int idx)
{
  assert(f != NULL);
  assert(ci >= 0);
  assert(ci < f->num_clauses);
  assert(idx >= 0);
  assert(idx < f->clauses[ci].num_lits);

  int v = f->clauses[ci].literals[idx];
  int i = var_to_index(v);
  f->var_list[i] -= 1;
  f->clauses[ci].literals[idx] = f->clauses[ci].literals[f->clauses[ci].num_lits-1];
  f->clauses[ci].literals[f->clauses[ci].num_lits-1] = 0;
  f->clauses[ci].num_lits--;
}


/**
 * TODO: comment remove_clause
 */
void remove_clause(int idx, Formula * f)
{
  assert(f != NULL);
  assert(idx >= 0);
  assert(idx < f->num_clauses);

  Clause temp = f->clauses[idx];
  f->clauses[idx] = f->clauses[f->num_clauses-1];
  f->clauses[f->num_clauses-1] = temp;
  int i;
  for (i=0; i<temp.num_lits; i++) {
    int v = temp.literals[i];
    int j = var_to_index(v);
    f->var_list[j] -= 1;
  }
  f->num_clauses--;
}


/**
 * TODO: comment propagate_unit_clause
 */
int propagate_unit_clause(Formula * fn, int ci, int n)
{
  assert(fn != NULL);
  assert(ci >= 0);
  assert(n != 0);

  int i;
  for (i = 0; i < fn->clauses[ci].num_lits; i++) {
    if (fn->clauses[ci].literals[i] == n) {
      remove_clause(ci, fn);
      return 1;
    }
    else if (fn->clauses[ci].literals[i] + n == 0) {
      remove_var(fn, ci, i);
      return 0;
    }
  }
  
  return 0;
}


/**
 * TODO: comment propagat_unit_inplace
 */
void propagate_unit_inplace(Formula * f, int n)
{
  assert(f != NULL);
  assert(n != 0);

  int i;
  for (i = 0; i < f->num_clauses; i++) {
    if (propagate_unit_clause(f, i, n)) {
      i--;
    }
  }
}


/**
 * for each clause c in f:
 *   if n appears in c:
 *     remove c from f
 *   if -n appears in c:
 *     remove -n from c
 */
Formula * propagate_unit(Formula * f, int n)
{
  assert(f != NULL);
  assert(n != 0);

  Formula * fn = copy_formula(f);

  propagate_unit_inplace(fn, n);

  return fn;
}


/**
 * TODO: comment has_single_polarity
 */
int has_single_polarity(int v, Formula * f)
{
  assert(f != NULL);
  assert(v != 0);

  int i = var_to_index(v);
  int j = var_to_index(-v);
  return (!f->var_list[i] != !f->var_list[j]); // logical XOR
}


/**
 * Returns true if clause c contains variable v,
 * false, otherwise.
 */
int clause_contains(Clause * c, int v)
{
  assert(c != NULL);
  assert(v != 0);
  
  int i;
  for (i = 0; i < c->num_lits; i++) {
    if (c->literals[i] == v) {
      return 1;
    }
  }
  
  return 0;
}


/**
 * TODO: comment remove_clauses_containing
 */
void remove_clauses_containing(Formula * f, int v)
{
  assert(f != NULL);
  assert(v != 0);

  int i;
  for (i = 0; i < f->num_clauses; i++) {
    if (clause_contains(&f->clauses[i], v)) {
      remove_clause(i, f);
      i--;
    }
  }
}


/**
 * for any variable v in F with a single polarity
 *   remove every clause in which v occurs
 */
void eliminate_pure_literals(Formula * f)
{
  assert(f != NULL);

  int v;
  for (v = -(f->original_vars); v < f->original_vars+1; v++) {
    if ((v == 0) || (f->var_list[var_to_index(v)] == 0)) {
      continue;
    }
    
    if (has_single_polarity(v, f)) {
      remove_clauses_containing(f, v);
    }
  }
}


/**
 * return a variable that occurs in f
 */
int pick_var_from_formula(Formula *f)
{
  assert(f != NULL);
  int i;
  int imax = -1;
  int max = -1;
  for(i=0; i<f->original_vars*2; i++) {
    if (f->var_list[i] > max) {
      max = f->var_list[i];
      imax = i;
    }
  }
  return index_to_var(imax);
}


/**
 * returns a new Formula with nv variables
 * and nc clauses, where clauses is an array
 * of arrays of ints, representing each clause
 * in the formula
 */
Formula * create_formula(int nv, int nc, int **in_clauses)
{
  assert(in_clauses != NULL);
  assert(nv > 0);
  assert(nc > 0);
  
  int i, j, count;
  Clause *cp;

  Formula *f = malloc(sizeof(Formula));
  error_on(f == NULL);
  
  f->num_clauses = nc;
  f->original_clauses = nc;
  f->original_vars = nv;
  f->var_list = malloc(sizeof(int)*2*nv);
  error_on(f->var_list == NULL);
  memset(f->var_list, 0 , sizeof(int)*2*nv);
  
  f->clauses = malloc(sizeof(Clause)*nc);
  error_on(f->clauses == NULL);
  
  cp = f->clauses;

  for (i = 0; i < f->num_clauses; i++) {
    count = 0;
    while(in_clauses[i][count] != 0) {
      count++;
    }
    
    cp->num_lits = count;
    cp->literals = malloc(sizeof(int)*(count));
    error_on(cp->literals == NULL);
    
    for (j = 0; j < count; j++) {
      f->clauses[i].literals[j] = in_clauses[i][j];
      int k = var_to_index(in_clauses[i][j]);
      f->var_list[k] += 1;
    }
    
    cp++;
  }
  
  return f;
}


/**
 * TODO: comment copy_formula
 */
Formula* copy_formula(Formula *f)
{
  assert(f != NULL);
  
  Formula * new_f = malloc(sizeof(Formula));
  error_on(new_f == NULL);

  new_f->num_clauses = f->num_clauses;
  new_f->original_clauses = f->num_clauses;
  new_f->original_vars = f->original_vars;
  
  new_f->var_list = malloc(sizeof(int)*f->original_vars*2);
  error_on(new_f->var_list == NULL);

  memcpy(new_f->var_list, f->var_list, sizeof(int)*f->original_vars*2);
  
  new_f->clauses = malloc(sizeof(Clause)*f->num_clauses);
  error_on(new_f->clauses == NULL);

  int i;
  for (i = 0; i < new_f->num_clauses; i++) {
    Clause * cp = &f->clauses[i];
    Clause * new_cp = &new_f->clauses[i];
    new_cp->num_lits = cp->num_lits;

    new_cp->literals = malloc(sizeof(int)*(cp->num_lits));
    error_on(new_cp->literals == NULL);
    
    memcpy(new_cp->literals, cp->literals, sizeof(int)*(cp->num_lits));
  }
  
  return new_f;
}


/**
 * TODO: comment free_formula
 */
void free_formula(Formula *f)
{
  assert(f != NULL);
  
  int i;
  for (i=0; i<f->original_clauses; i++) {
    free(f->clauses[i].literals);
    f->clauses[i].literals = NULL;
  }
  free(f->clauses);
  f->clauses = NULL;
  
  free(f->var_list);
  f->var_list = NULL;

  free(f);
  f = NULL;
}


/**
 * TODO: comment dpll
 */
int dpll(Formula *F)
{
  assert(F != NULL);

  Formula * original = F;
  
  if (is_consistent_literals(F)) {
    return 1;
  }
  
  if (contains_empty_clause(F)) {
    return 0;
  }

  int i;
  for (i=0; i<F->num_clauses; i++) {
    if (is_unit_clause(&F->clauses[i])) {
      if (F == original) {
	F = copy_formula(original);
      }
      propagate_unit_inplace(F, F->clauses[i].literals[0]);
    }
  }

  eliminate_pure_literals(F);


  /* pick one variable v */
  int v = pick_var_from_formula(F);

  if (v == 0) {
    if (F != original) {
      free_formula(F);      
    }
    return 0;
  }

  Formula * temp = F;
  F = propagate_unit(temp, v);
  int retval = dpll(F);
  free_formula(F);

  if (!retval) {
    F = propagate_unit(temp, ((-1)*v));
    retval = dpll(F);
    free_formula(F);
  }

  if (temp != original) {
    free_formula(temp);
  }
  return retval;
  //return dpll(propagate_unit(F, v)) || dpll(propagate_unit(F, ((-1)*v)));
}


int clause_compare(const void* left, const void* right)
{
  return ((Clause *)right)->num_lits - ((Clause *) left)->num_lits;
}

/**
 * TODO: comment solve
 */
int solve(int nv, int nc, int **clauses)
{
  assert(clauses != NULL);
  assert(nv > 0);
  assert(nc > 0);

  Formula *f = create_formula(nv, nc, clauses);
  qsort(f->clauses, nc, sizeof(Clause), clause_compare);
  int retval = dpll(f);
  free_formula(f);
  
  return retval;
}
