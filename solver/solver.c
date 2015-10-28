/*
 * var = 0 -> ignore var and val
 * otherwise,
 *   var is a variable in F,
 *   and val is the boolean value that all 
 *   instances of var will take
 */

/*
 * returns 1 if f is composed only of
 * literals that are consistent
 * i.e. all clauses contain 1 variable and
 *      no two clauses contain a variable and its negation
 */
int is_consistent_literals(Formula f){
  // TODO
  return 0;
}

/*
 * returns 1 if there is at least one clause in f
 *              that is empty
 *         0 otherwise
 */
int contains_empty_clause(Formula f){
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
Formula propagate_unit(Formula f, int i){
  // TODO
}

/*
 * for any variable v in F with a single polarity
 *   remove every clause in which v occurs
 */
Formula eliminate_pure_literals(Formula f){
  // TODO
}

/*
 * return a variable that occurs in f
 */
int pick_var_from_formula(Formula f){
  // TODO
}

int dpll(Formula F){

  if(is_consistent_literals(F))
    return 1;
  if(contains_empty_clause(F))
    return 0;

  for(i=0; i<MAX_CLAUSE_CNT/*change to length of F*/; i++)
    if(is_unit_clause(F[i])
       F = propagate_unit(F, F[i][0]);

  F = eliminate_pure_literals(F);
  
  /* pick one variable v */
  int v = pick_var_from_formula(F);
       return dpll(propagate_unit(v, F)) || dpll(propagate_unit(((-1)*v), F));
}
