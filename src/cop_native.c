/* Automatically generated. Do not edit by hand. */

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <stdlib.h>

extern SEXP cop_allreduce_mat_userop(SEXP send_data, SEXP R_comm, SEXP fun, SEXP commutative, SEXP env, SEXP root);
extern SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root_, SEXP type);

static const R_CallMethodDef CallEntries[] = {
  {"cop_allreduce_mat_userop", (DL_FUNC) &cop_allreduce_mat_userop, 6},
  {"cop_allreduce_mat_qr", (DL_FUNC) &cop_allreduce_mat_qr, 4},
  {NULL, NULL, 0}
};

void R_init_cop(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
