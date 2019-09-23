# cop

* **Version:** 0.1-0
* **URL**: https://github.com/RBigData/cop
* **License:** [BSD 2-Clause](http://opensource.org/licenses/BSD-2-Clause)
* **Author:** Drew Schmidt


Custom OPs for reductions with MPI.


## Installation

The development version is maintained on GitHub:

```r
remotes::install_github("RBigData/cop")
```



## API

Dense, numeric matrix reductions:

| Method | Explanation |
|--------|-------------|
| `cop_allreduce()` | Reduction that accepts a custom operation (R function). The function has some strong caveats; see `?cop::cop_allreduce` for details. |
| `cop_reduce()` |  |
| `qr_allreduce()` | Reduction where each process owns the R matrix of a QR decomposition, and the reduction is (conceptually) `op = function(a, b) qr.R(qr(a, b))` |
| `qr_reduce()` |  |

The difference between the `_reduce()` and the `_allreduce()` variants is that with the former, only the process specified by the `root` argument receives the return.

The package also has some helper utilities:

| Method | Explanation |
|--------|-------------|
| `mpi_cat()` | Helper `cat()` function. |
| `mpi_print()` | Helper `print()` function. |

These are similar to `pbdMPI::comm.cat()` and `pbdMPI::comm.print()`, although here printing is guaranteed to occur in rank order.



## Examples

For the sake of example, we can build a matrix addition custom reducer:

```r
suppressMessages(library(cop))

x = matrix(1:4, 2) + 10*comm.rank()

add = function(a, b) a+b
out = cop_allreduce(x, op=add, commutative=TRUE)

mpi_cat("\nfunction:\n", quiet=TRUE)
mpi_print(add)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out)

finalize()
```

Most of this is just printing and boilerplate, but even so it's still pretty simple. If we save this as `add.r` and run it via `mpirun -np 3 Rscript add.r`, we see:

```
function:
(rank=0)
function (a, b) 
a + b

output:
(rank=0)
     [,1] [,2]
[1,]   33   39
[2,]   36   42
```

With three ranks, conceptually this is the same as calculating:

```r
add = function(a, b) a+b
add(x, add(x+10, x+20))
##      [,1] [,2]
## [1,]   33   39
## [2,]   36   42
```

As a more substantive example, we can build a matrix product reducer.

```r
suppressMessages(library(cop))

x = matrix(1:4, 2) + 10*comm.rank()

mult = function(a, b) a %*% b
out = cop_allreduce(x, op=mult, commutative=FALSE)

mpi_cat("\nfunction:\n", quiet=TRUE)
mpi_print(mult)
mpi_cat("\noutput:\n", quiet=TRUE)
mpi_print(out)

finalize()
```

As before, most of the code is just boilerplate/printing. Really we only change the reducing function from the above. Note that we have to specify that the operation is not commutative.

If we save this as `mult.r` and run it via `mpirun -np 3 Rscript mult.r`, we see:

```
function:
(rank=0)
function (a, b) 
a %*% b

output:
(rank=0)
     [,1] [,2]
[1,] 2197 2401
[2,] 3274 3578
```

With three ranks, conceptually this is the same as calculating:

```r
x = matrix(1:4, 2)
mult = function(a, b) a %*% b
mult(x, mult(x+10, x+20))
##      [,1] [,2]
## [1,] 2197 2401
## [2,] 3274 3578
```

You can find more examples in the `inst/examples/` directory of the package source, or in `examples/` of the installed package, located at `system.file("examples", package="cop")`.
