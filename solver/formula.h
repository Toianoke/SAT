#define MAX_CLAUSE_CNT 65536
#define MAX_VAR_CNT    65536
/* talk to the other dudes,
 * do we know how many clauses we will have?
 * and do we know how many variables in each clause?
 */
/*
typedef struct{
  short variables[MAX_VAR_CNT / 2];
}Clause;

typedef struct{
  Clause clauses[MAX_CLAUSE_CNT / 4];
  short all_vars[MAX_VAR_CNT*2];
}Formula;
*/

typedef struct{
	short num_lits;
	short *literals;
}Clause;

typedef struct{
	short num_clauses;
	Clause *clauses;
	//var_list - list of all variables and their negations if they occur
	//does not have any duplicates.
	short *var_list;
	short vl_length;
}Formula;

Formula *create_formula(short, short, short**);
