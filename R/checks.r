check_is_matrix = function(x, comm)
{
  if (!is.matrix(x))
    pbdMPI::comm.stop("TODO", comm=comm)
}



check_common_matrix_dims = function(x, comm)
{
  dims = as.double(dim(x))
  alldims = pbdMPI::allgather(dims, comm=comm)
  
  nrows = sapply(alldims, `[`, 1L)
  ncols = sapply(alldims, `[`, 2L)
  
  if (any(diff(nrows) != 0) || any(diff(ncols) != 0))
    pbdMPI::comm.stop("", comm=comm)
}
