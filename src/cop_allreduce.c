#include <stddef.h>
#include <string.h>

#include "defs.h"
#include "utils.h"

SEXP _R_env;
SEXP _R_fcall;
SEXP _send_data_cp;
SEXP _recv_data;
SEXP lhs;

size_t _copylen;


static void custom_op_matrix(double *a, double *b, int *len, MPI_Datatype *dtype)
{
  (void)len;
  (void)dtype;
  
  memcpy(REAL(_send_data_cp), a, _copylen);
  memcpy(REAL(_recv_data), b, _copylen);
  PROTECT(lhs = eval(_R_fcall, _R_env));
  memcpy(b, REAL(lhs), _copylen);
  UNPROTECT(1);
}



SEXP cop_allreduce_mat_userop(SEXP send_data, SEXP R_comm, SEXP root_, 
  SEXP commutative, SEXP fun, SEXP env)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  // data setup
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  _copylen = (size_t) m*n*sizeof(double);

  double *_a_cp = (double*) R_alloc(m*n, sizeof(double));
  double *_b_cp = (double*) R_alloc(m*n, sizeof(double));
  memcpy(_a_cp, REAL(send_data), _copylen);
  
  const int root = INTEGER(root_)[0];
  int rank;
  MPI_Comm_rank(comm, &rank);
  
  PROTECT(_recv_data = allocMatrix(REALSXP, m, n));
  PROTECT(_send_data_cp = allocMatrix(REALSXP, m, n));
  PROTECT(_R_env = env);
  PROTECT(_R_fcall = lang3(fun, _send_data_cp, _recv_data));
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_DOUBLE, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  MPI_Op_create((MPI_User_function*) custom_op_matrix, LOGICAL(commutative)[0], &op);
  int ret;
  if (root == REDUCE_TO_ALL)
    ret = MPI_Allreduce(_a_cp, _b_cp, 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(_a_cp, _b_cp, 1, mat_type, op, root, comm);
  
  check_MPI_ret(ret);
  
  // cleanup and return
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  if (root == REDUCE_TO_ALL || root == rank)
    memcpy(REAL(_recv_data), _b_cp, _copylen);
  else
    _recv_data = R_NilValue;

  UNPROTECT(4);
  return _recv_data;
}
