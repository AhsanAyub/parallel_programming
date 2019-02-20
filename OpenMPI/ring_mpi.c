/*
 * This is a MPI Ring program that takes one string and one integar from line argument.
 * Process n will send the string (by appending its rank) to Process n+1 along with incrementing the integar value.
 * At last, Process Comp_size-1 will send the string (by appending its rank) to Process 0.
 *
 * @author Md. Ahsan Ayub
 * @version 1.8 02/20/2019 
 *
 */

#define MAXIMUM_STRING_SIZE 1024

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    //
    if (argc < 3)
    {
        printf("Usuage: mpirun -np <number_of_processes> ./<executable> <No. of Square Matrix Size> <No. of Thread>\n");
        return -1;
    }

    // This is the message that will be passed between processes.
    char* sMessage;
    static int iInput;

    // Specified number of the threads coming from the user
    sMessage = argv[1];
    iInput = atoi(argv[2]);
    iInput -= 1;

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of processes starting from 0 till (world_size - 1)

    // Assuming at least 2 processes for this programs
    if (world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // If the size of message (passed in the line argument) exceeds 1024, program will abort
    if (strlen(sMessage)+1 > MAXIMUM_STRING_SIZE)
    {
        printf("The size of the message is too large (more than %d).\n", MAXIMUM_STRING_SIZE);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }    

    // Ring communication among processes starts from here:
    // Receive from the lower process and Send to the higher process.
    if (world_rank != 0)
    {
        MPI_Recv(sMessage, MAXIMUM_STRING_SIZE, MPI_CHAR, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&iInput, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received '%s' (String) and %d (Integar) from process %d\n", world_rank, sMessage, iInput, world_rank - 1);
        
        // Appending the rank of the process to the message that was received my the process.
        char temp[2];
        sprintf(temp, "%d", world_rank);
        strcat(sMessage, temp);
    }
    
    // Integar input is to be incremented by 1 by each process
    iInput += 1;
    
    // Common send function for each process to to its higher process.
    MPI_Send(sMessage, strlen(sMessage)+1, MPI_CHAR, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
    MPI_Send(&iInput, 1, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
    
    // Process 0 is now ready to receive from the last process. It enables
    // least one MPI_Send is initialized before all MPI_Recvs (to prevent deadlock)
    if (world_rank == 0)
    {
        MPI_Recv(sMessage, MAXIMUM_STRING_SIZE, MPI_CHAR, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&iInput, 1, MPI_INT, world_size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received '%s' (String) and %d (Integar) from process %d\n", world_rank, sMessage, iInput, world_size - 1);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
}

/*
    >> hpcshell --tasks-per-node = 8
    >> mpicc -o main ring_mpi.c
    >> mpirun -np 8 ./main Hello 1
    Process 1 received 'Hello' (String) and 1 (Integar) from process 0
    Process 2 received 'Hello1' (String) and 2 (Integar) from process 1
    Process 3 received 'Hello12' (String) and 3 (Integar) from process 2
    Process 4 received 'Hello123' (String) and 4 (Integar) from process 3
    Process 5 received 'Hello1234' (String) and 5 (Integar) from process 4
    Process 6 received 'Hello12345' (String) and 6 (Integar) from process 5
    Process 7 received 'Hello123456' (String) and 7 (Integar) from process 6
    Process 0 received 'Hello1234567' (String) and 8 (Integar) from process 7
*/