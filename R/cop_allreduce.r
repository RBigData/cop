REDUCE_TO_ALL = -1L



#' @useDynLib cop cop_allreduce_mat_userop
reducer = function(x, fun, commutative, root, comm=0L, env=parent.frame())
{
  commutative = as.logical(commutative)
  
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  if (is.matrix(x))
  {
    if (!is.double(x))
      storage.mode(x) = "double"
    
    ret = .Call(cop_allreduce_mat_userop, x, comm_ptr, root, commutative, fun, env)
  }
  
  ret
}



#' @export
cop_allreduce = function(x, fun, commutative, comm=0L, env=new.env())
{
  reducer(x, fun, commutative, root=REDUCE_TO_ALL, comm=comm, env=env)
}

#' @export
cop_reduce = function(x, fun, commutative, root, comm=0L, env=parent.frame())
{
  reducer(x, fun, commutative, root=root, comm=comm, env=env)
}
