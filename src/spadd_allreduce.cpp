#include <Eigen/SparseCore>

#include <cstdint>
#include <iostream>
#include <Rdefines.h>
#include <Rinternals.h>

#include "cxxutils.hpp"
#include "utils.h"

typedef uint32_t scalar_t;
typedef int index_t;
typedef Eigen::SparseMatrix<scalar_t, Eigen::StorageOptions::ColMajor, index_t> spmat;

static inline scalar_t* spmat_X(spmat &s){return (scalar_t*) s.valuePtr();}
static inline index_t* spmat_I(spmat &s){return (index_t*) s.innerIndexPtr();}
static inline index_t* spmat_P(spmat &s){return (index_t*) s.outerIndexPtr();}



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP R_comm)
{
  return R_NilValue;
}
