#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "utils.h"


extern void dgeqp3_(const int *const m, const int *const n,
  double *const restrict A, const int *const lda, int *const restrict jpvt,
  double *const restrict tau, double *const restrict work,
  const int *const restrict lwork, int *const restrict info);


int _m, _n, _minmn;
int _mtb;
size_t _len;
double *_tallboy;
int _lwork;
double *_work;
double *_qraux;
int *_pivot;



static inline int worksize(const int m, const int n)
{
  int lwork;
  double tmp;
  
  dgeqp3_(&m, &n, &(double){0}, &m, &(int){0}, &(double){0}, &tmp, &(int){-1}, &(int){0});
  lwork = (int) tmp;
  
  return MAX(lwork, 1);
}



static void custom_op_qr_cleanup()
{
  if (_tallboy) free(_tallboy);
  if (_work) free(_work);
  if (_qraux) free(_qraux);
  if (_pivot) free(_pivot);
}



static void custom_op_qr_init(const int m, const int n)
{
  _m = m;
  _n = n;
  _minmn = MIN(m, n);
  _mtb = 2*m;
  _len = (size_t) m*n * sizeof(*_tallboy);
  
  // TODO handle bad mallocs
  _tallboy = malloc(_mtb*_n * sizeof(*_tallboy));
  _lwork = worksize(_mtb, _n);
  _work = malloc(_lwork * sizeof(*_work));
  _qraux = malloc(_minmn * sizeof(*_qraux));
  _pivot = malloc(n * sizeof(*_pivot));
}



static void custom_op_qr(double *a, double *b, int *len, MPI_Datatype *dtype)
{
  (void)len;
  (void)dtype;
  
  int info;
  
  for (int j=0; j<_n; j++)
  {
    for (int i=0; i<_m; i++)
      _tallboy[i + _mtb*j] = a[i + _m*j];
    
    for (int i=0; i<_m; i++)
      _tallboy[_m+i + _mtb*j] = b[i + _m*j];
  }
  
  dgeqp3_(&_mtb, &_n, _tallboy, &_mtb, _pivot, _qraux, _work, &_lwork, &info);
  
  memset(b, 0, _len);
  for (int j=0; j<_n; j++)
  {
    for (int i=0; i<=j; i++)
      b[i + _m*j] = _tallboy[i + _mtb*j];
  }
}



static int qr_allreduce(const int root, const int m, const int n,
  const double *const restrict a, double *const restrict b, MPI_Comm comm)
{
  int ret;
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_DOUBLE, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  custom_op_qr_init(m, n);
  
  MPI_Op op;
  const int commutative = 1;
  MPI_Op_create((MPI_User_function*) custom_op_qr, commutative, &op);
  if (root == REDUCE_TO_ALL)
    ret = MPI_Allreduce(a, b, 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(a, b, 1, mat_type, op, root, comm);
  
  // cleanup and return
  custom_op_qr_cleanup();
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  return ret;
}



SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  // data setup
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  
  SEXP recv_data;
  PROTECT(recv_data = allocMatrix(REALSXP, m, n));
  
  int ret = qr_allreduce(INTEGER(root)[0], m, n, REAL(send_data), REAL(recv_data), comm);
  check_MPI_ret(ret);
  
  UNPROTECT(1);
  return(recv_data);
}
