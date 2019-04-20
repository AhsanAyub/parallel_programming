# Project Title
Conway's Game of Life | Implemented with MPI and OpenMP - A Hybrid Approach

### Description
The game of life, also known as Life, is a cellular automation devised by the British mathematician John H Conway in 1970. The game starts from an initial state, and then it requires no further input from the user which makes it a zero-player game. The basic idea is to start with a simple configuration of counters (organisms), one to a cell, then observe how it changes as one applies the following Conway's "genetic laws" for births, deaths, and survivals.

- Any live cell with fewer than two live neighbors dies, as if caused by underpopulation.
- Any live cell with more than three live neighbors dies, as if by overcrowding.
- Any live cell with two or three live neighbors lives on to the next generation.
- Any dead cell with exactly three live neighbors becomes a live cell.

A program has been built to implement Conway’s game of life as a hybrid approach: MPI and Open MP. The program takes the initial state as an input from the user, and then it writes the last generation (which is also defined by the user) state as an output file.

### Prerequisites

- OpenMPI Library
- OpenMP Library
- GNU Library

### Execution

A step by step series of examples that tell you how to get a development env running

```
Step 1. Transfer the cpp files and Makefile in the HPC cluster
Step 2. command: make coompile
Step 3. command: make run
Step 4. command: make clean
```

### Example
```.. code-block:: console
	$ hpcshell --ntasks-per-node=2 --cpus-per-task=2
	$ make compile
	mpic++ -fopenmp -o hybrid game_of_life_hybrid.cpp
	$ make run
	mpirun -np 2 ./hybrid 10000by10000_0.txt 2 2 output.txt
	....
	....
	//A lot of text
	$ make clean
	rm hybrid
```
