suppressMessages(library(Matrix))
suppressMessages(library(cop))
comm.set.seed(1234, diff=TRUE)

n = 1000
prop_dense = .001



gen_sparse = function(m, n, prop_dense=0.001)
{
  n = as.integer(n)
  s = new("dgCMatrix", Dim=c(n, n), p=rep(0L, n+1))
  
  num_dense = as.integer(n*n*prop_dense)
  nzrows = sort(sample(n*n, size=num_dense, replace=FALSE))
  s[nzrows] = 1
  
  s
}



s = gen_sparse(n, n, prop_dense=prop_dense)
t = comm.timer({
  ret <- spadd_allreduce(s)
})

mpi_cat(sprintf("input density: %.2f%%\n", 100*prop_dense), quiet=TRUE)
mpi_cat(sprintf("outpt density: %.2f%%\n", 100*nnzero(ret)/prod(dim(s))), quiet=TRUE)
mpi_cat(sprintf("run time: %.3f\n", t[["max"]]), quiet=TRUE)


finalize()
