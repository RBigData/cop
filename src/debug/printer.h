#ifndef COP_UTILS_H
#define COP_UTILS_H


#include <stdio.h>

static inline void matprinter(int m, int n, double *x)
{
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
      printf("%.2f ", x[i + m*j]);
    
    putchar('\n');
  }
  putchar('\n');
}


#endif
