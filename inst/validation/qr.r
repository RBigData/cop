suppressMessages(library(cop))
comm.set.seed(1234, diff=TRUE)

getR = function(x) qr.R(qr(x, LAPACK=TRUE))

m = 100
n = 5
x = getR(matrix(rnorm(m*n), m, n))

R_ar = qr_reduce(x)

op = function(a, b) getR(rbind(a, b))
R_ar_cop = cop_allreduce(x, op, commutative=TRUE)

if (comm.rank() == 0){
  test = all.equal(abs(R_ar), abs(R_ar_cop))
  print(test)
}

x_g = gather(x)
if (comm.rank() == 0){
  R_i = x_g[[1]]
  for (i in seq.int(comm.size()-1)+1){
    R_i = op(R_i, x_g[[i]])
  }
  test = all.equal(abs(R_ar), abs(R_i))
  print(test)

  x_all = do.call(rbind, x_g)
  R = getR(x_all)
  test = all.equal(abs(R_ar), abs(R))
  print(test)
}


finalize()
