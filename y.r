suppressMessages(library(cop))

x = matrix(1:4, 2) + 10*comm.rank()

mpi_cat("\n", quiet=TRUE)

mpi_cat("function:\n", quiet=TRUE)
mpi_print("qr_allreduce")

ar=FALSE
mpi_cat("\ninput:\n", quiet=TRUE)
mpi_print(x, all.rank=ar)

out = qr_allreduce(x)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out, all.rank=ar)

out = qr_allreduce(x, type="float")
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out, all.rank=ar)

finalize()
