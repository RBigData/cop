suppressMessages(library(cop))

x = matrix(1:4, 2) + 10*comm.rank()

add = function(a, b) a+b
out_all = cop_allreduce(x, op=add, commutative=TRUE)
out = cop_reduce(x, op=add, commutative=TRUE)

mpi_cat("\ninput:\n", quiet=TRUE)
mpi_print(x, all.rank=TRUE)

mpi_cat("\nfunction:\n", quiet=TRUE)
mpi_print(add)
mpi_cat("\noutput_all:\n", quiet=TRUE)
mpi_print(out_all, all.rank=TRUE)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out, all.rank=TRUE)


R = function(a, b) qr.R(qr(rbind(a, b)))
out_all = cop_allreduce(x, op=R, commutative=TRUE)
out = cop_reduce(x, op=R, commutative=TRUE)

mpi_cat("\nfunction:\n", quiet=TRUE)
mpi_print(R)
mpi_cat("\noutput_all:\n", quiet=TRUE)
mpi_print(out_all, all.rank=TRUE)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out, all.rank=TRUE)

finalize()
