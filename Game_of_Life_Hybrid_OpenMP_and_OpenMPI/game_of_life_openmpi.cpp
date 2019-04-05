/*
 *
 * The Game of Life
 *		- OpenMPI implementation
 *
 * Rules of the game: Adapted from - http://codingdojo.org/kata/GameOfLife/ 
 *
 * Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
 * Any live cell with more than three live neighbours dies, as if by overcrowding.
 * Any live cell with two or three live neighbours lives on to the next generation.
 * Any dead cell with exactly three live neighbours becomes a live cell.
 *
 * @author Md. Ahsan Ayub
 * @version 2.0 04/04/2019 
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <mpi.h>

using namespace std;

// Declaring global grid array
int **iGrid;
//int **iGridProcessCopy;
int **iGridNew;

// Actial values will be initialized after knowing the user defined grid dimension
int iActualRowCount = 0, iActualColumnCount = 0;

// Allocate the arrays
void allocateGrids(int iRowSize, int iColumnSize)
{
	iGrid = new int*[iRowSize];
	iGridNew = new int*[iRowSize]; // A copy of iGrid
	for (int i = 0; i < iRowSize; i++)
	{
		iGrid[i] = new int[iColumnSize];
		iGridNew[i] = new int[iColumnSize];
	}
}

// Add outer layers to the grid
void addOuterLayers()
{
	// Add top layer which is the copy of the iRowCount th stride row-wise.
	for(int i = 1; i <= (iActualColumnCount - 2); i++)
		iGrid[0][i] = iGrid[iActualRowCount - 2][i];

	// Add bottom layer which is the copy of the 1 st stride row-wise.
	for(int i = 1; i <= (iActualColumnCount - 2); i++)
		iGrid[iActualRowCount - 1][i] = iGrid[1][i];

	// Add left layer which is the copy of the iColumnCount st stride column-wise.
	for(int i = 1; i <= (iActualRowCount - 2); i++)
		iGrid[i][0] = iGrid[i][iActualColumnCount - 2];

	// Add right layer which is the copy of the 1 st stride column-wise.
	for(int i = 1; i <= (iActualRowCount - 2); i++)
		iGrid[i][iActualColumnCount-1] = iGrid[i][1];

	// Diagonal copies
	iGrid[0][0] = iGrid[iActualRowCount - 2][iActualColumnCount - 2];	// Upper Left
	iGrid[0][iActualColumnCount - 1] = iGrid[iActualRowCount - 2][1];	// Upper Right
	iGrid[iActualRowCount - 1][0] = iGrid[1][iActualColumnCount - 2];	// Lower Left
	iGrid[iActualRowCount - 1][iActualColumnCount - 1] = iGrid[1][1];	// Lower Right
}

// Print routine of iGrid
void printGrid(int iRowCountStart, int iRowCountEnd, int iColumnCountStart, int iColumnCountEnd)
{
	for(int i = iRowCountStart; i < iRowCountEnd; i++)
	{
		for(int j = iColumnCountStart; j < iColumnCountEnd; j++)
			cout << iGrid[i][j] << "\t";
		cout << endl;
	}
}


// Copy routine to iGrid from iGridNew (to make a new state's copy)
void copyGrid()
{
	for(int i = 0; i < iActualRowCount; i++)
		for(int j = 0; j < iActualColumnCount; j++)
			iGrid[i][j] = iGridNew[i][j];
}

// Measure the neighbors' status of a particular cell
int adjacentNeighbors(int i, int j)
{
	// Variables to calculate the number of alive and dead cells
	int iAliveCount = 0, iDeadCount = 0;

	iGrid[i - 1][j - 1] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i - 1][j] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i - 1][j + 1] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i][j + 1] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i + 1][j + 1] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i + 1][j] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i + 1][j - 1] == 0 ? iDeadCount++ : iAliveCount++;
	iGrid[i][j - 1] == 0 ? iDeadCount++ : iAliveCount++;

	if(iAliveCount < 2)
		// A cell dies of loneliness
		return 0;

	else if(iAliveCount > 3)
		// A cell dies of overcrowding
		return 0;

	else if(iGrid[i][j] == 1 && (iAliveCount == 2 || iAliveCount == 3))
		// Current cell is alive, and there are TWO (which will become three in total) and exactly THREE cells alive.
		// Thus, the cell survives.s
		return 1;

	else if(iGrid[i][j] == 0 && iAliveCount == 3)
		// Current cell is NOT alive; however, there are exactly THREE alive cells: thus, the cell is born.
		return 1;
	
	else
		// Very unlikely this will be executed
		return 0;
}

// Add an outer layer of the whole array for the simplicity

// Main function
int main(int argc, char *argv[])
{
	// Checking the number of input has to be passed by the user
	if (argc < 3)
    {
        printf("Usuage: ./<executable> <input_file> <iterations> <output_file>\n");
        return -1;
    }

    // Getting values from the argument
    ifstream fInput(argv[1]);
    int iGenerations = atoi(argv[2]);
    ofstream fOutput(argv[3]);

    // Checking whether the input file exists in the directory or not
    if (!fInput)
	{
		printf("Error opening the input file.\n");
		return -1;
	}

	// Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Total number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank); // Rank of processes starting from 0 till (world_size - 1)

	// Initializing necessary variables
	int iRowCount, iColumnCount, iNeighborStatus, iChunkSize, iChunkRemainder, iStartRowIndex, iEndRowIndex;
	char cItem;

	// Getting the grid dimension from the first line of the input file
	fInput >> iRowCount >> iColumnCount;
	iActualRowCount = iRowCount + 2; // Two new layers will be added: Top and Bottom
	iActualColumnCount = iColumnCount + 2; // Two new layers will be added: Left and 

	iChunkSize =  iRowCount / world_size; // Calculating the size of the chunk against each processes
	iChunkRemainder = iRowCount % world_size; // Calculating the remainder to deal with the odd number of processes for later on.
	
	// Let all the processes get synchronized
    MPI_Barrier(MPI_COMM_WORLD);

	// Process 0 will initialize the grid and distribute the tasks to other processes
	if(world_rank == 0)
	{
		// Allocate 2D arrays dynamically
		allocateGrids(iActualRowCount, iActualColumnCount);

		// Populating the array from file
		int iCounterRow = 1, iCounterColumn = 1; // Ignoring 0 indexes, as it will later be added as outer layers!
		while(fInput >> cItem)
		{
			// Condition to always maintain the correct index order of the 2D array
			if(iCounterColumn > iColumnCount)
			{
				iCounterRow++;
				iCounterColumn = 1;
			}

			//cout << cItem << "\n";
			iGrid[iCounterRow][iCounterColumn] = cItem - '0';
			//cout << "Row: " << iCounterRow << " , Column: " << iCounterColumn << endl;
			iCounterColumn++;
		}

		// As the file has read properly, now it's time to add outer layers
		addOuterLayers();

		printGrid(0, iActualRowCount, 0, iActualColumnCount);
		cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

		for(int i = 0; i < world_size; i++) 
        {
        	int iSizeOfTheBuffer;

            // Last process will avail the chuck size and the remainder
            if(i == (world_size - 1))
            {
            	// Calculating the starting and end index for each processes
				iStartRowIndex = ((i * iChunkSize) + 1);
				if(iChunkRemainder)	// Last process will consider the remainder
					iChunkSize = iChunkSize + iChunkRemainder;
				iEndRowIndex = iStartRowIndex + iChunkSize;
				
				// Adding outer layer into consideration
				iStartRowIndex -= 1;
				iEndRowIndex += 1;

                // Calculate the buffer size
                iSizeOfTheBuffer = (iEndRowIndex - iStartRowIndex) * iActualColumnCount;

                // Initialzing the grid slice which needs to be passed via MPI Send
                int iSizeOfTheRow = iEndRowIndex - iStartRowIndex;
                int iGridSlice[iSizeOfTheRow][iActualColumnCount];

                // Sending the size of the row
                MPI_Send(&iSizeOfTheRow, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

                cout << "IF block for Process " << i << " | Buffer Size: " << iSizeOfTheBuffer << endl;
                cout << "Start Row Index: " << iStartRowIndex << " | End Row Index: " << iEndRowIndex << endl;

                for(int j = iStartRowIndex, k = 0; j < iEndRowIndex; j++, k++)
                {
                	for(int l = 0; l < iActualColumnCount; l++)
                		iGridSlice[k][l] = iGrid[j][l];
                }

                for(int j = 0; j < iSizeOfTheRow; j++)
                {
                	for(int k = 0; k < iActualColumnCount; k++)
                		cout << iGridSlice[j][k] << "\t";
                	cout << endl;
                }
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

                // Sending the buffer to the last process
                MPI_Send(&(iGridSlice[0][0]), iSizeOfTheBuffer, MPI_INT, i, 1, MPI_COMM_WORLD);
			}            

            // Rest of the other processes (1 to n-1) will avail the chunk size partioning only.
            else 
            {
            	// Calculating the starting and end index for each processes
				iStartRowIndex = ((i * iChunkSize) + 1);
				iEndRowIndex = iStartRowIndex + iChunkSize;
				
				// Adding outer layer into consideration
				iStartRowIndex -= 1;
				iEndRowIndex += 1;

                // Calculate the buffer size
                iSizeOfTheBuffer = (iEndRowIndex - iStartRowIndex) * iActualColumnCount;

                // Initialzing the grid slice which needs to be passed via MPI Send
                int iSizeOfTheRow = iEndRowIndex - iStartRowIndex;
                int iGridSlice[iSizeOfTheRow][iActualColumnCount];

                // Sending the size of the row
                MPI_Send(&iSizeOfTheRow, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

                cout << "IF block for Process " << i << " | Buffer Size: " << iSizeOfTheBuffer << endl;
                cout << "Start Row Index: " << iStartRowIndex << " | End Row Index: " << iEndRowIndex << endl;

                for(int j = iStartRowIndex, k = 0; j < iEndRowIndex; j++, k++)
                {
                	for(int l = 0; l < iActualColumnCount; l++)
                 		iGridSlice[k][l] = iGrid[j][l];
                }
             	
             	for(int j = 0; j < iSizeOfTheRow; j++)
                {
                	for(int k = 0; k < iActualColumnCount; k++)
                		cout << iGridSlice[j][k] << "\t";
                	cout << endl;
                }
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

         		// Sending the buffer to process 1 to process n-1
                MPI_Send(&(iGridSlice[0][0]), iSizeOfTheBuffer, MPI_INT, i, 1, MPI_COMM_WORLD);
            }
        }
	}

	// Done reading from the file
	fInput.close();

	// Let all the processes get synchronized
    MPI_Barrier(MPI_COMM_WORLD);

    // Receiving the size of row first
    MPI_Recv(&iRowCount, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Now that the size of the row is known by each process, allocating grid
	allocateGrids(iRowCount, iActualColumnCount);

	// Calculate the buffer size (for each process)
    int iSizeOfTheBuffer = iRowCount * iActualColumnCount;

    // Create a local copy
	int **iGridLocal;
	iGridLocal = new int*[iRowCount];
	for (int i = 0; i < iRowCount; i++)
		iGridLocal[i] = new int[iActualColumnCount];

	// Time to receive the slice grids
    MPI_Recv(&(iGridLocal[0][0]), iSizeOfTheBuffer, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// Let all the processes get synchronized
    MPI_Barrier(MPI_COMM_WORLD);

	// Initialization of calculated values in the result matrixs
    for(int i = 0; i < iRowCount; i++)
    {
        for(int j = 0; j < iActualColumnCount; j++)
        {
            cout << iGridLocal[i][j] << "\t";
        }
        cout << endl;
    }

    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    
	/*
	// All set for the game
	for(int iSteps = 1; iSteps <= iGenerations; iSteps++)
	{
		for(int i = 1; i <= iRowCount; i++)
		{
			for(int j = 1; j <= iColumnCount; j++)
			{
				iGridNew[i][j] = adjacentNeighbors(i, j);
			}
		}
		// Time to create a copy of the new generated state to the older one
		// as a reference to create the newer one in the next generation
		copyGrid();

		cout << "+++++ Generation " << iSteps << " +++++" << endl;
		
		// Print the new generation state
		printGrid(1, iRowCount + 1, 1, iColumnCount + 1);
	}

	// Write the final state to the 
	for(int i = 1; i <= iRowCount; i++)
	{
		for (int j = 1; j <= iColumnCount; j++)
			fOutput << iGrid[i][j] << " ";

		fOutput << endl;
	}

	cout << "Last generation output has been written to file." << endl;
	// Done writing to the file
	fOutput.close();*/

	// Finalize the MPI environment.
    MPI_Finalize();
	return 0;
}