#' @useDynLib cop cop_allreduce_spmat_add
spadd_reducer = function(x, root, comm)
{
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  .Call(cop_allreduce_spmat_add, x, root, comm_ptr)
}



#' @export
spadd_allreduce = function(x, comm=0L)
{
  spadd_reducer(x=x, root=REDUCE_TO_ALL, comm=comm)
}

#' @export
spadd_reduce = function(x, root=0L, comm=0L)
{
  spadd_reducer(x=x, root=root, comm=comm)
}
