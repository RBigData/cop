/* Automatically generated. Do not edit by hand. */

#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <stdlib.h>

extern SEXP cop_allreduce_mat_userop(SEXP send_data, SEXP R_comm, SEXP root_, SEXP commutative, SEXP fun, SEXP env);
extern SEXP cop_allreduce_mat_qr(SEXP send_data, SEXP R_comm, SEXP root_, SEXP type);
extern SEXP cop_allreduce_spar_densevec(SEXP send_data_s4, SEXP root, SEXP R_comm, SEXP scalar_type_);
extern SEXP cop_allreduce_spar_gather(SEXP send_data_s4, SEXP root, SEXP R_comm, SEXP scalar_type_);
extern SEXP cop_spar_gen_bandish(SEXP seed_, SEXP m_, SEXP n_);
extern SEXP cop_spar_gen_banded(SEXP band_, SEXP m_, SEXP n_);

static const R_CallMethodDef CallEntries[] = {
  {"cop_allreduce_mat_userop", (DL_FUNC) &cop_allreduce_mat_userop, 6},
  {"cop_allreduce_mat_qr", (DL_FUNC) &cop_allreduce_mat_qr, 4},
  {"cop_allreduce_spar_densevec", (DL_FUNC) &cop_allreduce_spar_densevec, 4},
  {"cop_allreduce_spar_gather", (DL_FUNC) &cop_allreduce_spar_gather, 4},
  {"cop_spar_gen_bandish", (DL_FUNC) &cop_spar_gen_bandish, 3},
  {"cop_spar_gen_banded", (DL_FUNC) &cop_spar_gen_banded, 3},
  {NULL, NULL, 0}
};

void R_init_cop(DllInfo *dll)
{
  R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
  R_useDynamicSymbols(dll, FALSE);
}
