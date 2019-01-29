#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;

int main (int argc, char *argv[])
{
    // Compute the runtime of the program.
    double wallTime = omp_get_wtime();

    // A static array with even number of index
    int evenArrayLength = 20;
    int evenArray[20] = {101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};

    if (argc < 2)
    {
        cerr << "Usuage: ./program <No. of Thread>" << endl;
        return -1;
    }

    // Specified number of the threads coming from the user
    int thread_count = atoi(argv[1]);
    int chunk = ceil((float)evenArrayLength / thread_count);
    int startIndex = -1, endIndex = -1;

    //Getting into the parallel region
    #pragma omp parallel num_threads(thread_count)
    {
        int myRank = omp_get_thread_num(); //What thread am I?

        // Executed one thead at a time
        #pragma omp critical
        {
            // Computing two varibles to buffer through the array which are derivatives from myRank variable
            startIndex = myRank * chunk;
            endIndex = min((startIndex + chunk), evenArrayLength) - 1;
            
            cout << endl << " ------------------------------------- " << endl;
            cout << "Thread Number: " << myRank << ", Start index: " << startIndex << ", and End Index: " << endIndex << endl;
            
            if(startIndex < evenArrayLength) // Important if statement to deal with random thread numbers 
            {
                // Buffering through the array respective of start index and end index
                for(int i = startIndex; i <= endIndex; i++)
                    cout << evenArray[i] << ' ';
            }
            else
                cout << "No need to buffer through the array and empty thread assignment";
        }
    }

    // Displaying the code run time
    cout << endl << "Code compilation time: " << wallTime << endl;
    return 0;
}