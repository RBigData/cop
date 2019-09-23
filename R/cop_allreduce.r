REDUCE_TO_ALL = -1L



#' @useDynLib cop cop_allreduce_mat_userop
reducer = function(x, op, commutative, root, comm=0L)
{
  check_is_matrix(x, comm)
  check_is_function(op, comm)
  check_is_flag(commutative, comm)
  check_is_int(root, comm)
  
  commutative = as.logical(commutative)
  
  root = as.integer(root)
  
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  env=parent.frame()
  if (is.matrix(x))
  {
    if (!is.double(x))
      storage.mode(x) = "double"
    
    ret = .Call(cop_allreduce_mat_userop, x, comm_ptr, root, commutative, op, env)
  }
  
  ret
}



#' Custom Reduction
#' 
#' This performs a reduce/allreduce operation on a matrix with a custom binary
#' operation. Currently, only normal dense/numeric matrices are supported.
#' 
#' This is a generalization of the \code{qr_allreduce()}/\code{qr_reduce()}
#' methods also offered by this package. Conceptually, \code{qr_reduce()} is the
#' same as doing \code{cop_reduce()} with
#' \code{op = function(a, b) qr.R(qr(a, b))} and \code{commutative=TRUE},
#' although the internals of \code{qr_reduce()} are actually quite different.
#' In general, it should be faster and use  less memory.
#' 
#' @details
#' The function should take inputs of the same dimension, and the output should
#' be of the same dimension as the inputs. Indeed, for intermediary
#' calculations, the output of the op at one level will be an input for the next
#' evaliation. This criterion is assumed and not validated or tested.
#' Violating it could cause crashes or difficult to understand memory errors.
#' 
#' The operation should be associative, meaning that you should be able to put
#' parentheses wherever you want with 2 or more chained calls. Said another
#' way, \code{a op b op c} is the same whether evaluated \code{a op (b op c)} or
#' \code{(a op b) op c}. The operation does not have to be commutative, which
#' would require that \code{a op b} is the same as \code{b op a}. However, if it
#' is, then there may be an improvement to performance by correctly specifying
#' it as such via the \code{commutative} argument.
#' 
#' @param x
#' The input data. Should be a numeric matrix. The matrix should be the same
#' dimensions across all processes.
#' @param op
#' A binary operation for numeric matrices. The function should be associative,
#' and inputs/outputs are assumed to have the same dimensions. See the Details
#' section for more information.
#' @param commutative
#' Is the operation commutative? Should be one of \code{TRUE} or \code{FALSE}.
#' @param comm
#' MPI communicator number.
#' @param root
#' MPI rank that should receive the return in the non-all version.
#' 
#' @return
#' If the all version is called or if the calling rank is equal to \code{root},
#' then a numeric matrix is returned, and otherwise \code{NULL}.
#' 
#' @examples
#' \dontrun{
#' suppressMessages(library(cop))
#' 
#' x = matrix(1:4, 2) + 10*comm.rank()
#' add = function(a, b) a+b
#' out = cop_allreduce(x, fun=add, commutative=TRUE)
#' mpi_print(out)
#' 
#' finalize()
#' }
#' 
#' @seealso \code{\link{qr_allreduce}()}
#' 
#' @name cop_allreduce
#' @rdname cop_allreduce
NULL



#' @export
cop_allreduce = function(x, op, commutative=FALSE, comm=0L)
{
  reducer(x=x, op=op, commutative=commutative, root=REDUCE_TO_ALL, comm=comm)
}

#' @export
cop_reduce = function(x, op, root, commutative=FALSE, comm=0L)
{
  reducer(x=x, op=op, commutative=commutative, root=root, comm=comm)
}
