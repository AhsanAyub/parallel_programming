/*
 * This is a MPI Matrix Multiplicaiton program that takes one integar from line argument which denotes the size
 * of the square matrix. Process 0 will initalize the matrix with double type random number and perform
 * row-wise partioning.
 *
 * @author Md. Ahsan Ayub
 * @version 1.4 03/11/2019 
 *
 */

#include <iostream>
#include <mpi.h>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
    // Check whether user passes a valid line argument
    if (argc >= 3)
    {
        printf("Usuage: mpirun -np <number_of_processes> ./<executable> <No. of Square Matrix Size>\n");
        return -1;
    }

    // Time clock variables to calculate the start and end time of execution
    double dStartTime = 0, dEndTime = 0;

    // Specified number of the threads coming from the user
    int iSize = atoi(argv[1]);
    int iSquareMatrixTotalElements = iSize * iSize;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Calculate the starting time of the program
    dStartTime = MPI_Wtime();
    
    //Creating dynanic array
    double *dA, *dB, *dResult;

    // Get the number of processes
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of processes starting from 0 till (world_size - 1)

    // Calculation of the chunk size and remainder (if any) to equally distribute the partioning and let
    // the last process have the extra tasks (remainder)
    int iChunk = iSize / world_size;
    int iRem = iSize % world_size;
    
    // Process 0 will initialize the matrix and distribute the tasks to other processes
    if (world_rank == 0)
    {
        // Allocation of the Square matrix
        dA = new double[iSquareMatrixTotalElements];
        dB = new double[iSquareMatrixTotalElements];

        // Initializing random double values into A and B matrix
        for(int i = 0; i < iSize; i++)
        {
            for(int j = 0; j < iSize; j++)
            {
                dA[(iSize * i) + j] = (double)(iSize * i) + (j + 1);
                dB[(iSize * i) + j] = (double)(iSize * i) + (j + 1);
            }
        }

        // Process 0 will send the values of B matrix to other processes
        for(int i = 1; i < world_size; i++)

            /*
            MPI_Send(
                        void* data,
                        int count,
                        MPI_Datatype datatype,
                        int destination,
                        int tag,
                        MPI_Comm communicator)
            */
    
            MPI_Send(dB, iSquareMatrixTotalElements, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);

        // Process 0 is to send the A matrix elements by employing the partion of its elements
        for(int i = 1; i < world_size; i++) 
        {
            // Last process will avail the chuck size and the remainder
            if(i == (world_size - 1))
            {
                // Calculate the buffer size
                int iSizeOfTheBuffer = (iChunk * iSize) + (iRem * iSize);

                // Sending the buffer to the last process
                MPI_Send(&dA[i * iChunk * iSize], iSizeOfTheBuffer, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            }

            // Rest of the other processes (1 to n-1) will avail the chunk size partioning only.
            else 
            {
                // Calculate the buffer size
                int iSizeOfTheBuffer = iChunk * iSize;

                // Sending the buffer to process 1 to process n-1
                MPI_Send(&dA[i * iChunk * iSize], iSizeOfTheBuffer, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
            }
        }
    }

    // Other processes will catch the below else block
    else
    {
        // Allocating the B matrix dynamically
        dB = new double[iSquareMatrixTotalElements];

        /*
            MPI_Recv(
                        void* data,
                        int count,
                        MPI_Datatype datatype,
                        int source,
                        int tag,
                        MPI_Comm communicator,
                        MPI_Status* status)
        */

    
        // Receiving the elements of the B matrix sent from process 0
        MPI_Recv(dB, iSquareMatrixTotalElements, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Inializating the A matrix which was sent with partioned elements 
        if(world_rank != (world_size - 1))
        {
            // No need to worry about iRem variable to calculate the buffer
            int iSizeOfTheBuffer = iChunk * iSize;

            // Alocating the array
            dA = new double[iSizeOfTheBuffer];
            
            // Receiving the elements from process 0
            MPI_Recv(dA, iSizeOfTheBuffer, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else // True for last process
        {
            // Remainder value will be added into partioning for the last process
            int iSizeOfTheBuffer = (iChunk * iSize) + (iRem * iSize);

            // Allocating the aray
            dA = new double[iSizeOfTheBuffer];

            // Receiving the elements from process 0
            MPI_Recv(dA, iSizeOfTheBuffer, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // Time to initialize the Result matrix
    if(world_rank == 0)
        dResult = new double[iSquareMatrixTotalElements];
    
    else if (world_rank == (world_size - 1))
    {
        iChunk = iChunk + iRem;
        dResult = new double[iChunk * iSize];
    }
    
    else 
        dResult = new double[iChunk * iSize];

    // Initialization of calculated values in the result matrixs
    for(int i = 0; i < iChunk; i++)
    {
        for(int j = 0; j < iSize; j++)
        {
            dResult[(i * iSize) + j] = 0; // Storing 0 value as static means

            // Matrix multiplication calculation: Result = A * B
            for(int k = 0; k < iSize; k++) 
                dResult[(i * iSize) + j] += dA[(i * iSize) + k] * dB[(k * iSize) + j];
        }
    }

    // Calculated result will now need to pass to Processes 0 by other processes
    // Note: Tag is defined as world_rank to understand which processes sends the buffer
    if(world_rank != 0)
        MPI_Send(dResult, (iChunk * iSize), MPI_DOUBLE, 0, world_rank, MPI_COMM_WORLD);

    // Process 0 will execute the following block of codes to merge the 
    else
    {
        // Receiving calculated values from all the processes (1 to n)
        for(int i = 1; i < world_size; i++)
        {
            if(i == (world_size - 1)) // True for the last process
            {
                // Again, remainder needs to be calculated for the final process
                int iSizeOfTheBuffer = (iChunk * iSize) + (iRem * iSize);

                // Receive from the last processes
                MPI_Recv(&dResult[i * iChunk * iSize], iSizeOfTheBuffer, MPI_DOUBLE, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
            {
                // Calculating generic buffer size for rest of the processes
                int iSizeOfTheBuffer = iChunk * iSize;

                // Receive from the last processes
                MPI_Recv(&dResult[i * iChunk * iSize], iSizeOfTheBuffer, MPI_DOUBLE, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }

    // Wait for other processes to come at this point to calculate the end time
    MPI_Barrier(MPI_COMM_WORLD);
    dEndTime = MPI_Wtime();

    // Finalize the MPI environment.
    MPI_Finalize();

    // Clearing the memory
    delete[] dA;
    delete[] dB;
    delete[] dResult;

    // Calculating the execution time of the program
    if(world_rank == 0)
        cout << "Matrix Size: " << iSize << ", # of Processes: " << world_size << ", Time: " << (dEndTime - dStartTime) << endl;

    return 0;
}

/*
    >> hpcshell --tasks-per-node = 8
    >> mpic++ -o main mpi_matrix_multiplication.cpp
    >> mpirun -np 4 ./main 100
    Time: 1.42
*/