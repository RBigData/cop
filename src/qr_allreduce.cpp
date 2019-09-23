#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "cxxutils.hpp"
#include "defs.h"
#include "lapack.hpp"
#include "utils.h"

int _m, _n, _minmn, _mtb;
size_t _copylen;
int _lwork;
int *_pivot;
template <typename REAL>
REAL *_tallboy;
template <typename REAL>
REAL *_work;
template <typename REAL>
REAL *_qraux;


template <typename T, typename S>
void floatconv(const int nr, const int nc, const T *src, S *dst)
{
  for (int j=0; j<nc; j++)
  {
    #pragma omp for simd
    for (int i=0; i<nr; i++)
      dst[i + nr*j] = (S) src[i + nr*j];
  }
}



static inline void FREE(void *x)
{
  if (x != NULL)
  {
    std::free(x);
    x = NULL;
  }
}

template <typename REAL>
void qr_global_cleanup()
{
  FREE(_tallboy<REAL>);
  FREE(_work<REAL>);
  FREE(_qraux<REAL>);
  FREE(_pivot);
}



template <typename REAL>
static inline int qr_worksize(const int m, const int n)
{
  REAL tmp;
  
  int info;
  lapack::geqp3(m, n, NULL, NULL, NULL, &tmp, -1, &info);
  int lwork = (int) tmp;
  
  return std::max(lwork, 1);
}

template <typename REAL>
void qr_global_init(int m, int n)
{
  _m = m;
  _n = n;
  _minmn = std::min(_m, _n);
  _mtb = 2*_m;
  _copylen = (size_t) _m*_n * sizeof(REAL);
  
  _tallboy<REAL> = (REAL*) malloc(_mtb*_n * sizeof(REAL));
  _lwork = qr_worksize<REAL>(_mtb, _n);
  _work<REAL> = (REAL*) malloc(_lwork * sizeof(REAL));
  _qraux<REAL> = (REAL*) malloc(_minmn * sizeof(REAL));
  _pivot = (int*) malloc(_n * sizeof(REAL));
  
  if (_tallboy<REAL> == NULL || _work<REAL> == NULL || _qraux<REAL> == NULL || _pivot == NULL)
  {
    qr_global_cleanup<REAL>();
    error("OOM");
  }
}



template <typename REAL>
void custom_op_qr(void *a_, void *b_, int *len, MPI_Datatype *dtype)
{
  (void)len;
  (void)dtype;
  
  REAL *a = (REAL*)a_;
  REAL *b = (REAL*)b_;
  
  #pragma omp parallel for default(shared) if(_m*_n > OMP_MIN_SIZE)
  for (int j=0; j<_n; j++)
  {
    #pragma omp simd
    for (int i=0; i<_m; i++)
      _tallboy<REAL>[i + _mtb*j] = a[i + _m*j];
    
    #pragma omp simd
    for (int i=0; i<_m; i++)
      _tallboy<REAL>[_m+i + _mtb*j] = b[i + _m*j];
  }
  
  int info = 0;
  lapack::geqp3(_mtb, _n, _tallboy<REAL>, _pivot, _qraux<REAL>, _work<REAL>, _lwork, &info);
  
  memset(b, 0, _copylen);
  for (int j=0; j<_n; j++)
  {
    #pragma omp for simd
    for (int i=0; i<=j; i++)
      b[i + _m*j] = _tallboy<REAL>[i + _mtb*j];
  }
}



template <typename REAL>
int qr_allreduce(const int root, const REAL *const restrict a, 
  REAL *const restrict b, MPI_Datatype dt, MPI_Comm comm)
{
  int ret;
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(_m*_n, dt, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  const int commutative = 1;
  
  MPI_Op_create((MPI_User_function*) custom_op_qr<REAL>, commutative, &op);
  if (root == REDUCE_TO_ALL)
    ret = MPI_Allreduce(a, b, 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(a, b, 1, mat_type, op, root, comm);
  
  // cleanup and return
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  return ret;
}



extern "C" SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root_, SEXP type)
{
  int ret;
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  
  const int root = INTEGER(root_)[0];
  int rank;
  MPI_Comm_rank(comm, &rank);
  
  SEXP recv_data;
  
  if (INTEGER(type)[0] == TYPE_DOUBLE)
  {
    PROTECT(recv_data = allocMatrix(REALSXP, m, n));
    
    qr_global_init<double>(m, n);
    ret = qr_allreduce(root, REAL(send_data), REAL(recv_data), MPI_DOUBLE, comm);
    qr_global_cleanup<double>();
  }
  else // if (INTEGER(type)[0] == TYPE_FLOAT)
  {
    float *send_data_f = (float*) malloc(m*n*sizeof(*send_data_f));
    float *recv_data_f = (float*) malloc(m*n*sizeof(*recv_data_f));
    floatconv(m, n, REAL(send_data), send_data_f);
    
    qr_global_init<float>(m, n);
    ret = qr_allreduce(root, send_data_f, recv_data_f, MPI_FLOAT, comm);
    qr_global_cleanup<float>();
    
    free(send_data_f);
    PROTECT(recv_data = allocMatrix(REALSXP, m, n));
    if (root == REDUCE_TO_ALL || root == rank)
      floatconv(m, n, recv_data_f, REAL(recv_data));
    free(recv_data_f);
  }
  
  check_MPI_ret(ret);
  
  UNPROTECT(1);
  if (root == REDUCE_TO_ALL || root == rank)
    return recv_data;
  else
    return R_NilValue;
}
