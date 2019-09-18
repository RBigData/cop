#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "cxxutils.hpp"
#include "defs.h"
#include "lapack.hpp"
#include "utils.h"



int m, n, minmn;
int mtb;
size_t copylen;
int lwork;
int *pivot;
template <typename REAL>
REAL *tallboy;
template <typename REAL>
REAL *work;
template <typename REAL>
REAL *qraux;



static inline void FREE(void *x)
{
  if (x != NULL)
  {
    std::free(x);
    x = NULL;
  }
}

template <typename REAL>
void cleanup()
{
  FREE(tallboy<REAL>);
  FREE(work<REAL>);
  FREE(qraux<REAL>);
  FREE(pivot);
}



static inline int worksize(const int m, const int n)
{
  double tmp;
  
  int info;
  lapack::geqp3(m, n, NULL, NULL, NULL, &tmp, -1, &info);
  int lwork = (int) tmp;
  
  return std::max(lwork, 1);
}

template <typename REAL>
void init(int _m, int _n)
{
  m = _m;
  n = _n;
  minmn = std::min(m, n);
  mtb = 2*m;
  copylen = (size_t) m*n * sizeof(REAL);
  
  tallboy<REAL> = (REAL*) malloc(mtb*n * sizeof(REAL));
  lwork = worksize(mtb, n);
  work<REAL> = (REAL*) malloc(lwork * sizeof(REAL));
  qraux<REAL> = (REAL*) malloc(minmn * sizeof(REAL));
  pivot = (int*) malloc(n * sizeof(REAL));
  
  if (tallboy<REAL> == NULL || work<REAL> == NULL || qraux<REAL> == NULL || pivot == NULL)
  {
    cleanup<REAL>();
    error("OOM");
  }
}



template <typename REAL>
void custom_op(void *_a, void *_b, int *len, MPI_Datatype *dtype)
{
  (void)len;
  (void)dtype;
  
  REAL *a = (REAL*)_a;
  REAL *b = (REAL*)_b;
  
  #pragma omp parallel for default(shared) if(m*n > OMP_MIN_SIZE)
  for (int j=0; j<n; j++)
  {
    // memcpy(tallboy + mtb*j, a + m*j, m*sizeof(REAL));
    #pragma omp simd
    for (int i=0; i<m; i++)
      tallboy<REAL>[i + mtb*j] = a[i + m*j];
    
    // memcpy(tallboy + m + mtb*j, b + m*j, m*sizeof(REAL));
    #pragma omp simd
    for (int i=0; i<m; i++)
      tallboy<REAL>[m+i + mtb*j] = b[i + m*j];
  }
  
  int info = 0;
  lapack::geqp3(mtb, n, tallboy<REAL>, pivot, qraux<REAL>, work<REAL>, lwork, &info);
  
  memset(b, 0, copylen);
  for (int j=0; j<n; j++)
  {
    #pragma omp for simd
    for (int i=0; i<=j; i++)
      b[i + m*j] = tallboy<REAL>[i + mtb*j];
  }
}



template <typename REAL>
int allreduce(const int root, const REAL *const restrict a, 
  REAL *const restrict b, MPI_Datatype dt, MPI_Comm comm)
{
  int ret;
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, dt, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  const int commutative = 1;
  
  MPI_Op_create((MPI_User_function*) custom_op<REAL>, commutative, &op);
  if (root == REDUCE_TO_ALL)
    ret = MPI_Allreduce(a, b, 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(a, b, 1, mat_type, op, root, comm);
  
  // cleanup and return
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  return ret;
}



extern "C" SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root, SEXP type)
{
  int ret;
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  const int _m = nrows(send_data);
  const int _n = ncols(send_data);
  
  SEXP recv_data;
  PROTECT(recv_data = allocMatrix(REALSXP, _m, _n));
  
  if (INTEGER(type)[0] == TYPE_DOUBLE)
  {
    init<double>(_m, _n);
    ret = allreduce(INTEGER(root)[0], REAL(send_data), REAL(recv_data), MPI_DOUBLE, comm);
    cleanup<double>();
  }
  else
  {
    float *send_data_f = (float*) malloc(m*n*sizeof(*send_data_f));
    float *recv_data_f = (float*) malloc(m*n*sizeof(*recv_data_f));
    for (int i=0; i<_m*_n; i++)
      send_data_f[i] = (float) REAL(send_data)[i];
    
    init<float>(_m, _n);
    ret = allreduce(INTEGER(root)[0], send_data_f, recv_data_f, MPI_FLOAT, comm);
    cleanup<float>();
    
    free(send_data_f);
    for (int i=0; i<_m*_n; i++)
      REAL(recv_data)[i] = (double) recv_data_f[i];
  }
  
  check_MPI_ret(ret);
  
  UNPROTECT(1);
  return(recv_data);
}
