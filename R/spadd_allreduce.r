#' @useDynLib cop cop_allreduce_spmat_add
spadd_reducer = function(x, root, comm, type)
{
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  .Call(cop_allreduce_spmat_add, x, root, comm_ptr)
}



#' Sparse Adder Reduction
#' 
#' TODO
#' 
#' @details
#' TODO
#' 
#' @param x
#' The input data. Should be a numeric matrix. The matrix should be the same
#' dimensions across all processes.
#' @param comm
#' MPI communicator number.
#' @param type
#' The precision used for the intermediate calculations. Should be one of
#' "double" or "float".
#' @param root
#' MPI rank that should receive the return in the non-all version.
#' 
#' @return
#' If the all version is called or if the calling rank is equal to \code{root},
#' then a numeric matrix is returned, and otherwise \code{NULL}.
#' 
#' @name spadd_allreduce
#' @rdname spadd_allreduce
NULL



#' @rdname spadd_allreduce
#' @export
spadd_allreduce = function(x, comm=0L, type="double")
{
  UseMethod("spadd_allreduce", x)
}

#' @rdname spadd_allreduce
#' @export
spadd_reduce = function(x, root=0L, comm=0L, type="double")
{
  UseMethod("spadd_allreduce", x)
}



#' @rdname spadd_allreduce
#' @export
spadd_allreduce.dgCMatrix = function(x, comm=0L, type="double")
{
  spadd_reducer(x=x, root=REDUCE_TO_ALL, comm=comm, type=type)
}

#' @rdname spadd_allreduce
#' @export
spadd_reduce.dgCMatrix = function(x, root=0L, comm=0L, type="double")
{
  spadd_reducer(x=x, root=root, comm=comm, type=type)
}
