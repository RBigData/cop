suppressMessages(library(float))

set.seed(1234)
row_scaling_factor = 4

ompthreads = function()
{
  threads = as.numeric(Sys.getenv("OMP_NUM_THREADS"))
  if (is.na(threads))
    0L
  else
    threads
}

m = 2500 * row_scaling_factor
n = 2500
x = matrix(runif(m*n), m, n)

t_dbl = system.time(out_double <- qr.R(qr(x, LAPACK=TRUE)))
t_flt = system.time(out_float <- dbl(qr.R(qr(fl(x)))))

cat("method,m,n,size,threads,t_dbl_min,t_dbl_mean,t_dbl_max,t_flt_min,t_flt_mean,t_flt_max\n")
data = paste(
  "stack", m, n, 1, ompthreads(),
  t_dbl[[3]], t_dbl[[3]], t_dbl[[3]],
  t_flt[[3]], t_flt[[3]], t_flt[[3]],
  sep=","
)
cat(data)
cat("\n")
