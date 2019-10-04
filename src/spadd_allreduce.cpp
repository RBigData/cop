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
    const int col_len = (INTEGER(s4_P)[col_ind+1]-1) - start_ind;
    
    s.set(col_len, INTEGER(s4_I) + start_ind, REAL(s4_X) + start_ind);
  }
  
  template <typename INDEX, typename SCALAR>
  static inline spvec<INDEX, SCALAR> s4col_to_spvec(const int col_ind, SEXP s4)
  {
    SEXP s4_P = sexp::get_p_from_s4(s4);
    const int col_len = INTEGER(s4_P)[col_ind+1] - INTEGER(s4_P)[col_ind];
    
    spvec<INDEX, SCALAR> s(col_len * constants::MEM_FUDGE_ELT_FAC);
    s4col_to_spvec(col_ind, s4, s);
    return s;
  }
}



/*
static inline int spadd_allreduce(const int root, SEXP send_data_s4, MPI_Comm comm)
{
  int mpi_ret;
  
  for (index_t j=0; j<n; j++)
  {
    
    if (root == REDUCE_TO_ALL)
      mpi_ret = MPI_Allreduce(MPI_IN_PLACE, tmp, m, MPI_SCALAR_T, MPI_SUM, comm);
    else
      mpi_ret = MPI_Reduce(MPI_IN_PLACE, tmp, m, MPI_SCALAR_T, MPI_SUM, root, comm);
    
    
    // TODO copy back
    
  }
  
  
  free(tmp);
  
  return mpi_ret;
}
*/



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP root, SEXP R_comm)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  // TODO
  
  return R_NilValue;
}
