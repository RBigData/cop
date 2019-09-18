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
double *tallboy;
int lwork;
double *work;
double *qraux;
int *pivot;



static inline void FREE(void *x)
{
  if (x != NULL)
  {
    std::free(x);
    x = NULL;
  }
}

static inline void cleanup()
{
  FREE(tallboy);
  FREE(work);
  FREE(qraux);
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



static inline void init(int _m, int _n)
{
  m = _m;
  n = _n;
  minmn = std::min(m, n);
  mtb = 2*m;
  copylen = (size_t) m*n * sizeof(*tallboy);
  
  tallboy = (double*) malloc(mtb*n * sizeof(*tallboy));
  lwork = worksize(mtb, n);
  work = (double*) malloc(lwork * sizeof(*work));
  qraux = (double*) malloc(minmn * sizeof(*qraux));
  pivot = (int*) malloc(n * sizeof(*pivot));
  
  if (tallboy == NULL || work == NULL || qraux == NULL || pivot == NULL)
  {
    cleanup();
    error("OOM");
  }
}



static inline void custom_op(void *_a, void *_b, int *len, MPI_Datatype *dtype)
{
  (void)len;
  (void)dtype;
  
  double *a = (double*)_a;
  double *b = (double*)_b;
  
  #pragma omp parallel for default(shared) if(m*n > OMP_MIN_SIZE)
  for (int j=0; j<n; j++)
  {
    // memcpy(tallboy + mtb*j, a + m*j, m*sizeof(double));
    #pragma omp simd
    for (int i=0; i<m; i++)
      tallboy[i + mtb*j] = a[i + m*j];
    
    // memcpy(tallboy + m + mtb*j, b + m*j, m*sizeof(double));
    #pragma omp simd
    for (int i=0; i<m; i++)
      tallboy[m+i + mtb*j] = b[i + m*j];
  }
  
  int info = 0;
  lapack::geqp3(mtb, n, tallboy, pivot, qraux, work, lwork, &info);
  
  memset(b, 0, copylen);
  for (int j=0; j<n; j++)
  {
    #pragma omp for simd
    for (int i=0; i<=j; i++)
      b[i + m*j] = tallboy[i + mtb*j];
  }
}



static inline int allreduce(const int root, const int _m, const int _n,
  const double *const restrict a, double *const restrict b, MPI_Comm comm)
{
  int ret;
  
  init(_m, _n);
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_DOUBLE, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  const int commutative = 1;
  
  MPI_Op_create((MPI_User_function*) custom_op, commutative, &op);
  if (root == REDUCE_TO_ALL)
    ret = MPI_Allreduce(a, b, 1, mat_type, op, comm);
  else
    ret = MPI_Reduce(a, b, 1, mat_type, op, root, comm);
  
  // cleanup and return
  cleanup();
  MPI_Op_free(&op);
  MPI_Type_free(&mat_type);
  
  return ret;
}



extern "C" SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root)
{
  MPI_Comm comm = get_mpi_comm_from_Robj(R_comm);
  
  const int m = nrows(send_data);
  const int n = ncols(send_data);
  
  SEXP recv_data;
  PROTECT(recv_data = allocMatrix(REALSXP, m, n));
  
  int ret = allreduce(INTEGER(root)[0], m, n, REAL(send_data), REAL(recv_data), comm);
  check_MPI_ret(ret);
  
  UNPROTECT(1);
  return(recv_data);
}
