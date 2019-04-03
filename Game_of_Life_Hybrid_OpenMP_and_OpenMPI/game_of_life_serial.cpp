/*
 *
 * The Game of Life
 *		- a serial implementation
 *
 * Rules of the game: Adapted from - http://codingdojo.org/kata/GameOfLife/ 
 *
 * Any live cell with fewer than two live neighbours dies, as if caused by underpopulation.
 * Any live cell with more than three live neighbours dies, as if by overcrowding.
 * Any live cell with two or three live neighbours lives on to the next generation.
 * Any dead cell with exactly three live neighbours becomes a live cell.
 *
 * @author Md. Ahsan Ayub
 * @version 1.1 04/02/2019 
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>

using namespace std;

// Declaring global grid array
int **iGrid;
int **iGridNew;

// Actial values will be initialized after knowing the user defined grid dimension
int iActualRowCount = 0, iActualColumnCount = 0;

// Allocate the arrays
void allocateGrids()
{
	iGrid = new int*[iActualRowCount];
	iGridNew = new int*[iActualRowCount]; // A copy of iGrid
	for (int i = 0; i < iActualRowCount; i++)
	{
		iGrid[i] = new int[iActualColumnCount];
		iGridNew[i] = new int[iActualColumnCount];
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

	// Initializing necessary variables
	int iRowCount, iColumnCount, iNeighborStatus;
	char cItem;

	// Getting the grid dimension from the first line of the input file
	fInput >> iRowCount >> iColumnCount;
	iActualRowCount = iRowCount + 2; // Two new layers will be added: Top and Bottom
	iActualColumnCount = iColumnCount + 2; // Two new layers will be added: Left and Right

	// Allocate 2D arrays dynamically
	allocateGrids();

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
		//cout << "Row: " << iCounterRow << " , Column: " << (iCounterColumn % iColumnCount) << endl;
		iCounterColumn++;
	}

	// Done reading from the file
	fInput.close();

	// As the file has read properly, now it's time to add outer layers
	addOuterLayers();

	// Print the grid before going into the generations
	cout << "===== Given State =====" << endl;
	printGrid(1, iRowCount + 1, 1, iColumnCount + 1);

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
	fOutput.close();

	return 0;
}