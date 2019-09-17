#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int m, n;
MPI_Comm comm;
int rank, size;


static inline void print(int m, int n, int *x)
{
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
      printf("%d ", x[i + m*j]);
    
    putchar('\n');
  }
}


static inline void allprint(int *x)
{
  for (int r=0; r<size; r++)
  {
    if (rank == 0)
    {
      if (r != 0)
      {
        MPI_Status status;
        MPI_Recv(x, m*n, MPI_INT, r, 0, comm, &status);
      }
      printf("(rank=%d)\n", r);
      print(m, n, x);
    }
    else if (r == rank)
      MPI_Send(x, m*n, MPI_INT, 0, 0, comm);
  }
}


static void mat_op_eval_R(double *a, double *b, int *len, MPI_Datatype *dtype)
{
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<m; j++)
      b[i + m*j] += a[i + m*j];
  }
}




int main()
{
  MPI_Init(NULL, NULL);
  comm = MPI_COMM_WORLD;
  m = 2;
  n = 2;
  size_t len = (size_t) m*n*sizeof(double);
  
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &size);
  
  int *send_data = malloc(len);
  for (int i=0; i<m*n; i++)
    send_data[i] = i+1 + 10*rank;
  
  int *recv_data = malloc(len);
  
  // allprint(send_data, recv_data);
  // MPI_Barrier(comm);
  // memset(recv_data, 0, len);
  
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_INT, &mat_type);
  MPI_Type_commit(&mat_type);
  
  MPI_Op op;
  MPI_Op_create((MPI_User_function*) mat_op_eval_R, 1, &op);
  MPI_Allreduce(send_data, recv_data, 1, mat_type, op, comm);
  MPI_Op_free(&op);
  
  MPI_Type_free(&mat_type);
  
  if (rank == 0) putchar('\n');
  allprint(recv_data);
  MPI_Barrier(comm);
  
  MPI_Finalize();
  return 0;
}
