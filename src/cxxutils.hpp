#ifndef COP_CXXUTILS_H
#define COP_CXXUTILS_H


#ifndef restrict
#define restrict __restrict__
#endif


#define OMPI_SKIP_MPICXX 1
#include <mpi.h>


#include <cstdint>
#include <typeinfo>

template <typename T>
static inline MPI_Datatype mpi_type_lookup(T x)
{
  (void) x;
  
  // C types
  if (typeid(T) == typeid(char))
    return MPI_CHAR;
  else if (typeid(T) == typeid(double))
    return MPI_DOUBLE;
  else if (typeid(T) == typeid(float))
    return MPI_FLOAT;
  else if (typeid(T) == typeid(int))
    return MPI_INT;
  else if (typeid(T) == typeid(long))
    return MPI_LONG;
  else if (typeid(T) == typeid(long double))
    return MPI_LONG_DOUBLE;
  else if (typeid(T) == typeid(long long))
    return MPI_LONG_LONG_INT;
  else if (typeid(T) == typeid(short))
    return MPI_SHORT;
  else if (typeid(T) == typeid(unsigned int))
    return MPI_UNSIGNED;
  else if (typeid(T) == typeid(unsigned char))
    return MPI_UNSIGNED_CHAR;
  else if (typeid(T) == typeid(unsigned long))
    return MPI_UNSIGNED_LONG;
  else if (typeid(T) == typeid(unsigned short))
    return MPI_UNSIGNED_SHORT;
  else if (typeid(T) == typeid(uint32_t))
    return MPI_UINT32_T;
  
  // stdint types
  else if (typeid(T) == typeid(int8_t))
    return MPI_INT8_T;
  else if (typeid(T) == typeid(int16_t))
    return MPI_INT16_T;
  else if (typeid(T) == typeid(int32_t))
    return MPI_INT32_T;
  else if (typeid(T) == typeid(int64_t))
    return MPI_INT64_T;
  else if (typeid(T) == typeid(uint8_t))
    return MPI_UINT8_T;
  else if (typeid(T) == typeid(uint16_t))
    return MPI_UINT16_T;
  else if (typeid(T) == typeid(uint32_t))
    return MPI_UINT32_T;
  else if (typeid(T) == typeid(uint64_t))
    return MPI_UINT64_T;
  
  else
    return MPI_DATATYPE_NULL;
}


#endif
