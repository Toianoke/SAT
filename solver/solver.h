#include "formula.h"

int is_consistent_literals(Formula*);

int contains_empty_clause(Formula*);

int is_unit_clause(Clause);

Formula* propagate_unit(Formula*, int);

void eliminate_pure_literal(Formula*);

int pick_var_from_formula(Formula*);

int dpll(Formula*);
