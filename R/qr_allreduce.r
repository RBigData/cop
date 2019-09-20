#' @useDynLib cop cop_allreduce_mat_qr
qr_reducer = function(x, root, comm=0L, type)
{
  check_is_matrix(x, comm)
  check_common_matrix_dims(x, comm)
  check_is_int(root, comm)
  type = comm.match.arg(tolower(type), TYPES_STR, comm=comm)
  type_int = type_str2int(type)
  
  root = as.integer(root)
  
  if (!is.double(x))
    storage.mode(x) = "double"
  
  comm = as.integer(comm)
  comm_ptr = pbdMPI::get.mpi.comm.ptr(comm)
  
  if (comm.size(comm=comm) == 1)
    ret = qr.R(qr(x, LAPACK=TRUE))
  else
    ret = .Call(cop_allreduce_mat_qr, x, comm_ptr, root, type_int)
  
  ret
}



#' QR Reduction
#' 
#' Given an R matrix from a QR factorization on each process, this computes the
#' R matrix which is conceptually calculated by "stacking" all R matrices on top
#' of each other (like \code{do.call(rbind, R_list)}). The way this is actually
#' calculated is much more memory efficient, and probably more run time
#' efficient as well, if the local R matrices aren't very small.
#' 
#' This can be used to create a TSQR. If the tall matrix is split by rows across
#' processes, compute the R matrix on the local chunk and then hand it to
#' \code{qr_reduce()}.
#' 
#' @details
#' This works by defining a custom MPI data type (dense matrix) with a custom
#' reduction operation (given 2 R matrices, "stack" them, compute the stacked
#' QR and emit R).
#' Most likely, your MPI implementation uses 
#' 
#' Each local operation uses the LAPACK functions \code{_geqp3()}, similar to R's \code{qr()} with
#' \code{LAPACK=TRUE}.
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
#' @examples
#' \dontrun{
#' suppressMessages(library(cop))
#' 
#' x = matrix(1:4, 2) + 10*comm.rank()
#' out = qr_reduce(x)
#' mpi_print(out)
#' 
#' finalize()
#' }
#' 
#' @name qr_allreduce
#' @rdname qr_allreduce
NULL



#' @rdname qr_allreduce
#' @export
qr_allreduce = function(x, comm=0L, type="double")
{
  qr_reducer(x, root=REDUCE_TO_ALL, comm=comm, type=type)
}

#' @rdname qr_allreduce
#' @export
qr_reduce = function(x, root=0, comm=0L, type="double")
{
  qr_reducer(x, root=root, comm=comm, type=type)
}
