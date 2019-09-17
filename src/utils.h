#ifndef COP_UTILS_H
#define COP_UTILS_H


#include <mpi.h>
#include <Rinternals.h>


static inline MPI_Comm get_mpi_comm_from_Robj(SEXP comm_ptr)
{
  MPI_Comm *comm = (MPI_Comm*) R_ExternalPtrAddr(comm_ptr);
  return *comm;
}



static inline void check_MPI_ret(int ret)
{
  if (ret != MPI_SUCCESS)
  {
    int slen;
    char s[MPI_MAX_ERROR_STRING];
    
    MPI_Error_string(ret, s, &slen);
    error(s);
  }
}


#endif
