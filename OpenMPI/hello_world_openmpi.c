// A simple MPI hello world program

#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the total number of processes in world_size variable
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process in world_rank variable
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor and storing it to processor_name variable
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s: rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();
}

/*
Execution processes:
>> hpcshell --tasks-per-node=4
>> mpicc -o main hello_world_openmpi.c
>> mpirun -np 4 ./main
Hello world from processor <node_name>: rank 0 out of 4 processors
Hello world from processor <node_name>: rank 1 out of 4 processors
Hello world from processor <node_name>: rank 2 out of 4 processors
Hello world from processor <node_name>: rank 3 out of 4 processors
*/