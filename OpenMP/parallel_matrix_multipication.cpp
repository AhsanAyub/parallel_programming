/*
 *    parallel_matrix_multipication.cpp
 *    Purpose: Calculation of the runtime of squrate matrix multiplicaiton using OpenMP
 *
 *    @author Md. Ahsan Ayub
 *    @version 1.5 02/12/2019 
 */

#include <iostream>
#include <cmath>
#include <omp.h>
#include <cstring>
#include <cstdlib>

using namespace std;

// Printing function of the element of the matrix
void printArray(int **arr, int colSize, int rowSize)
{
    for(int i = 0; i < colSize; i++)
    {
        for(int j = 0; j < rowSize; j++)
            cout << arr[i][j] << "\t";
        cout << endl;
    }    
}

int main (int argc, char *argv[])
{
    int index = 0, thread_count = 1; // Initial declaration of dynamic index (coming from the user)

    if (argc < 3)
    {
        cerr << "Usuage: ./program <No. of Square Matrix Size> <No. of Thread>" << endl;
        return -1;
    }

    // Specified number of the threads coming from the user
    thread_count  = atoi(argv[2]);
    index = atoi(argv[1]);

    //Creating dynanic array
    int **matrix, **result;
    matrix = new int* [index]; //Defining the number of the rows (1D Array)
    result = new int* [index]; //The size of the square matrix multiplicaiton will the same
    
    // This loop will be responsible to create multile columns
    for(int i = 0; i < index; i++)
    {
        matrix[i] = new int[index];
        result[i] = new int[index];
    }

    // Array element initialization (Static)
    for(int i = 0; i < index; i++)
        for(int j = 0; j < index; j++)
            //Defining each array element as 2
            matrix[i][j] = 2;

    int chunk = index / thread_count;
    int chunkRemainder = index % thread_count;
    int startIndex = -1, endIndex = -1;

    // Compute the start of the runtime of matrix multiplication only.
    double wallTimeStart = omp_get_wtime();

    //Getting into the parallel region
    #pragma omp parallel firstprivate(startIndex, endIndex) num_threads(thread_count)
    {
        int myRank = omp_get_thread_num(); //What thread am I?

        // Computing two varibles to buffer through the array which are derivatives from myRank variable
        startIndex = myRank * chunk;
        if((startIndex + chunk) > index)
            endIndex = chunkRemainder;
        else
            endIndex = startIndex + chunk;

        for(int i = startIndex; i < endIndex; i++)
        {
            for(int j = 0; j < index; j++)
            {
                int sum = 0;
                for(int bufferIndex = 0; bufferIndex < index; bufferIndex++)
                    sum +=  matrix[j][bufferIndex] * matrix[bufferIndex][j];
                result[i][j] = sum;
            }
        }
    }

    // Compute the end of the runtime of matrix multiplication only.
    double wallTimeEnd = omp_get_wtime();

    // Displaying the code run time
    cout << "Matrix Size: " << index << "\tThread: " << thread_count << "\tCode compilation time: " << (wallTimeEnd - wallTimeStart) << endl;

    /* // Printing array element
    printArray(result, index, index);
    */

    // Deallocate memory of the dynamically created 2D arrays
    memset(result, 0, index);
    memset(matrix, 0, index);

    return 0;
}
