#' Sparse Matrix Generators
#' 
#' Generate sparse matrices.
#' 
#' @details
#' \code{spar_gen_bandish()} generates a "band-ish" matrix, a matrix whose
#' entries are probabilistically more likely to be 1 the closer to the main
#' diagonal they are.
#' 
#' \code{spar_gen_banded()} generates a banded matrix.
#' 
#' @param nrows,ncols
#' The dimension
#' @param seed
#' Random seed.
#' @param band
#' The band size, a positive integer. \code{band=1} returns a matrix whose
#' diagonal entries are all 1; \code{band=2} returns a matrix with 1 on each
#' diagonal entry and 1 on each entry directly above and below the main
#' diagonal; etc.
#' 
#' @return
#' A sparse \code{"dgCMatrix"} object is returned.
#' 
#' @name sparse_gen
#' @rdname sparse_gen
NULL



#' @useDynLib cop cop_spar_gen_bandish
#' @rdname sparse_gen
#' @export
spar_gen_bandish = function(nrows, ncols, seed)
{
  if (!missing(seed))
    seed = as.integer(seed)
  else
    seed = -1L
  
  m = as.integer(nrows)
  n = as.integer(ncols)
  
  check_is_posint(seed)
  check_is_posint(m)
  check_is_posint(n)
  
  .Call(cop_spar_gen_bandish, seed, m, n)
}



#' @useDynLib cop cop_spar_gen_banded
#' @rdname sparse_gen
#' @export
spar_gen_banded = function(nrows, ncols, band=2)
{
  band = as.integer(band)
  m = as.integer(nrows)
  n = as.integer(ncols)
  
  check_is_posint(band)
  check_is_posint(m)
  check_is_posint(n)
  
  .Call(cop_spar_gen_banded, band, m, n)
}
