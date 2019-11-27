#include "cxxutils.hpp"
#include "defs.h"
#include "sparsehelpers.hpp"
#include "utils.h"


template <typename INDEX, typename SCALAR>
static inline spmat<INDEX, SCALAR> spadd_allreduce_densevec(const int root, SEXP send_data_s4, MPI_Comm comm)
{
  int mpi_ret;
  int m, n;
  sparsehelpers::sexp::get_dim_from_s4(send_data_s4, &m, &n);
  
  // setup
  const int len = sparsehelpers::sexp::get_col_len_from_s4(0, sparsehelpers::sexp::get_p_from_s4(send_data_s4)) * sparsehelpers::constants::MEM_FUDGE_ELT_FAC;
  spvec<INDEX, SCALAR> a(len);
  spmat<INDEX, SCALAR> s(m, n, n*len);
  dvec<INDEX, SCALAR> d(m);
  
  const MPI_Datatype reduce_type = mpi_type_lookup(*d.data_ptr());
  if (reduce_type == MPI_DATATYPE_NULL)
    error("unknown reducer type");
  
  // allreduce column-by-column
  for (INDEX j=0; j<n; j++)
  {
    sparsehelpers::s4col_to_spvec(j, send_data_s4, a);
    a.densify(d);
    
    if (root == REDUCE_TO_ALL)
      mpi_ret = MPI_Allreduce(MPI_IN_PLACE, d.data_ptr(), m, reduce_type, MPI_SUM, comm);
    else
      mpi_ret = MPI_Reduce(MPI_IN_PLACE, d.data_ptr(), m, reduce_type, MPI_SUM, root, comm);
    
    check_MPI_ret(mpi_ret);
    
    d.update_nnz();
    a.set(d);
    
    int needed_space = s.insert(j, a);
    if (needed_space > 0)
    {
      s.resize((s.get_len() + needed_space) * sparsehelpers::constants::MEM_FUDGE_ELT_FAC);
      s.insert(j, a);
    }
  }
  
  return s;
}



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP root, SEXP R_comm,
  SEXP scalar_type_)
{
  SEXP ret;
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  using INDEX = int;
  const int scalar_type = INTEGER(scalar_type_)[0];
  
  if (scalar_type == TYPE_DOUBLE)
  {
    using SCALAR = double;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_FLOAT)
  {
    using SCALAR = float;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_INT)
  {
    using SCALAR = int;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_INT8)
  {
    using SCALAR = int8_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_INT16)
  {
    using SCALAR = int16_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_INT32)
  {
    using SCALAR = int32_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_INT64)
  {
    using SCALAR = int64_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_UINT8)
  {
    using SCALAR = uint8_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_UINT16)
  {
    using SCALAR = uint16_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else if (scalar_type == TYPE_UINT32)
  {
    using SCALAR = uint32_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  else //if (scalar_type == TYPE_UINT64)
  {
    using SCALAR = uint64_t;
    auto s = spadd_allreduce_densevec<INDEX, SCALAR>(INTEGER(root)[0], send_data_s4, comm);
    PROTECT(ret = sparsehelpers::spmat_to_s4(s));
  }
  
  
  UNPROTECT(1);
  return ret;
}
