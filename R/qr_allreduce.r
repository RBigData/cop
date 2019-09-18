#' @useDynLib cop cop_allreduce_mat_qr
qr_reducer = function(x, root, comm=0L, type)
{
  check_is_matrix(x)
  check_common_matrix_dims(x)
  
  if (!is.double(x))
    storage.mode(x) = "double"
  
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  type_int = ifelse(type=="double", TYPE_DOUBLE, TYPE_FLOAT)
  
  ret = .Call("cop_allreduce_mat_qr", x, comm_ptr, root, type_int, PACKAGE="cop")
  ret
}



#' @export
qr_allreduce = function(x, comm=0L, type="double")
{
  qr_reducer(x, root=REDUCE_TO_ALL, comm=comm, type=type)
}

#' @export
qr_reduce = function(x, root, comm=0L, type="double")
{
  qr_reducer(x, root=root, comm=comm, type=type)
}
