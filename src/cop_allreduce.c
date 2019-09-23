#include <stddef.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

SEXP R_env;
SEXP R_fcall;
SEXP send_data_cp;
SEXP recv_data_cp;


// ----------------------------------------------------------------------------
// matrix ops
// ----------------------------------------------------------------------------

size_t copy_len;



static void custom_op_matrix(double *a, double *b, int *len, MPI_Datatype *dtype)
{
  (void)a;
  (void)b;
  (void)len;
  (void)dtype;
  
  // for testing
  // for (int i=0; i<4; i++) b[i] += a[i];
  
  memcpy(REAL(send_data_cp), a, copy_len);
  memcpy(REAL(recv_data_cp), b, copy_len);
  
  SEXP lhs;
  PROTECT(lhs = eval(R_fcall, R_env));
  memcpy(b, REAL(lhs), copy_len);
  
  UNPROTECT(1);
}



SEXP cop_allreduce_mat_userop(SEXP send_data, SEXP R_comm, SEXP root, 
  SEXP commutative, SEXP fun, SEXP env)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  // data setup
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  copy_len = (size_t) m*n*sizeof(double);
  
  SEXP recv_data;
  PROTECT(recv_data = allocMatrix(REALSXP, m, n));
  recv_data_cp = recv_data;
  
  PROTECT(send_data_cp = allocMatrix(REALSXP, m, n));
  memcpy(REAL(send_data_cp), REAL(send_data), copy_len);
  
  PROTECT(R_env = env);
  PROTECT(R_fcall = lang3(fun, send_data_cp, recv_data));
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_DOUBLE, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  MPI_Op_create((MPI_User_function*) custom_op_matrix, LOGICAL(commutative)[0], &op);
  int ret;
  if (INTEGER(root)[0] == REDUCE_TO_ALL)
    ret = MPI_Allreduce(REAL(send_data_cp), REAL(recv_data), 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(REAL(send_data_cp), REAL(recv_data), 1, mat_type, op, INTEGER(root)[0], comm);
  check_MPI_ret(ret);
  
  // cleanup and return
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  UNPROTECT(4);
  return(recv_data);
}
