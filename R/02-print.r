rankcat = function(rank, quiet)
{
  if (!isTRUE(quiet))
    cat(paste0("(rank=", rank, ")\n"))
  
 invisible()
}

printer = function(printfun, x, quiet=FALSE, all.rank=FALSE, comm=0L)
{
  myrank = pbdMPI::comm.rank(comm=comm)
  
  if (myrank == 0)
  {
    rankcat(myrank, quiet)
    printfun(x)
  }
  
  if (all.rank)
  {
    for (rank in (seq_len(comm.size()) - 1L)[-1])
    {
      if (myrank == 0)
      {
        rankmsg_len = pbdMPI::spmd.recv.integer(integer(1), rank.source=rank, comm=comm)
        rankmsg = pbdMPI::recv(character(rankmsg_len), rank.source=rank, comm=comm)
        rankcat(rank, quiet)
        cat(rankmsg)
      }
      else
      {
        cx = paste(capture.output(printfun(x)), "\n", collapse="")
        pbdMPI::spmd.send.integer(nchar(cx), rank.dest=0, comm=comm)
        pbdMPI::send(cx, rank.dest=0, comm=comm)
      }
    }
  }
  
  pbdMPI::barrier(comm=comm)
  invisible()
}



#' MPI Printer
#' 
#' Orderly printing of data across MPI ranks. Similar to
#' \code{pbdMPI::comm.print()}.
#' 
#' @details
#' All printing is done on rank 0 to ensure printing occurs in rank order. This
#' requires communication if \code{all.rank=TRUE}.
#' 
#' @param x
#' The input to be printed.
#' @param quiet
#' Should ranks print their rank number before printing the input?
#' @param all.rank
#' Should all ranks print or only rank 0?
#' @param comm
#' MPI communicator number.
#' 
#' @return
#' Invisibly returns \code{NULL}.
#' 
#' @examples
#' \dontrun{
#' suppressMessages(library(cop))
#' 
#' x = matrix(1:4, 2) + 10*comm.rank()
#' mpi_print(x)
#' 
#' finalize()
#' }
#' 
#' @name mpi_print
#' @rdname mpi_print
NULL



#' @rdname mpi_print
#' @export
mpi_cat = function(x, quiet=FALSE, all.rank=FALSE, comm=0L)
{
  printer(cat, x=x, quiet=quiet, all.rank=all.rank, comm=comm)
}



#' @rdname mpi_print
#' @export
mpi_print = function(x, quiet=FALSE, all.rank=FALSE, comm=0L)
{
  printer(print, x=x, quiet=quiet, all.rank=all.rank, comm=comm)
}
