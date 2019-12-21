#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <omp.h>

using namespace std;

const int N = 20000000; 

int is_prime(int n)
{
	if(n < 2)
		return 0;
	
	int sqrt_n = sqrt(n);
	
	for(int i = 2; i <= sqrt_n; i++)
		if(n % i == 0)
			return 0;
	return 1;
}

int main(int argc, char* argv[])
{
	// Compute the run time
	double start_time = omp_get_wtime();
	
	if (argc < 2)
    {
        cerr << "Usuage: ./program <No. of Thread>" << endl;
        return -1;
    }

	// Variable to store number of primes until n
	int num_primes = 0;

	// Specified number of the threads coming from the user
    int thread_count = atoi(argv[1]);
    int chunk = N / thread_count;
    int chunkRemainder = N % thread_count;
    int startIndex = -1, endIndex = -1;

    //Getting into the parallel region
    #pragma omp parallel firstprivate(startIndex, endIndex) num_threads(thread_count)
    {
    	int myRank = omp_get_thread_num(); //What thread am I?

        // Computing two varibles to buffer through the array which are derivatives from myRank variable
        startIndex = myRank * chunk;
        if((startIndex + chunk) > N)
            endIndex = chunkRemainder;
        else
            endIndex = startIndex + chunk;

        int num_primes_local = 0;
        
        cout << "Thread Number: " << myRank << ", Start index: " << startIndex << ", and End Index: " << endIndex << endl;
    
        for(int i = startIndex; i <= endIndex; i++)
            num_primes_local += is_prime(i);
        
        // Executed one thead at a time
        #pragma omp critical
        {
        	num_primes += num_primes_local;
    	}
    }
	
	double end_time = omp_get_wtime();
	
	cout << "N: " << N << endl;
	cout << "Primes: " << num_primes << endl;
	cout << "Took: " << end_time - start_time << endl;

	return 0;
}