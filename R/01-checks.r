is.badval <- function(x)
{
  is.na(x) || is.nan(x) || is.infinite(x)
}

is.inty <- function(x)
{
  abs(x - round(x)) < 1e-10
}

is.zero <- function(x)
{
  abs(x) < 1e-10
}

is.negative <- function(x)
{
  x < 0
}

is.annoying <- function(x)
{
  length(x) != 1 || is.badval(x)
}

is.string <- function(x)
{
  is.character(x) && !is.annoying(x)
}

is.flag <- function(x)
{
  is.logical(x) && !is.annoying(x)
}

is.posint <- function(x)
{
  is.numeric(x) && !is.annoying(x) && is.inty(x) && !is.negative(x) && !is.zero(x)
}



check_is_flag = function(x, comm)
{
  if (!is.flag(x))
  {
    nm <- deparse(substitute(x))
    comm.stop(paste0("argument '", nm, "' must be TRUE or FALSE"), call.=FALSE, comm=comm)
  }
  
  invisible(TRUE)
}



check_is_int = function(x, comm)
{
  if (!is.numeric(x) || is.annoying(x) || !is.inty(x))
  {
    nm <- deparse(substitute(x))
    comm.stop(paste0("argument '", nm, "' must be an integer"), call.=FALSE, comm=comm)
  }
  
  invisible(TRUE)
}



check_is_function = function(x, comm)
{
  if (!is.function(x))
  {
    nm <- deparse(substitute(x))
    comm.stop(paste0("argument '", nm, "' must be a function"), call.=FALSE, comm=comm)
  }
  
  invisible(TRUE)
}



check_is_matrix = function(x, comm)
{
  if (!is.matrix(x) || !is.numeric(x))
  {
    nm <- deparse(substitute(x))
    comm.stop(paste0("argument '", nm, "' must be a numeric matrix"), call.=FALSE, comm=comm)
  }
  
  invisible(TRUE)
}



check_common_matrix_dims = function(x, comm)
{
  dims = as.double(dim(x))
  alldims = pbdMPI::allgather(dims, comm=comm)
  
  nrows = sapply(alldims, `[`, 1L)
  ncols = sapply(alldims, `[`, 2L)
  
  if (any(diff(nrows) != 0) || any(diff(ncols) != 0))
    pbdMPI::comm.stop("input matrices must have the same dimension across all processes", comm=comm)
  
  invisible(TRUE)
}
