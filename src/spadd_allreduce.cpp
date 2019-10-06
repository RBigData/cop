#include "cxxutils.hpp"
#include "defs.h"
#include "utils.h"


#include <cstdint>
#include <Rdefines.h>
#include <Rinternals.h>

#include "spvec/src/arraytools.hpp"
#include "spvec/src/spmat.hpp"
#include "spvec/src/spvec.hpp"

typedef int index_t;
#define MPI_INDEX_T MPI_INT
typedef uint32_t scalar_t;
#define MPI_SCALAR_T MPI_UINT32_T



namespace sparsehelpers
{
  namespace constants
  {
    const static float MEM_FUDGE_ELT_FAC = 1.675;
    const static int MIN_LEN = 1;
  }
  
  
  
  namespace sexp
  {
    namespace
    {
      static inline SEXP get_obj_from_s4(SEXP s4, const char *obj)
      {
        return GET_SLOT(s4, install(obj));
      }
    }
    
    
    
    static inline SEXP get_x_from_s4(SEXP s4)
    {
      return get_obj_from_s4(s4, "x");
    }
    
    static inline SEXP get_i_from_s4(SEXP s4)
    {
      return get_obj_from_s4(s4, "i");
    }
    
    static inline SEXP get_p_from_s4(SEXP s4)
    {
      return get_obj_from_s4(s4, "p");
    }
    
    static inline void get_dim_from_s4(SEXP s4, int *m, int *n)
    {
      SEXP dim = get_obj_from_s4(s4, "Dim");
      *m = INTEGER(dim)[0];
      *n = INTEGER(dim)[1];
    }
    
    static inline int get_nnz_from_s4(SEXP s4_I)
    {
      return LENGTH(s4_I);
    }
    
    static inline int get_plen_from_s4(SEXP s4_P)
    {
      return LENGTH(s4_P);
    }
    
    static inline int get_col_len_from_s4(int col_ind, SEXP s4_P)
    {
      return INTEGER(s4_P)[col_ind+1] - INTEGER(s4_P)[col_ind];
    }
  }
  
  
  
  template <typename INDEX, typename SCALAR>
  static inline void s4col_to_spvec(const int col_ind, SEXP s4, spvec<INDEX, SCALAR> &s)
  {
    SEXP s4_X = sexp::get_x_from_s4(s4);
    SEXP s4_I = sexp::get_i_from_s4(s4); // len == nnz
    SEXP s4_P = sexp::get_p_from_s4(s4);
    
    const int start_ind = INTEGER(s4_P)[col_ind];
    const int col_len = INTEGER(s4_P)[col_ind+1] - start_ind;
    
    s.set(col_len, INTEGER(s4_I) + start_ind, REAL(s4_X) + start_ind);
  }
  
  template <typename INDEX, typename SCALAR>
  static inline spvec<INDEX, SCALAR> s4col_to_spvec(const int col_ind, SEXP s4)
  {
    SEXP s4_P = sexp::get_p_from_s4(s4);
    const int col_len = sexp::get_col_len_from_s4(col_ind, s4_P);
    
    spvec<INDEX, SCALAR> s(col_len * constants::MEM_FUDGE_ELT_FAC);
    s4col_to_spvec(col_ind, s4, s);
    return s;
  }
  
  
  
  template <typename INDEX, typename SCALAR>
  static inline SEXP spmat_to_s4(const spmat<INDEX, SCALAR> &s)
  {
    SEXP s4_class, s4;
    SEXP s4_i, s4_p, s4_Dim, s4_Dimnames, s4_x, s4_factors;
    
    const index_t m = s.nrows();
    const index_t n = s.ncols();
    const index_t nnz = s.get_nnz();
    const index_t p_len = n+1;
    
    PROTECT(s4_i = allocVector(INTSXP, nnz));
    arraytools::copy(nnz, s.index_ptr(), INTEGER(s4_i));
    
    PROTECT(s4_p = allocVector(INTSXP, p_len));
    arraytools::copy(p_len, s.col_ptr(), INTEGER(s4_p));
    
    PROTECT(s4_Dim = allocVector(INTSXP, 2));
    INTEGER(s4_Dim)[0] = (int) m;
    INTEGER(s4_Dim)[1] = (int) n;
    
    PROTECT(s4_Dimnames = allocVector(VECSXP, 2));
    SET_VECTOR_ELT(s4_Dimnames, 0, R_NilValue);
    SET_VECTOR_ELT(s4_Dimnames, 1, R_NilValue);
    
    PROTECT(s4_x = allocVector(REALSXP, nnz));
    arraytools::copy(nnz, s.data_ptr(), REAL(s4_x));
    
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
}



spvec<index_t, scalar_t> _a;
spvec<index_t, scalar_t> _b;

typedef struct
{
  index_t *i;
  scalar_t *x;
} spvec_struct_t;

void custom_op_spadd(void *a, void *b, int *len, MPI_Datatype *dtype)
{
  (void)a;
  (void)b;
  (void)len;
  (void)dtype;
  
  int check = _b.add(_a);
}



static inline SEXP spadd_allreduce(const int root, SEXP send_data_s4, MPI_Comm comm)
{
  int mpi_ret;
  int m, n;
  sparsehelpers::sexp::get_dim_from_s4(send_data_s4, &m, &n);
  
  const int initial_len = sparsehelpers::sexp::get_col_len_from_s4(0, sparsehelpers::sexp::get_p_from_s4(send_data_s4)) * sparsehelpers::constants::MEM_FUDGE_ELT_FAC;
  spvec<index_t, scalar_t> v(initial_len);
  spmat<index_t, scalar_t> s(m, n, n * initial_len);
  
  for (index_t j=0; j<n; j++)
  {
    sparsehelpers::s4col_to_spvec(j, send_data_s4, v);
    // if (root == REDUCE_TO_ALL)
    //   mpi_ret = MPI_Allreduce(MPI_IN_PLACE, tmp, m, MPI_SCALAR_T, MPI_SUM, comm);
    // else
    //   mpi_ret = MPI_Reduce(MPI_IN_PLACE, tmp, m, MPI_SCALAR_T, MPI_SUM, root, comm);
    
    
    int needed_space = s.insert(j, v);
    if (needed_space > 0)
    {
      s.resize((s.get_len() + needed_space) * sparsehelpers::constants::MEM_FUDGE_ELT_FAC);
      s.insert(j, v);
    }
  }
  
  s.print(true);
  
  SEXP recv_data;
  PROTECT(recv_data = sparsehelpers::spmat_to_s4(s));
  UNPROTECT(1);
  return recv_data;
}



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP root, SEXP R_comm)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  SEXP ret;
  PROTECT(ret = spadd_allreduce(INTEGER(root)[0], send_data_s4, comm));
  UNPROTECT(1);
  return ret;
}
