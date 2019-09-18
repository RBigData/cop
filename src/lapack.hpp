#ifndef COP_LAPACK_H
#define COP_LAPACK_H


extern "C" void sgeqp3_(const int *const m, const int *const n,
  float *const restrict A, const int *const lda, int *const restrict jpvt,
  float *const restrict tau, float *const restrict work,
  const int *const restrict lwork, int *const restrict info);

extern "C" void dgeqp3_(const int *const m, const int *const n,
  double *const restrict A, const int *const lda, int *const restrict jpvt,
  double *const restrict tau, double *const restrict work,
  const int *const restrict lwork, int *const restrict info);


namespace lapack
{
  inline void geqp3(const int m, const int n, float *const restrict x,
    int *const restrict pivot, float *const restrict qraux,
    float *const restrict work, const int lwork, int *const restrict info)
  {
    sgeqp3_(&m, &n, x, &m, pivot, qraux, work, &lwork, info);
  }
  
  inline void geqp3(const int m, const int n, double *const restrict x,
    int *const restrict pivot, double *const restrict qraux,
    double *const restrict work, const int lwork, int *const restrict info)
  {
    dgeqp3_(&m, &n, x, &m, pivot, qraux, work, &lwork, info);
  }
}


#endif
