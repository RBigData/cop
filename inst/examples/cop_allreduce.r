suppressMessages(library(cop))

x = matrix(1:4, 2) + 10*comm.rank()

add = function(a, b) a+b
out = cop_allreduce(x, fun=add, commutative=TRUE)

mpi_cat("\ninput:\n", quiet=TRUE)
mpi_print(x, all.rank=TRUE)

mpi_cat("\nfunction:\n", quiet=TRUE)
mpi_print(add)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out)


R = function(a, b) qr.R(qr(rbind(a, b)))
out = cop_allreduce(x, fun=R, commutative=TRUE)

mpi_cat("\nfunction:\n", quiet=TRUE)
mpi_print(R)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out)

finalize()
