EIGENFLAGS := $(shell Rscript -e "RcppEigen:::CxxFlags()")

MPICXX = mpicxx
CXXFLAGS = -Wall -pedantic -Wextra $(EIGENFLAGS)

all: clean test

test:
	$(MPICXX) $(CXXFLAGS) test.cpp -o test

clean:
	rm -f test
