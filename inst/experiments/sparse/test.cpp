#include <cstdint>
#include <iostream>

#include <Eigen/SparseCore>


int main()
{
  int n = 5;
  
  Eigen::SparseMatrix<uint32_t, Eigen::StorageOptions::ColMajor, int> x(n, n);
  x.makeCompressed();
  x.reserve(4);
  x.insert(0, 0) = 1;
  x.insert(2, 4) = 1;
  
  Eigen::SparseMatrix<uint32_t, Eigen::StorageOptions::ColMajor, int> y(n, n);
  y.makeCompressed();
  y.reserve(4);
  y.insert(0, 1) = 1;
  y.insert(2, 4) = 1;
  
  x = x+y;
  std::cout << x << std::endl;
  
  // x.resize(rows, cols);
  // x.makeCompressed();
  // x.resizeNonZeros(nnz);
  // memcpy(x.valuePtr(), values, nnz * sizeof(*values));
  // memcpy(x.outerIndexPtr(), P, P_len * sizeof(*P));
  // memcpy(x.innerIndexPtr(), I, nnz * sizeof(*I));
  
  int nnz = x.nonZeros();
  uint32_t *X = x.valuePtr();
  std::cout << "X: ";
  for (int i=0; i<nnz; i++)
    std::cout << X[i] << " ";
  std::cout << std::endl;
  
  int *I = x.innerIndexPtr();
  std::cout << "I: ";
  for (int i=0; i<nnz; i++)
    std::cout << I[i] << " ";
  std::cout << std::endl;
  
  int P_len = x.outerSize();
  int *P = x.outerIndexPtr();
  std::cout << "P: ";
  for (int i=0; i<P_len; i++)
    std::cout << P[i] << " ";
  std::cout << std::endl;
  
  return 0;
}
