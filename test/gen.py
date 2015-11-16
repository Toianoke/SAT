#!/usr/bin/env python3
import random as R


def gen_clause(nbvar, terms):
    assert(nbvar >= 1)
    assert(terms <= nbvar)
    clause = list()
    while len(clause) < terms:
        c = R.randint(-nbvar, nbvar)
        if c in clause or -c in  clause or c == 0:
            continue
        clause.append(c)
    clause.append(0)
    return clause

def gen_clauses(nbvar, nbclauses):
    assert(nbvar >= 1)
    clauses = list()

    for i in range(nbclauses):
        clauses.append(gen_clause(nbvar, R.randint(1, nbvar)))
    return clauses

def gen_assignments(nbvars):
    for i in range(2**nbvars):
        bin_str = bin(i)[2:]
        bin_str = bin_str + '0'*(nbvars-len(bin_str))
        assignment = ["INVALID"] + [bool(bin_str[j]) for j in range(nbvars)]
        yield assignment

def check_clause(clause, assignment):
    for term in clause[:-1]:
        if term > 0:
            res = assignment[term]
        else:
            res = not assignment[-term]
        if res:
            return True
    return False

def gen_test_string(nbvars, nbclauses):
    strings = list()
    clauses = gen_clauses(nbvars, nbclauses)
    strings.append("p cnf {} {}".format(nbvars, nbclauses))
    clause_strings = [' '.join(map(str, clause)) for clause in clauses]
    strings.extend(clause_strings)
    return '\n'.join(strings)

def gen_test(nbvars, nbclauses, filename=None):
    test = gen_test_string(nbvars, nbclauses)
    if filename==None:
        print(test)
    else:
        with open(filename, "w") as f:
            f.write(test)
        
if __name__ == "__main__":
    R.seed(42)
    for i in range(10000):
        v = R.randint(10,20)
        gen_test(v,
                 min(int(5.4*v), 65535),
                 "test_{:05}.cnf".format(i))
