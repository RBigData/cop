MPICC = mpicc
CFLAGS = -Wall -pedantic -Wextra

all: clean test

test:
	$(MPICC) $(CFLAGS) test.c -o test

clean:
	rm -f test
