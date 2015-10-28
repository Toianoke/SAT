/*
 * How will we represent:
 *   > a clause
 *   > a formula (set of clauses)
 */

int dpll(/* what format will the problem be in? */){

  propagate_units();

  eliminate_pure_literals();

  
}
