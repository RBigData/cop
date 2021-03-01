#include <arraytools/src/arraytools.hpp>

#include "cxxutils.hpp"
#include "defs.h"
#include "lapack.hpp"
#include "utils.h"


namespace tsqr
{
  namespace
  {
    bool badinfo;
    int _m, _n, minmn, mtb;
    int lwork;
    
    int *pivot;
    
    template <typename REAL>
    REAL *tallboy;
    template <typename REAL>
    REAL *work;
    template <typename REAL>
    REAL *qraux;
  }
  
  
  template <typename REAL>
  void qr_global_cleanup()
  {
    arraytools::free(tallboy<REAL>);
    tallboy<REAL> = NULL;
    
    arraytools::free(work<REAL>);
    work<REAL> = NULL;
    
    arraytools::free(qraux<REAL>);
    qraux<REAL> = NULL;
    
    arraytools::free(pivot);
    pivot = NULL;
  }
  
  
  
  template <typename REAL>
  static inline int qrworksize(const int m, const int n)
  {
    REAL tmp;
    
    int info;
    // lapack::geqp3(m, n, NULL, NULL, NULL, &tmp, -1, &info);
    lapack::geqrf(m, n, NULL, NULL, &tmp, -1, &info);
    int lwork = (int) tmp;
    
    return std::max(lwork, 1);
  }
  
  template <typename REAL>
  void qr_global_init(int m, int n)
  {
    _m = m;
    _n = n;
    minmn = std::min(_m, _n);
    mtb = 2*_m;
    
    badinfo = false;
    
    arraytools::alloc(mtb*_n, &(tallboy<REAL>));
    lwork = qrworksize<REAL>(mtb, _n);
    arraytools::alloc(lwork, &(work<REAL>));
    arraytools::alloc(minmn, &(qraux<REAL>));
    arraytools::alloc(_n, &pivot);
    
    // TODO use arraytools::check_alloc
    if (tallboy<REAL> == NULL || work<REAL> == NULL || qraux<REAL> == NULL || pivot == NULL)
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
        tallboy<REAL>[i + mtb*j] = a[i + _m*j];
      
      #pragma omp simd
      for (int i=0; i<_m; i++)
        tallboy<REAL>[_m+i + mtb*j] = b[i + _m*j];
    }
    
    int info = 0;
    // lapack::geqp3(mtb, _n, tallboy<REAL>, pivot, qraux<REAL>, work<REAL>, lwork, &info);
    lapack::geqrf(mtb, _n, tallboy<REAL>, qraux<REAL>, work<REAL>, lwork, &info);
    if (info != 0)
      badinfo = true;
    
    for (int j=0; j<_n; j++)
    {
      #pragma omp for simd
      for (int i=0; i<=j; i++)
        b[i + _m*j] = tallboy<REAL>[i + mtb*j];
      
      #pragma omp for simd
      for (int i=j+1; i<_m; i++)
        b[i + _m*j] = (REAL) 0.f;
    }
  }
  
  
  
  template <typename REAL>
  void qr_allreduce(const int root, const REAL *const restrict a, 
    REAL *const restrict b, MPI_Datatype dt, MPI_Comm comm)
  {
    int mpi_ret;
    
    // custom data type
    MPI_Datatype mat_type;
    MPI_Type_contiguous(_m*_n, dt, &mat_type);
    MPI_Type_commit(&mat_type);
    
    // custom op + reduce
    MPI_Op op;
    const int commutative = 1;
    
    MPI_Op_create((MPI_User_function*) custom_op_qr<REAL>, commutative, &op);
    if (root == REDUCE_TO_ALL)
      mpi_ret = MPI_Allreduce(a, b, 1, mat_type, op, comm);
    else
      mpi_ret = MPI_Reduce(a, b, 1, mat_type, op, root, comm);
    
    // cleanup and return
    MPI_Op_free(&op);
    MPI_Type_free(&mat_type);
    
    check_MPI_ret(mpi_ret);
    if (badinfo)
      error("unrecoverable error with LAPACK function geqrf() occurred during reduction");
  }
}



extern "C" SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root_,
  SEXP type)
{
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
    
    tsqr::qr_global_init<double>(m, n);
    tsqr::qr_allreduce(root, REAL(send_data), REAL(recv_data), MPI_DOUBLE, comm);
    tsqr::qr_global_cleanup<double>();
  }
  else // if (INTEGER(type)[0] == TYPE_FLOAT)
  {
    float *send_data_f = (float*) malloc(m*n*sizeof(*send_data_f));
    float *recv_data_f = (float*) malloc(m*n*sizeof(*recv_data_f));
    arraytools::copy(m*n, REAL(send_data), send_data_f);
    
    tsqr::qr_global_init<float>(m, n);
    tsqr::qr_allreduce(root, send_data_f, recv_data_f, MPI_FLOAT, comm);
    tsqr::qr_global_cleanup<float>();
    
    free(send_data_f);
    PROTECT(recv_data = allocMatrix(REALSXP, m, n));
    if (root == REDUCE_TO_ALL || root == rank)
      arraytools::copy(m*n, recv_data_f, REAL(recv_data));
    free(recv_data_f);
  }
  
  UNPROTECT(1);
  if (root == REDUCE_TO_ALL || root == rank)
    return recv_data;
  else
    return R_NilValue;
}
