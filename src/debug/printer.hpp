#ifndef COP_DEBUG_PRINTER_H
#define COP_DEBUG_PRINTER_H


#include <cstdio>

template <typename REAL>
static inline void matprinter(int m, int n, REAL *x)
{
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
      printf("%.2f ", (REAL) x[i + m*j]);
    
    putchar('\n');
  }
  putchar('\n');
}


#endif
