#include <spar.hpp>
#include <converters/s4.hpp>
#include <reduce.hpp>

#include "cxxutils.hpp"
#include "defs.h"
#include "utils.h"


#define REDUCE(scalar_type, FUN) \
  if (scalar_type == TYPE_DOUBLE) \
  { \
    using SCALAR = double; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_FLOAT) \
  { \
    using SCALAR = float; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_INT) \
  { \
    using SCALAR = int; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_INT8) \
  { \
    using SCALAR = int8_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_INT16) \
  { \
    using SCALAR = int16_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_INT32) \
  { \
    using SCALAR = int32_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_INT64) \
  { \
    using SCALAR = int64_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_UINT8) \
  { \
    using SCALAR = uint8_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_UINT16) \
  { \
    using SCALAR = uint16_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else if (scalar_type == TYPE_UINT32) \
  { \
    using SCALAR = uint32_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  } \
  else /*if (scalar_type == TYPE_UINT64)*/ \
  { \
    using SCALAR = uint64_t; \
    auto s = FUN<SEXP, INDEX, SCALAR>(root, send_data_s4, comm); \
    PROTECT(ret = spar::conv::spmat_to_s4(s)); \
  }



extern "C" SEXP cop_allreduce_spar_densevec(SEXP send_data_s4, SEXP root_,
  SEXP R_comm, SEXP scalar_type_)
{
  SEXP ret;
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  int root = INTEGER(root_)[0];
  
  using INDEX = int;
  const int scalar_type = INTEGER(scalar_type_)[0];
  
  REDUCE(scalar_type, spar::reduce::dense)
  
  UNPROTECT(1);
  return ret;
}



extern "C" SEXP cop_allreduce_spar_gather(SEXP send_data_s4, SEXP root_,
  SEXP R_comm, SEXP scalar_type_)
{
  SEXP ret;
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  int root = INTEGER(root_)[0];
  
  using INDEX = int;
  const int scalar_type = INTEGER(scalar_type_)[0];
  
  TRY_CATCH(
    REDUCE(scalar_type, spar::reduce::gather)
  )
  
  UNPROTECT(1);
  return ret;
}
