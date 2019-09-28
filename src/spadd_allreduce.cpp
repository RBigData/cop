#include "cxxutils.hpp"
#include "defs.h"
#include "utils.h"


/*
static inline int spadd_allreduce(const int root, MPI_Comm comm)
{
  int mpi_ret;
  
  const index_t m = send_data.rows();
  const index_t n = send_data.cols();
  scalar_t *tmp = (scalar_t*) malloc(m * sizeof(*tmp));
  
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
