#ifndef FORMULA_H
#define FORMULA_H

#define MAX_CLAUSE_CNT 65536
#define MAX_VAR_CNT    65536

typedef struct {
  int level;
  int length;
} decision;

typedef struct{
  int num_lits;
  int stack_level;
  decision * num_lits_stack;
  int * literals;
}Clause;

typedef struct{
	Clause *clauses;
	int num_clauses;
	int stack_level;
	decision * num_clauses_stack;
	//var_list - list of all variables and their negations if they occur
	//does not have any duplicates.
	int *var_list;
	int original_vars;
}Formula;



#endif
