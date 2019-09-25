#include "cxxutils.hpp"
#include "defs.h"
#include "eigenhelpers.hpp"
#include "utils.h"

using namespace eigenhelpers;


static inline int spadd_allreduce(const int root, spmat &send_data, MPI_Comm comm)
{
  int mpi_ret;
  
  const index_t m = send_data.rows();
  const index_t n = send_data.cols();
  scalar_t *tmp = (scalar_t*) malloc(m * sizeof(*tmp));
  
  for (index_t j=0; j<n; j++)
  {
    sparsecol_to_densecol(j, send_data, tmp);
    
    if (root == REDUCE_TO_ALL)
      mpi_ret = MPI_Allreduce(MPI_IN_PLACE, tmp, m, MPI_SCALAR_T, MPI_SUM, comm);
    else
      mpi_ret = MPI_Reduce(MPI_IN_PLACE, tmp, m, MPI_SCALAR_T, MPI_SUM, root, comm);
    
    
    // TODO copy back
    
  }
  
  
  free(tmp);
  
  return mpi_ret;
}



extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP root, SEXP R_comm)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  spmat send_data = s4_to_spmat(send_data_s4);
  
  spmat x(send_data.rows(), send_data.cols());
  x.makeCompressed();
  x.reserve(3);
  x.insert(0, 0) = 1;
  x.insert(9, 0) = 1;
  x.insert(3, 1) = 1;
  x += send_data;
  
  spadd_allreduce(INTEGER(root)[0], x, comm);
  
  SEXP ret;
  PROTECT(ret = spmat_to_s4(send_data));
  UNPROTECT(1);
  return ret;
}
