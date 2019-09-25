#include <iostream>

#include "cxxutils.hpp"
#include "eigenhelpers.hpp"
#include "utils.h"


using namespace eigenhelpers;


extern "C" SEXP cop_allreduce_spmat_add(SEXP send_data_s4, SEXP R_comm)
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
  std::cout << x << std::endl;
  
  SEXP ret;
  PROTECT(ret = spmat_to_s4(send_data));
  UNPROTECT(1);
  return ret;
}
