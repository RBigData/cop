suppressMessages(library(cop))
comm.set.seed(1234, diff=TRUE)

ompthreads = function()
{
  threads = as.numeric(Sys.getenv("OMP_NUM_THREADS"))
  if (is.na(threads))
    0L
  else
    threads
}

m = 2500
n = 2500
x = matrix(runif(m*n), m, n)

barrier()
t_double = comm.timer(out_double <- qr_reduce(x))
barrier()
t_float = comm.timer(out_float <- qr_reduce(x, type="float"))

size = comm.size()
mpi_cat("method,m,n,size,threads,t_dbl_min,t_dbl_mean,t_dbl_max,t_flt_min,t_flt_mean,t_flt_max\n", quiet=TRUE)
data = paste(
  "reduce", m*size, n, size, ompthreads(),
  t_double[["min"]], t_double[["mean"]], t_double[["max"]],
  t_float[["min"]], t_float[["mean"]], t_float[["max"]],
  sep=","
)
mpi_cat(data, quiet=TRUE)
mpi_cat("\n", quiet=TRUE)

finalize()
