#include <Eigen/SparseCore>

#include <cstdint>
#include <iostream>
#include <Rdefines.h>
#include <Rinternals.h>



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP R_comm)
{
  return R_NilValue;
}
