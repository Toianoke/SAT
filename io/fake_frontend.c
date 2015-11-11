#include <stdio.h>
#include "solver.h"

int
main()
{
  short * cl0[4] = {1, -5, 6, 0};
  short * cl1[5] = {-1, 5, 3, 4, 0};
  short * cl2[3] = {-3, -4, 0};
  short * cl3[5] = {-1, 4, 5, -6, 0};
  short ** clauses[4] = {cl1, cl2, cl3, cl4};
  
  if (solve(6, 4, clauses)) {
    printf("SATISFIABLE");
  } else {
    printf("UNSATISFIABLE");
  }
  
}
