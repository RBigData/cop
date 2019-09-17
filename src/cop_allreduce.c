#include <mpi.h>
#include <Rinternals.h>
#include <stddef.h>
#include <string.h>

SEXP R_env;
SEXP R_fcall;


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



static inline SEXP evalfun_stringarg(const char *const restrict fun, const char *const restrict arg)
{
  SEXP ret, expr, fun_install, arg_str;
  PROTECT(fun_install = install(fun));
  PROTECT(arg_str = ScalarString(mkChar(arg)));
  PROTECT(expr = lang2(fun_install, arg_str));
  PROTECT(ret = eval(expr, R_GlobalEnv));
  
  UNPROTECT(4);
  return ret;
}

static inline void PRINT(SEXP x)
{
  SEXP basePackage;
  SEXP print_install;
  SEXP expr;
  
  PROTECT(basePackage = evalfun_stringarg("getNamespace", "base"));
  
  PROTECT(print_install = install("print"));
  PROTECT(expr = lang2(print_install, x));
  eval(expr, basePackage);
  
  UNPROTECT(3);
}



// ----------------------------------------------------------------------------
// matrix ops
// ----------------------------------------------------------------------------

size_t _len;



static void mat_op_eval_R(double *a, double *b, int *len, MPI_Datatype *dtype)
{
  (void)a;
  (void)len;
  (void)dtype;
  
  // SEXP lhs;
  // PROTECT(lhs = eval(R_fcall, R_env));
  // memcpy(b, REAL(lhs), _len);
  // 
  // UNPROTECT(1);
  for (int i=0; i<4; i++) b[i] += a[i];
}



SEXP cop_allreduce_mat_userop(SEXP send_data, SEXP R_comm, SEXP fun,
  SEXP commutative, SEXP env, SEXP root)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  _len = (size_t) m*n*sizeof(double);
  
  SEXP recv_data;
  PROTECT(recv_data = allocMatrix(REALSXP, m, n));
  
  PROTECT(R_env);
  R_env = env;
  PROTECT(R_fcall);
  R_fcall = lang3(fun, send_data, recv_data);
  
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_DOUBLE, &mat_type);
  MPI_Type_commit(&mat_type);
  
  MPI_Op op;
  MPI_Op_create((MPI_User_function*) mat_op_eval_R, LOGICAL(commutative)[0], &op);
  int ret;
  if (INTEGER(root)[0] == -1)
    ret = MPI_Allreduce(REAL(send_data), REAL(recv_data), 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(REAL(send_data), REAL(recv_data), 1, mat_type, op, INTEGER(root)[0], comm);
  check_MPI_ret(ret);
  MPI_Op_free(&op);
  
  MPI_Type_free(&mat_type);
  
  UNPROTECT(3);
  return(recv_data);
}
