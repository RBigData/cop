REDUCE_TO_ALL = -1L



#' @useDynLib cop cop_allreduce_mat_qr
qr_reducer = function(x, root, comm=0L)
{
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  if (is.matrix(x))
  {
    if (!is.double(x))
      storage.mode(x) = "double"
    
    ret = .Call("cop_allreduce_mat_qr", x, comm_ptr, root, PACKAGE="cop")
  }
  
  ret
}



#' @export
qr_allreduce = function(x, comm=0L)
{
  qr_reducer(x, root=REDUCE_TO_ALL, comm=comm)
}

#' @export
qr_reduce = function(x, root, comm=0L)
{
  qr_reducer(x, root=root, comm=comm)
}
