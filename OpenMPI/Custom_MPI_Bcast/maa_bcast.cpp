// Including the custom MPI Bcast implementation library (Credit: Md. Ahsan Ayub)
#include "maa_bcast.h"

using namespace std;

// Implementation of the signature method defined in maa_bcast.h header file
void maaMPI_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
{
    // Get the number of processes
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of processes starting from 0 till (world_size - 1)

    cout << "Process " << world_rank << " is here.\n";

    if (world_rank == root)
    {
        // The root process is going to broadcast the message to other used defined processes
        for (int i = 0; i < world_size; i++)
        {
            if (i != world_rank)
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

                MPI_Send(buffer, count, datatype, i, 0, communicator);
                cout << "Process " << world_rank << " executes the MPI_Send funciton and broadcasts the message." << endl;
            }
            else    // Root process isn't required to send the message to itself.
                continue;
        }
    }
    // Other processes need to gather the message sent by the root.
    else
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

        MPI_Recv(buffer, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
        cout << "Process " << world_rank << " executes the MPI_Recv funciton and gathers the message sent by the root." << endl;
    }
}