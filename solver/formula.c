#define MAX_CLAUSE_CNT 65536
#define MAX_VAR_VNT    65536

struct Clause{
  int variables[MAX_VAR_CNT];
}

struct Formula{
  Clause clauses[MAX_CLAUSE_CNT];
}
