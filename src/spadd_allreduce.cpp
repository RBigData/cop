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


template <typename T>
void arrcopy(const index_t len, const T *src, T *dst)
{
  std::memcpy(dst, src, len*sizeof(*src));
}

template <typename SRC, typename DST>
void arrcopy(const index_t len, const SRC *src, DST *dst)
{
  #pragma omp for simd
  for (index_t i=0; i<len; i++)
    dst[i] = (DST) src[i];
}



static inline spmat s4_to_spmat(SEXP s4)
{
  SEXP dim = GET_SLOT(s4, install("Dim"));
  const int m = INTEGER(dim)[0];
  const int n = INTEGER(dim)[1];
  
  SEXP s4_X = GET_SLOT(s4, install("x"));
  const int nnz = LENGTH(s4_X);
  
  SEXP s4_I = GET_SLOT(s4, install("i")); // len == nnz
  
  SEXP s4_P = GET_SLOT(s4, install("p"));
  const int P_len = LENGTH(s4_P);
  
  spmat s(m, n);
  s.makeCompressed();
  s.resizeNonZeros(nnz);
  
  arrcopy(nnz, REAL(s4_X), spmat_X(s));
  arrcopy(nnz, INTEGER(s4_I), spmat_I(s));
  arrcopy(P_len, INTEGER(s4_P), spmat_P(s));
  
  return s;
}



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP R_comm)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  spmat send_data = s4_to_spmat(send_data_s4);
  std::cout << send_data << std::endl;
  
  return R_NilValue;
}
