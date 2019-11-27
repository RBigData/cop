#ifndef COP_SPARSEHELPERS_H
#define COP_SPARSEHELPERS_H
#pragma once


#include <Rdefines.h>
#include <Rinternals.h>

#include "spvec/src/spmat.hpp"
#include "spvec/src/spvec.hpp"


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
    
    const INDEX m = s.nrows();
    const INDEX n = s.ncols();
    const INDEX nnz = s.get_nnz();
    const INDEX p_len = n+1;
    
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


#endif
