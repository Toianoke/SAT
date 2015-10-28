#define MAX_CLAUSE_CNT 65536
#define MAX_VAR_CNT    65536
/* talk to the other dudes,
 * do we know how many clauses we will have?
 * and do we know how many variables in each clause?
 */

typedef struct{
  int variables[MAX_VAR_CNT / 4];
}Clause;

typedef struct{
  Clause clauses[MAX_CLAUSE_CNT / 4];
  int all_vars[MAX_VAR_CNT*2];
}Formula;
