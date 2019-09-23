#include <stddef.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

SEXP _R_env;
SEXP _R_fcall;
SEXP _send_data_cp;
SEXP _recv_data_cp;

size_t _copylen;


static void custom_op_matrix(double *a, double *b, int *len, MPI_Datatype *dtype)
{
  (void)a;
  (void)b;
  (void)len;
  (void)dtype;
  
  memcpy(REAL(_send_data_cp), a, _copylen);
  memcpy(REAL(_recv_data_cp), b, _copylen);
  
  SEXP lhs;
  PROTECT(lhs = eval(_R_fcall, _R_env));
  memcpy(b, REAL(lhs), _copylen);
  
  UNPROTECT(1);
}



SEXP cop_allreduce_mat_userop(SEXP send_data, SEXP R_comm, SEXP root, 
  SEXP commutative, SEXP fun, SEXP env)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  // data setup
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  _copylen = (size_t) m*n*sizeof(double);
  
  SEXP recv_data;
  PROTECT(recv_data = allocMatrix(REALSXP, m, n));
  _recv_data_cp = recv_data;
  
  PROTECT(_send_data_cp = allocMatrix(REALSXP, m, n));
  memcpy(REAL(_send_data_cp), REAL(send_data), _copylen);
  
  PROTECT(_R_env = env);
  PROTECT(_R_fcall = lang3(fun, _send_data_cp, recv_data));
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_DOUBLE, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  MPI_Op_create((MPI_User_function*) custom_op_matrix, LOGICAL(commutative)[0], &op);
  int ret;
  if (INTEGER(root)[0] == REDUCE_TO_ALL)
    ret = MPI_Allreduce(REAL(_send_data_cp), REAL(recv_data), 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(REAL(_send_data_cp), REAL(recv_data), 1, mat_type, op, INTEGER(root)[0], comm);
  check_MPI_ret(ret);
  
  // cleanup and return
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  UNPROTECT(4);
  return(recv_data);
}
