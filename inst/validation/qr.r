suppressMessages(library(cop))
comm.set.seed(1234, diff=TRUE)

m = 100
n = 4
x = qr.R(qr(matrix(rnorm(m*n), m, n), LAPACK=TRUE))

R_ar = qr_reduce(x)

x_g = gather(x)
if (comm.rank() == 0){
  x_all = do.call(rbind, x_g)
  R = qr.R(qr(x_all, LAPACK=TRUE))
  
  test = all.equal(R, R_ar)
  print(test)
}

finalize()
