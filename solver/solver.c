#include "formula.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


static void unknown_on(int bool);
static int var_to_index(int v);
static int index_to_var(int i);
static int is_consistent_literals(Formula * f);
static int contains_empty_clause(Formula * f);
static int is_unit_clause(Clause * c);
static void remove_var(Formula * f, int ci, int idx);
static void remove_clause(int idx, Formula * f);
static int propagate_unit_clause(Formula * fn, int ci, int n);
static void propagate_unit(Formula * f, int n);
static int has_single_polarity(int v, Formula * f);
static int clause_contains(Clause * c, int v);
static void remove_clauses_containing(Formula * f, int v);
static void eliminate_pure_literals(Formula * f);
static int pick_var_from_formula(Formula *f);
static Formula * create_formula(int nv, int nc, int **in_clauses);
static void free_formula(Formula *f);
static int dpll(Formula *F, int level);
static int clause_compare(const void* left, const void* right);


static void
add_var(Formula * f, int v)
{
  int i = var_to_index(v);
  f->var_list[i]++;
}


static void
reset_decision_level_clause(Formula * f, Clause * c, int level)
{
  int i, j;
  for (i=c->stack_level; i>=0; i--) {
    if (c->num_lits_stack[i].level <= level) {
      break;
    }
  }
  for (j=c->num_lits; j<c->num_lits_stack[i].length; j++) {
    add_var(f, c->literals[j]);
  }
  c->num_lits = c->num_lits_stack[i].length;
  c->stack_level = i;
}


static void
reset_decision_level(Formula * f, int level)
{
  int i,j,k;
  for (i=f->stack_level; i>=0; i--) {
    if (f->num_clauses_stack[i].level <= level) {
      break;
    } 
  }
  for(k=f->num_clauses; k<f->num_clauses_stack[i].length; k++) {
    for (j=0; j<f->clauses[k].num_lits; j++) {
      add_var(f, f->clauses[k].literals[j]);
    }
  }
  f->num_clauses = f->num_clauses_stack[i].length;
  f->stack_level = i;
  
  for (i=0; i<f->num_clauses; i++) {
    reset_decision_level_clause(f, &f->clauses[i], level);
  }
}

static void
set_decision_level_clause(Clause * c, int level)
{
  if (c->num_lits_stack[c->stack_level].length != c->num_lits) {
    c->stack_level++;
    c->num_lits_stack[c->stack_level].level = level;
    c->num_lits_stack[c->stack_level].length = c->num_lits;
  }
}

static void
set_decision_level(Formula * f, int level)
{
  if (f->num_clauses_stack[f->stack_level].length != f->num_clauses) {
    f->stack_level++;
    f->num_clauses_stack[f->stack_level].level = level;
    f->num_clauses_stack[f->stack_level].length = f->num_clauses;
  }

  int i;
  for (i = 0; i<f->num_clauses; i++) {
    set_decision_level_clause(&(f->clauses[i]), level);
  }
  
}
  
// TODO: make and use util version
static void unknown_on(int bool)
{
  if (bool) {
    printf("UNKNOWN\n");
    exit(0);
  }
}


static int var_to_index(int v)
{
  assert(v != 0);
  if (v < 0) {
    return (-2)*v - 1;
  } else {
    return 2*v - 2;
  }
}


static int index_to_var(int i)
{
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
static int is_consistent_literals(Formula * f)
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
static int contains_empty_clause(Formula * f)
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
static int is_unit_clause(Clause * c)
{
  return c->num_lits == 1;
}


/**
 * TODO: comment remove_var
 */
static void remove_var(Formula * f, int ci, int idx)
{
  assert(f != NULL);
  assert(ci >= 0);
  assert(ci < f->num_clauses);
  assert(idx >= 0);
  assert(idx < f->clauses[ci].num_lits);

  int v = f->clauses[ci].literals[idx];
  int i = var_to_index(v);
  f->var_list[i] -= 1;
  int temp =   f->clauses[ci].literals[idx];
  f->clauses[ci].literals[idx] = f->clauses[ci].literals[f->clauses[ci].num_lits-1];
  f->clauses[ci].literals[f->clauses[ci].num_lits-1] = temp;
  f->clauses[ci].num_lits--;
}


/**
 * TODO: comment remove_clause
 */
static void remove_clause(int idx, Formula * f)
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
static int propagate_unit_clause(Formula * fn, int ci, int n)
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
static void propagate_unit(Formula * f, int n)
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
 * TODO: comment has_single_polarity
 */
static int has_single_polarity(int v, Formula * f)
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
static int clause_contains(Clause * c, int v)
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
static void remove_clauses_containing(Formula * f, int v)
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
static void eliminate_pure_literals(Formula * f)
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
static int pick_var_from_formula(Formula *f)
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
static Formula * create_formula(int nv, int nc, int **in_clauses)
{
  assert(in_clauses != NULL);
  assert(nv > 0);
  assert(nc > 0);
  
  int i, j, count;
  Clause *cp;

  Formula *f = malloc(sizeof(Formula));
  unknown_on(f == NULL);
  
  f->num_clauses_stack = malloc(sizeof(decision)*(nc));
  unknown_on(f->num_clauses_stack == NULL);
  f->stack_level = 0;
  f->num_clauses_stack[0].level = -1;
  f->num_clauses_stack[0].length = nc;
  f->num_clauses = nc;
  
  f->original_vars = nv;
  f->var_list = malloc(sizeof(int)*2*nv);
  unknown_on(f->var_list == NULL);
  memset(f->var_list, 0 , sizeof(int)*2*nv);
  
  f->clauses = malloc(sizeof(Clause)*nc);
  unknown_on(f->clauses == NULL);
  
  cp = f->clauses;

  for (i = 0; i < f->num_clauses; i++) {
    count = 0;
    while(in_clauses[i][count] != 0) {
      count++;
    }
    
    cp->num_lits_stack = malloc(sizeof(decision)*(count));
    unknown_on(cp->num_lits_stack == NULL);
    cp->stack_level = 0;
    cp->num_lits_stack[0].level = -1;
    cp->num_lits_stack[0].length = count;
    cp->num_lits = count;
    
    
    cp->literals = malloc(sizeof(int)*(count));
    unknown_on(cp->literals == NULL);
    
    for (j = 0; j < count; j++) {
      f->clauses[i].literals[j] = in_clauses[i][j];
      int k = var_to_index(in_clauses[i][j]);
      f->var_list[k] += 1;
    }
    
    cp++;
  }

  qsort(f->clauses, nc, sizeof(Clause), clause_compare);
  return f;
}


/**
 * TODO: comment free_formula
 */
static void free_formula(Formula *f)
{
  assert(f != NULL);
  
  int i;
  for (i=0; i<f->num_clauses_stack[0].length; i++) {
    free(f->clauses[i].literals);
    f->clauses[i].literals = NULL;
    free(f->clauses[i].num_lits_stack);
    f->clauses[i].num_lits_stack = NULL;
  }
  free(f->clauses);
  f->clauses = NULL;

  free(f->num_clauses_stack);
  f->num_clauses_stack = NULL;
  
  free(f->var_list);
  f->var_list = NULL;

  free(f);
  f = NULL;
}


/**
 * TODO: comment dpll
 */
static int dpll(Formula *F, int level)
{
  assert(F != NULL);
  
  if (is_consistent_literals(F)) {
    return 1;
  }
  
  if (contains_empty_clause(F)) {
    return 0;
  }

  int i;
  for (i=0; i<F->num_clauses; i++) {
    if (is_unit_clause(&F->clauses[i])) {
      propagate_unit(F, F->clauses[i].literals[0]);
    }
  }

  eliminate_pure_literals(F);

  set_decision_level(F, level);
  
  /* pick one variable v */
  int v = pick_var_from_formula(F);

  propagate_unit(F, v);
  if (dpll(F, level+1)) {
    return 1;
  }

  reset_decision_level(F, level);
  propagate_unit(F, ((-1)*v));
  return dpll(F, level+1);
  //return dpll(propagate_unit(F, v)) || dpll(propagate_unit(F, ((-1)*v)));
}


static int clause_compare(const void* left, const void* right)
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
  int retval = dpll(f, 0);
  free_formula(f);
  
  return retval;
}
