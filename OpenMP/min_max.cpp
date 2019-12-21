#include <iostream>
#include <stdlib.h>
#include <omp.h>

using namespace std;

const int n = 1000000000; 

int main(int argc, char* argv[])
{
	// Compute the run time
	double start_time = omp_get_wtime();

	if (argc < 2)
    {
        cerr << "Usuage: ./program <No. of Thread>" << endl;
        return -1;
    }

	// Specified number of the threads coming from the user
    int thread_count = atoi(argv[1]);
    int chunk = n / thread_count;
    int chunkRemainder = n % thread_count;
    int startIndex = -1, endIndex = -1;

    // Dynamic array initialization
	double* array = new double[n];
	for(int i = 0; i < n; i++)
		array[i] = i;

	double min = array[0];
	double max = array[0];

	//Getting into the parallel region
    #pragma omp parallel firstprivate(startIndex, endIndex) num_threads(thread_count)
    {
    	int myRank = omp_get_thread_num(); //What thread am I?

        // Computing two varibles to buffer through the array which are derivatives from myRank variable
        startIndex = myRank * chunk;
        if((startIndex + chunk) > n)
            endIndex = chunkRemainder;
        else
            endIndex = startIndex + chunk;

        // Local sum variable
        double min_local = array[startIndex];
        double max_local = array[startIndex];
        
        //cout << "Thread Number: " << myRank << ", Start index: " << startIndex << ", and End Index: " << endIndex << endl;
    
        for(int i = startIndex; i <= endIndex; i++)
        {
        	if(array[i]<min_local)
        		min_local=array[i];
			if(array[i]>max_local)
				max_local=array[i];
        }
        
        // Executed one thead at a time
        #pragma omp critical
        {
        	if(min > min_local)
        		min = min_local;
        	if(max < max_local)
        		max = max_local;
    	}
    }

	double end_time = omp_get_wtime();
	
	cout << "N: " << n << endl;
	cout << "Min: " << min << endl;
	cout << "Max: " << max << endl;
	cout << "Took: " << end_time - start_time << endl;
	
	delete[] array;
	return 0;
}