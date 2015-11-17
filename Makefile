CFLAGS := -Wall -g -Og # -O3 -march=native -flto  


sat: solver/solver.h solver/formula.h solver/solver.c io/solver_frontend.c
	$(CC) $(CFLAGS) -I solver solver/solver.c io/solver_frontend.c -o sat

.PHONY: test
test: sat
	cd test && ./run_tests.py

.PHONY: clean
clean:
	$(RM) ~*
	$(RM) \#*\#
	$(RM) sat
