suppressMessages(library(cop))
comm.set.seed(1234, diff=TRUE)

m = 100
n = 4
x = qr.R(qr(matrix(rnorm(m*n), m, n), LAPACK=TRUE))

R_ar = qr_reduce(x)

x_g = gather(x)
if (comm.rank() == 0){
  R_i = x_g[[1]]
  for (i in seq.int(comm.size()-1)+1){
    R_i = qr.R(qr(rbind(R_i, x_g[[i]]), LAPACK=TRUE))
  }
  test = all.equal(R_ar, R_i)
  print(test)
  
  x_all = do.call(rbind, x_g)
  R = qr.R(qr(x_all, LAPACK=TRUE))
  test = all.equal(R_ar, R)
  print(test)
}

finalize()
