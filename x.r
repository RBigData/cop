suppressMessages(library(cop))

mult_plus_one = function(a, b) a+b #qr.R(qr(rbind(a, b)))

x = matrix(1:4, 2) + 10*comm.rank()

out = cop_allreduce(x, fun=mult_plus_one, commutative=TRUE)

mpi_cat("\n", quiet=TRUE)

mpi_cat("function:\n", quiet=TRUE)
mpi_print(mult_plus_one)

ar=TRUE
mpi_cat("\ninput:\n", quiet=TRUE)
mpi_print(x, all.rank=ar)

mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out, all.rank=ar)

finalize()
