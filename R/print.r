rankcat = function(rank, quiet)
{
  if (!isTRUE(quiet))
    cat(paste0("(rank=", rank, ")\n"))
  
 invisible()
}

printer = function(printfun, msg, quiet=FALSE, all.rank=FALSE)
{
  myrank = pbdMPI::comm.rank()
  
  if (myrank == 0)
  {
    rankcat(myrank, quiet)
    printfun(msg)
  }
  
  if (all.rank)
  {
    for (rank in (seq_len(comm.size()) - 1L)[-1])
    {
      if (myrank == 0)
      {
        rankmsg_len = pbdMPI::spmd.recv.integer(integer(1), rank.source=rank)
        rankmsg = pbdMPI::recv(character(rankmsg_len), rank.source=rank)
        rankcat(rank, quiet)
        cat(rankmsg)
      }
      else
      {
        cmsg = paste(capture.output(printfun(msg)), "\n", collapse="")
        pbdMPI::spmd.send.integer(nchar(cmsg), rank.dest=0)
        pbdMPI::send(cmsg, rank.dest=0)
      }
    }
  }
  
  pbdMPI::barrier()
}



#' @export
mpi_cat = function(msg, quiet=FALSE, all.rank=FALSE)
{
  printer(cat, msg, quiet, all.rank)
}



#' @export
mpi_print = function(msg, quiet=FALSE, all.rank=FALSE)
{
  printer(print, msg, quiet, all.rank)
}
