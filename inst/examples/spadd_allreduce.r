suppressMessages(library(Matrix))
suppressMessages(library(cop))

x = matrix(0, 10, 3)
x[3, 1] = 1
x[6, 1] = 2
x[2, 2] = 3
x[4, 2] = 4
x[7, 2] = 5
x[1, 3] = 6
x[2, 3] = 7
s = as(x, "dgCMatrix")
comm.print(s)

ret = spadd_allreduce(s)
comm.print(ret)


finalize()
