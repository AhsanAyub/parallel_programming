#if !defined MAA_BCAST_H
#define MAA_BCAST_H

// Including libraries
#include <iostream>
#include <mpi.h>
#include <cstring>
#include <cstdlib>

// Signature of the methods
void maaMPI_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);

#endif