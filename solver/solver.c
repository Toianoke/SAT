#include "formula.h"
#include "solver.h"

/*
 * returns 1 if f is composed only of
 * literals that are consistent
 * i.e. all clauses contain 1 variable and
 *      no two clauses contain a variable and its negation
 */
int is_consistent_literals(Formula *f){
  // TODO
  return 0;
}

/*
 * returns 1 if there is at least one clause in f
 *              that is empty
 *         0 otherwise
 */
int contains_empty_clause(Formula *f){
  // TODO
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

  for(i=0; i<MAX_CLAUSE_CNT/*change to length of F*/; i++)
    if(is_unit_clause(F->clauses[i]))
      propagate_unit(F, F->clauses->variables[0]);

  eliminate_pure_literals(F);
  
  /* pick one variable v */
  short v = pick_var_from_formula(F);
  return dpll(propagate_unit(F, v)) || dpll(propagate_unit(F, ((-1)*v)));
}
