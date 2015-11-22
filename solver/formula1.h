#ifndef FORMULA_H
#define FORMULA_H

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
typedef struct Var{
	short frequency;
	short value;
}Var;

typedef struct{
	short num_lits;
	short *literals;
}Clause;

typedef struct{
	short num_clauses;
	Clause *clauses;
	//var_list - list of the frequencies that variables occur. Index of the array is
	//corresponds to the value of the var and the number stored there is the
	//it occurs in the formula. The first half of the array is filled with + values
	//The second half is negative. 
	short *var_list;
	short vl_length;
	/*
	 *sorted_var_list is sorted by the frequency a variable occurs so 
	 */
	Var *sorted_var_list;
	short svl_length;
}Formula;



Formula *create_formula(short, short, short**);

#endif
