#ifndef COP_DEBUG_RPRINTER_H
#define COP_DEBUG_RPRINTER_H


#include <Rinternals.h>


static inline SEXP _R_evalfun_stringarg(const char *const restrict fun, const char *const restrict arg)
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
  
  PROTECT(basePackage = _R_evalfun_stringarg("getNamespace", "base"));
  
  PROTECT(print_install = install("print"));
  PROTECT(expr = lang2(print_install, x));
  eval(expr, basePackage);
  
  UNPROTECT(3);
}


#endif
