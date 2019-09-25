#include <Eigen/SparseCore>

#include <cstdint>
#include <Rdefines.h>
#include <Rinternals.h>


namespace eigenhelpers
{
  typedef uint32_t scalar_t;
  #define MPI_SCALAR_T MPI_UINT32_T
  typedef int index_t;
  #define MPI_INDEX_T MPI_INT
  
  typedef Eigen::SparseMatrix<scalar_t, Eigen::StorageOptions::ColMajor, index_t> spmat;
  
  static inline scalar_t* spmat_X(spmat &s) {return (scalar_t*) s.valuePtr();}
  static inline  index_t* spmat_I(spmat &s) {return  (index_t*) s.innerIndexPtr();}
  static inline  index_t* spmat_P(spmat &s) {return  (index_t*) s.outerIndexPtr();}
  
  static inline const scalar_t* spmat_X(const spmat &s) {return (scalar_t*) s.valuePtr();}
  static inline const  index_t* spmat_I(const spmat &s) {return  (index_t*) s.innerIndexPtr();}
  static inline const  index_t* spmat_P(const spmat &s) {return  (index_t*) s.outerIndexPtr();}
  
  
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
  
  
  
  static inline SEXP spmat_to_s4(spmat &s)
  {
    SEXP s4_class, s4;
    SEXP s4_i, s4_p, s4_Dim, s4_Dimnames, s4_x, s4_factors;
    
    const index_t m = s.rows();
    const index_t n = s.cols();
    const index_t nnz = s.nonZeros();
    const index_t p_len = s.outerSize();
    
    PROTECT(s4_i = allocVector(INTSXP, nnz));
    arrcopy(nnz, spmat_I(s), INTEGER(s4_i));
    
    PROTECT(s4_p = allocVector(INTSXP, p_len+1));
    arrcopy(p_len, spmat_P(s), INTEGER(s4_p));
    INTEGER(s4_p)[p_len] = nnz;
    
    PROTECT(s4_Dim = allocVector(INTSXP, 2));
    INTEGER(s4_Dim)[0] = (int) m;
    INTEGER(s4_Dim)[1] = (int) n;
    
    PROTECT(s4_Dimnames = allocVector(VECSXP, 2));
    SET_VECTOR_ELT(s4_Dimnames, 0, R_NilValue);
    SET_VECTOR_ELT(s4_Dimnames, 1, R_NilValue);
    
    PROTECT(s4_x = allocVector(REALSXP, nnz));
    arrcopy(nnz, spmat_X(s), REAL(s4_x));
    
    PROTECT(s4_factors = allocVector(VECSXP, 0));
    
    PROTECT(s4_class = MAKE_CLASS("dgCMatrix"));
    PROTECT(s4 = NEW_OBJECT(s4_class));
    SET_SLOT(s4, install("i"), s4_i);
    SET_SLOT(s4, install("p"), s4_p);
    SET_SLOT(s4, install("Dim"), s4_Dim);
    SET_SLOT(s4, install("Dimnames"), s4_Dimnames);
    SET_SLOT(s4, install("x"), s4_x);
    SET_SLOT(s4, install("factors"), s4_factors);
    
    UNPROTECT(8);
    return s4;
  }
  
  
  
  static inline void sparsecol_to_densecol(index_t col, SEXP s, scalar_t *d)
  {
    SEXP dim = GET_SLOT(s, install("Dim"));
    const int m = INTEGER(dim)[0];
    const int n = INTEGER(dim)[1];
    
    SEXP s_X = GET_SLOT(s, install("x"));
    const double *X = REAL(s_X);
    const int nnz = LENGTH(s_X);
    
    SEXP s_I = GET_SLOT(s, install("i")); // len == nnz
    const int *I = INTEGER(s_I);
    
    SEXP s_P = GET_SLOT(s, install("p"));
    const int *P = INTEGER(s_P);
    
    memset(d, 0, m*sizeof(*d));
    
    index_t start = P[col];
    index_t end = (col == n) ? nnz : P[col+1];
    for (index_t i=start; i<end; i++)
      d[ I[i] ] = (scalar_t) X[i];
  }
  
  
  
  static inline void sparsecol_to_densecol(index_t col, const spmat &s, scalar_t *d)
  {
    const index_t m = s.rows();
    const index_t n = s.cols();
    const index_t nnz = s.nonZeros();
    
    const scalar_t *X = spmat_X(s);
    const index_t *I = spmat_I(s);
    const index_t *P = spmat_P(s);
    
    memset(d, 0, m*sizeof(*d));
    
    index_t start = P[col];
    index_t end = (col == n) ? nnz : P[col+1];
    for (index_t i=start; i<end; i++)
      d[ I[i] ] = X[i];
  }
}
