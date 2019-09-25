#' @useDynLib cop cop_allreduce_spmat_add



#' @export
spadd_allreduce = function(x, comm=0L)
{
  .Call(cop_allreduce_spmat_add, x, comm)
}

#' @export
spadd_reduce = function(x, root=0, comm=0L)
{

}
