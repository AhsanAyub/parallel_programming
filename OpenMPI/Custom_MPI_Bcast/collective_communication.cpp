// This is a sample program to understand MPI_Send and MPI_Recv
// The program is designed to send a message from Process 0 and
// Process 1 will receive it.
// Other proceesses have got no functionalities.

#include <iostream>
#include <mpi.h>
#include <cstring>
#include <cstdlib>

// Including custom MPI Bcast implementation library (Credit: Md. Ahsan Ayub)
#include "maa_bcast.h"

using namespace std;

const int iArraySize = 100000;

int main(int argc, char* argv[])
{
    // This is the message that will be passed to all the processes from root.
    double *dMessageCustom, *dMessageDefault;

    // Execution time calculation variables for custom bcast and default implementation
    double dCustomBcastExecutionTime = 0.0, dDefaultBcastExecutionTime = 0.0, startTime, endTime;

    // Creating the array
    dMessageCustom = new double[iArraySize];
    dMessageDefault = new double[iArraySize];

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size, world_rank, root = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of processes starting from 0 till (world_size - 1)

    // Assuming at least 2 processes for this task
    if (world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Let all the processes get synchronized
    MPI_Barrier(MPI_COMM_WORLD);

    // The loop is defined to obtain the measurement of the execution time by three times
    for(int iExecution = 1; iExecution <= 3; iExecution++)  
    {
        // Measure the clock time before custom bcast implementation
        startTime = MPI_Wtime();

        if(world_rank == root)
        {
            // Storing random value to the array (custom bcast) by the root process only.
            for(int i = 0; i < iArraySize; i++)
                // Just a random expression that'll give us a dobule value as a result!
                dMessageCustom[i] = (double)((iArraySize * (i+1)) / (iExecution * 0.235464));
        }

        // Custom broatcast message command
        cout << "Process " << world_rank << " uses 'custom' BCast method." << endl;
        maaMPI_Bcast(dMessageCustom, iArraySize, MPI_DOUBLE, root, MPI_COMM_WORLD);

        // As a proof that custom Bcast implementation is working properly, the buffer can be printed bellow.
        if(world_rank != root)
        {
            cout << "Proces " << world_rank << " is printing the buffer:\n";

            //for(int i; i < iArraySize; i++)   // No need to print out the whole buffer message
                                                //as it will unnecessarily increase the execution time
            for(int i =0; i < 5; i++) // N
                cout << dMessageCustom[i]<< " \t";
            cout << endl;
        }

        // Let all the processes get synchronized
        MPI_Barrier(MPI_COMM_WORLD);

        // Measure the clock time before custom bcast implementation
        endTime = MPI_Wtime();

        // Store the execution time to the custom bcast time execution variable
        dCustomBcastExecutionTime += (endTime - startTime);

        // Let all the processes get synchronized
        MPI_Barrier(MPI_COMM_WORLD);

        // Measure the clock time before applying the default bcast
        startTime = MPI_Wtime();

        if(world_rank == root)
        {
            // Storing random value to the array (default bcast) by the root process only.
            for(int i = 0; i < iArraySize; i++)
                // Just a random expression that'll give us a dobule value as a result!
                dMessageDefault[i] = (double)((iArraySize * (i+1)) / (iExecution * 0.86454657));
        }
        
        /*
            MPI_Bcast(
                          void *buffer,
                          int count,
                          MPI_Datatype datatype,
                          int root,
                          MPI_Comm comm
            );
        */

        // Default broatcast message command
        cout << "Process " << world_rank << " uses 'built-in' BCast method." << endl;
        MPI_Bcast(dMessageDefault, iArraySize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // As a proof that custom Bcast implementation is working properly, the buffer can be printed bellow.
        if(world_rank != root)
        {
            cout << "Proces " << world_rank << " is printing the buffer:\n";

            //for(int i; i < iArraySize; i++)   // No need to print out the whole buffer message
                                                //as it will unnecessarily increase the execution time
            for(int i =0; i < 5; i++) // N
                cout << dMessageDefault[i]<< " \t";
            cout << endl;
        }

        // Let all the processes get synchronized
        MPI_Barrier(MPI_COMM_WORLD);

        // Measure the clock time before custom bcast implementation
        endTime = MPI_Wtime();

        // Store the execution time to the custom bcast time execution variable
        dDefaultBcastExecutionTime += (endTime - startTime);
    }
    
    // Let all the processes get synchronized
    MPI_Barrier(MPI_COMM_WORLD);

    if(world_rank == root)    
    {
        // Calcuation the execution time for both cases and measure the average.
        dCustomBcastExecutionTime /= 3;
        dDefaultBcastExecutionTime /= 3;

        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        cout << "Number of defined processes: " << world_size << endl;
        cout << "Custom MPI BCast implementation's execution time: " << (dCustomBcastExecutionTime * 1000) << " miliseconds" << endl;
        cout << "Default MPI BCast implementation's execution time: " << (dDefaultBcastExecutionTime * 1000) << " miliseconds" << endl;
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    }

    // Clearing the memory
    delete[] dMessageCustom;
    delete[] dMessageDefault;

    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}

/*
    >> hpcshell --ntasks-per-node=4
    >> mpic++ -o main collective_communication.cpp maa_bcast.cpp
    >> mpirun -np 4 ./main
*/