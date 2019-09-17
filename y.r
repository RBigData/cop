suppressMessages(library(cop))

x = matrix(1:4, 2) + 10*comm.rank()

out = qr_allreduce(x)

mpi_cat("\n", quiet=TRUE)

mpi_cat("function:\n", quiet=TRUE)
mpi_print("qr_allreduce")

ar=TRUE
mpi_cat("\ninput:\n", quiet=TRUE)
mpi_print(x, all.rank=ar)

mpi_cat("\noutput:\n", quiet=TRUE)
comm.print(out, all.rank=T)
# mpi_print(out, all.rank=ar)

finalize()
