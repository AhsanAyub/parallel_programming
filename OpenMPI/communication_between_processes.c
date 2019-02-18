// This is a sample program to understand MPI_Send and MPI_Recv
// The program is designed to send a message from Process 0 and
// Process 1 will receive it.
// Other proceesses have got no functionalities.

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // This is the message that will be passed between processes.
    char msg[50] = "This is a sample message.";

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of processes starting from 0 till (world_size - 1)

    // Assuming at least 2 processes for this task
    if (world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    //Communication between processes feature starts from here
    if(world_rank == 0) // Process 0
    {
        /*
            MPI_Send(
                        void* data,
                        int count,
                        MPI_Datatype datatype,
                        int destination,
                        int tag,
                        MPI_Comm communicator)
        */
        MPI_Send(&msg, 50, MPI_CHAR, 1, 0, MPI_COMM_WORLD); // Destination is specified as 1
        printf("Process %d sent to Process 1: %s\n", world_rank, msg);
    }
    else if(world_rank == 1) // Process 1
    {
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
        MPI_Recv(&msg, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Source is specified as 0
        printf("Process %d received from process 0: %s\n", world_rank, msg);
    }
    else // Other processes
        printf("Process %d is unused.\n",world_rank);

    // Finalize the MPI environment.
    MPI_Finalize();
}

/*
    >> hpcshell --tasks-per-node = 2
    >> mpicc -o main communication_between_processes.c
    >> mpirun -np 2 ./main
    Process 0 sent to Process 1: This is a sample message.
    Process 1 received from process 0: This is a sample message.

    >> mpirun -np 4 ./main
    Process 0 sent to Process 1: This is a sample message.
    Process 2 is unused.
    Process 3 is unused.
    Process 1 received from process 0: This is a sample message.
*/
