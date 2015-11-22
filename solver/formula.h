#ifndef FORMULA_H
#define FORMULA_H

#define MAX_CLAUSE_CNT 65536
#define MAX_VAR_CNT    65536

typedef struct{
	int num_lits;
	int *literals;
}Clause;

typedef struct{
	int num_clauses;
	int original_clauses;
	Clause *clauses;
	
	//var_list - list of all variables and their negations if they occur
	//does not have any duplicates.
	int *var_list;
	int vl_length;
	int original_vars;
	int lits_rebuild_needed;
}Formula;

Formula *create_formula(int, int, int**);
Formula* copy_formula(Formula*);

#endif
