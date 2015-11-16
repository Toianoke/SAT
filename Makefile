CFLAGS := -Wall #-Werror


sat: solver/formula.h solver/solver.c solver/solver.h io/input_alt.c
	$(CC) $(CFLAGS) -I solver solver/solver.c io/input_alt.c -o sat

.PHONY: clean
clean:
	$(RM) ~*
	$(RM) \#*\#
	$(RM) sat
