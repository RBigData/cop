check_is_matrix = function(x)
{
  if (!is.matrix(x))
    pbdMPI::comm.stop("TODO", comm=comm)
}



check_common_matrix_dims = function(x)
{
  dims = as.double(dim(x))
  # TODO
}
