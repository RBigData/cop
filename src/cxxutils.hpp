#ifndef COP_CXXUTILS_H
#define COP_CXXUTILS_H
#pragma once


#ifndef restrict
#define restrict __restrict__
#endif


#define OMPI_SKIP_MPICXX 1
#include <mpi.h>


#define TRY_CATCH(expr) try { expr; } catch(const std::exception& e) { error(e.what()); }


#endif
