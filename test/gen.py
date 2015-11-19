#!/usr/bin/env python3
import random as R
import sys
import os

def gen_clause(nbvar, terms, var_list):
    assert(nbvar >= 1)
    assert(terms <= nbvar)
    R.shuffle(var_list)
    return var_list[:terms] + [0]

def gen_clauses(nbvar, nbclauses):
    assert(nbvar >= 1)
    var_list = list(range(1, nbvar+1))
    for i in range(nbclauses):
        yield gen_clause(nbvar, R.randint(1, nbvar), var_list)


def gen_test_strings(nbvar, nbclauses):
    yield "p cnf {} {}".format(nbvar, nbclauses)
    for clause in gen_clauses(nbvar, nbclauses):
        yield ' '.join(map(str, clause))


def gen_test(nbvar, nbclauses, filename):
    with open(filename, "w") as f:
        for line in gen_test_strings(nbvar, nbclauses):
            f.write(line+"\n")


if __name__ == "__main__":
    try:
        os.chdir(sys.argv[1])
        tests = int(sys.argv[2])
        lowvars = int(sys.argv[3])
        highvars = int(sys.argv[4])
        assert(lowvars <= highvars)
    except:
        print("use: {} dir tests lowvars highvars".format(argv[0]))

    R.seed(42)
    for i in range(tests):
        print(".",end='')
        if i%80 == 79:
            print("")
        v = R.randint(lowvars, highvars)
        gen_test(v, min(int(5.4*v), 65535), "test_{:05}".format(i))
    print("")

