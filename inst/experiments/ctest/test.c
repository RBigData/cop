#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int m, n;
size_t len;
MPI_Comm comm;


static inline void print(int *x)
{
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
      printf("%d ", x[i + m*j]);
    
    putchar('\n');
  }
}



static inline void allprint(int rank, int size, int *x)
{
  int *buf;
  if (rank == 0)
    buf = malloc(len);
  
  for (int r=0; r<size; r++)
  {
    if (rank == 0)
    {
      if (r == 0)
        memcpy(buf, x, len);
      else
      {
        MPI_Status status;
        MPI_Recv(buf, m*n, MPI_INT, r, 0, comm, &status);
      }
      printf("(rank=%d)\n", r);
      print(buf);
    }
    else if (r == rank)
      MPI_Send(x, m*n, MPI_INT, 0, 0, comm);
  }
  
  if (rank == 0)
    free(buf);
}



static void custom_op(int *a, int *b, int *len, MPI_Datatype *dtype)
{
  (void)len;
  (void)dtype;
  
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<m; j++)
      b[i + m*j] += a[i + m*j];
  }
}




int main()
{
  // mpi setup
  MPI_Init(NULL, NULL);
  comm = MPI_COMM_WORLD;
  
  int rank;
  MPI_Comm_rank(comm, &rank);
  int size;
  MPI_Comm_size(comm, &size);
  
  // data setup
  m = 2;
  n = 2;
  len = (size_t) m*n*sizeof(int);
  
  int *send_data = malloc(len);
  for (int i=0; i<m*n; i++)
    send_data[i] = i+1 + 10*rank;
  
  int *recv_data = malloc(len);
  
  allprint(rank, size, send_data);
  MPI_Barrier(comm);
  
  // custom data type
  MPI_Datatype mat_type;
  MPI_Type_contiguous(m*n, MPI_INT, &mat_type);
  MPI_Type_commit(&mat_type);
  
  // custom op + reduce
  MPI_Op op;
  MPI_Op_create((MPI_User_function*) custom_op, 1, &op);
  MPI_Allreduce(send_data, recv_data, 1, mat_type, op, comm);
  MPI_Op_free(&op);
  
  MPI_Type_free(&mat_type);
  
  // print and cleanup
  if (rank == 0) putchar('\n');
  allprint(rank, size, recv_data);
  MPI_Barrier(comm);
  
  free(send_data);
  free(recv_data);
  
  MPI_Finalize();
  return 0;
}
