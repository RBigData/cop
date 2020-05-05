#include <spar.hpp>
#include <gen/gen.hpp>
#include <converters/s4.hpp>

#include "cxxutils.hpp"
#include "defs.h"
#include "utils.h"


extern "C" SEXP cop_spar_gen_bandish(SEXP seed_, SEXP m_, SEXP n_)
{
  SEXP ret;
  
  using INDEX = int;
  using SCALAR = uint16_t;
  
  const INDEX seed = (INDEX) INTEGER(seed_)[0];
  const INDEX m = (INDEX) INTEGER(m_)[0];
  const INDEX n = (INDEX) INTEGER(n_)[0];
  
  if (seed == -1)
  {
    auto s = spar::gen::bandish<INDEX, SCALAR>(m, n);
    PROTECT(ret = spar::conv::spmat_to_s4(s));
  }
  else
  {
    auto s = spar::gen::bandish<INDEX, SCALAR>(seed, m, n);
    PROTECT(ret = spar::conv::spmat_to_s4(s));
  }
  
  UNPROTECT(1);
  return ret;
}



extern "C" SEXP cop_spar_gen_banded(SEXP band_, SEXP m_, SEXP n_)
{
  SEXP ret;
  
  using INDEX = int;
  using SCALAR = uint16_t;
  
  const INDEX band = (INDEX) INTEGER(band_)[0];
  const INDEX m = (INDEX) INTEGER(m_)[0];
  const INDEX n = (INDEX) INTEGER(n_)[0];
  
  auto s = spar::gen::banded<INDEX, SCALAR>(band, m, n);
  PROTECT(ret = spar::conv::spmat_to_s4(s));
  
  UNPROTECT(1);
  return ret;
}
