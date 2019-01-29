#include <iostream>
#include <omp.h>

using namespace std;

int main (int argc, char *argv[])
{
    // Compute the runtime of the program.
    double wallTime = omp_get_wtime();

    if (argc < 2)
    {
        cerr << "Usuage: ./program <No. of Thread>" << endl;
        return -1;
    }

    // Specified number of the threads coming from the user
    int thread_count = atoi(argv[1]);

    //Getting into the parallel region
    #pragma omp parallel num_threads(thread_count)
    {
        int my_rank, num_threads;
        my_rank = omp_get_thread_num(); //What thread am I?
        num_threads = omp_get_num_threads(); // Get number of threads

        #pragma omp critical
        {
            if(my_rank == 0) //At the thread number of 0, it will be displayed
                cout << "No of thead spawned: " << num_threads << endl;
            
            cout << "Hello from thead # " << my_rank + 1 << " out of " << num_threads << " thread(s)." << endl;
        }
    }

    // Displaying the code run time
    cout << "Code compilation time: " << wallTime << endl;
    return 0;
}
